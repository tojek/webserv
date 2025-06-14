#include "webserv_config.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstddef>
#include <string>

static void parser_error(const std::string& message, int line_num = -1) {
    if (line_num >= 0) {
        std::cerr << "Parser Error [Line " << line_num << "]: " << message << std::endl;
    } else {
        std::cerr << "Parser Error: " << message << std::endl;
    }
    exit(EXIT_FAILURE);
}

static void trim_whitespace(std::string& s) {
    size_t start = s.find_first_not_of(" \t\n\r\f\v");
    if (std::string::npos == start) { // all whitespace
        s.clear();
        return;
    }
    size_t end = s.find_last_not_of(" \t\n\r\f\v");
    s = s.substr(start, (end - start + 1));
}

static void remove_comment(std::string& s) {
    size_t comment_pos = s.find('#');
    if (comment_pos != std::string::npos) {
        s = s.substr(0, comment_pos);
    }
}

static int string_to_int(const std::string& s) {
    std::istringstream iss(s); // array of s strings
    int num;
    iss >> num; // Tries to extract an int from array
    if (iss.fail() || !iss.eof()) { // fail if fail or not int
        parser_error("Invalid integer value '" + s + "'");
    }
    return num;
}

// Returns the size in bytes
static size_t parse_size(const std::string& s) {
    size_t len = s.length();
    if (len == 0) {
        parser_error("Empty size value");
    }

    char unit = s[len-1];
    size_t multiplier = 1;
    std::string num_str;

    // Check if the last character is a unit
    if (unit == 'k' || unit == 'K') {
        multiplier = 1024;
        num_str = s.substr(0, len-1);
    } else if (unit == 'm' || unit == 'M') {
        multiplier = 1024 * 1024;
        num_str = s.substr(0, len-1);
    } else if (unit == 'g' || unit == 'G') {
        multiplier = 1024 * 1024 * 1024;
        num_str = s.substr(0, len-1);
    } else if (isdigit(unit)) {
        num_str = s;
    } else {
        parser_error("Invalid size unit '" + std::string(1, unit) + "'");
    }

    // atoi
    try {
        int num = string_to_int(num_str);
        if (num < 0) {
            parser_error("Size cannot be negative: '" + s + "'");
        }
        return num * multiplier;
    } catch (...) {
        parser_error("Invalid size format '" + s + "'");
    }
    return 0;
}

WebservConfig parse_config_file(const std::string& filepath) {
    WebservConfig config;
    std::ifstream file(filepath.c_str());
    std::string line;
    int line_num = 0;
    bool in_server_block = false;

    if (!file.is_open()) {
        parser_error("Could not open configuration file '" + filepath + "'");
    }

    while (std::getline(file, line))
	{
        line_num++;
        remove_comment(line);
        trim_whitespace(line);

        if (line.empty())
            continue;

        // --- Tokenization ---
        // Find the first space to split token and rest of line
        size_t first_space = line.find_first_of(" \t");
        std::string token;
        std::string remainder;

        if (first_space == std::string::npos)
		{
            token = line;
            remainder = "";
        }
		else
		{
            token = line.substr(0, first_space);
            remainder = line.substr(first_space);
            trim_whitespace(remainder);
        }

        if (!in_server_block)
		{
            if (token == "server")
			{
                if (remainder == "{")
                    in_server_block = true;
				else
                    parser_error("Expected '{' after 'server'", line_num);
            }
			else
			{
                parser_error("Unexpected token '" + token + "'. Expected 'server'.", line_num);
            }
        }
		else
		{ // inside a server block
            if (token == "listen")
			{
                // Expect the next part to be the port or host:port
                size_t semicolon_pos = remainder.find(';');
                if (semicolon_pos == std::string::npos)
				{
                    parser_error("Missing ';' after 'listen' directive.", line_num);
                }
                std::string listen_value = remainder.substr(0, semicolon_pos);
                trim_whitespace(listen_value);

                size_t colon_pos = listen_value.find(':');

                if (colon_pos == std::string::npos)
				{ // Only port
                    config.main_server.host = "0.0.0.0"; // default host
                    config.main_server.port = string_to_int(listen_value);
                }
				else
				{ // host:port specified
                    config.main_server.host = listen_value.substr(0, colon_pos);
                    config.main_server.port = string_to_int(listen_value.substr(colon_pos + 1));
                }

            }
			else if (token == "server_name")
			{
                size_t semicolon_pos = remainder.find(';');
                if (semicolon_pos == std::string::npos) {
                    parser_error("Missing ';' after 'server_name' directive.", line_num);
                }
                std::string server_name = remainder.substr(0, semicolon_pos);
                trim_whitespace(server_name);
                config.main_server.server_name = server_name;
            }
			else if (token == "client_max_body_size")
			{
                size_t semicolon_pos = remainder.find(';');
                if (semicolon_pos == std::string::npos) {
                    parser_error("Missing ';' after 'client_max_body_size' directive.", line_num);
                }
                std::string size_str = remainder.substr(0, semicolon_pos);
                trim_whitespace(size_str);
                config.main_server.client_max_body_size = parse_size(size_str);
            }
			else if (token == "error_page")
			{
                size_t semicolon_pos = remainder.find(';');
                if (semicolon_pos == std::string::npos) {
                    parser_error("Missing ';' after 'error_page' directive.", line_num);
                }
                std::string error_page_str = remainder.substr(0, semicolon_pos);
                trim_whitespace(error_page_str);

                // Extract error code and page path
                size_t space_pos = error_page_str.find(' ');
                if (space_pos == std::string::npos) {
                    parser_error("Invalid 'error_page' format. Expected 'error_page CODE PATH;'", line_num);
                }

                std::string error_code_str = error_page_str.substr(0, space_pos);
                std::string error_page_path = error_page_str.substr(space_pos + 1);
                trim_whitespace(error_code_str);
                trim_whitespace(error_page_path);

                int error_code = string_to_int(error_code_str);
                config.main_server.error_pages[error_code] = error_page_path;

            }
			else if (token == "}")
			{
                in_server_block = false; // exit server block
            }
			else
			{
                parser_error("Unknown directive '" + token + "' inside server block.", line_num);
            }
        }
    }

    file.close();

    if (in_server_block) {
        parser_error("Unexpected EOF. Missing '}' for server block.");
    }

    return config;
}
