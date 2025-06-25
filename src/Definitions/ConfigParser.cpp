/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkonarze <kkonarze@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 12:48:11 by kkonarze          #+#    #+#             */
/*   Updated: 2025/06/25 13:35:37 by kkonarze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstddef>
#include <string>

/**
 * Prints out a error message for parser and exits the program.
 * 
 * @param message message to print on cerr.
 * @param line_num config file line on which there is error.
 */
__attribute__((noreturn)) void ConfigParser::parser_error(const std::string& message, int line_num = -1)
{
    if (line_num >= 0)
        std::cerr << "Parser Error [Line " << line_num << "]: " << message << std::endl;
    else
        std::cerr << "Parser Error: " << message << std::endl;
    exit(EXIT_FAILURE);
}

/**
 * Converts string to byte amount while units. 
 * 
 * @param s string reference to byte amount.
 * @return Byte amount multiplied by units.
 */
size_t ConfigParser::parse_size(const std::string& s)
{
	size_t		multiplier = 1;
	size_t		len = s.length();
    std::string	num_str;
	char		unit;
    int			num;

    if (len == 0)
        parser_error("Empty size value");
    unit = s[len-1];
	switch (unit)
	{
		case 'k': case 'K':
			multiplier = 1024;
			num_str = s.substr(0, len-1);
			break;
		case 'm': case 'M':
			multiplier = 1024 * 1024;
			num_str = s.substr(0, len-1);
			break;
		case 'g': case 'G':
			multiplier = 1024 * 1024 * 1024;
			num_str = s.substr(0, len-1);
			break;
		default:
			if (!isdigit(unit))
				parser_error("Invalid size unit '" + std::string(1, unit) + "'");
			num_str = s;
			break;
	}
    try {
        num = string_to_int(num_str);
        if (num < 0)
            parser_error("Size cannot be negative: '" + s + "'");
        return num * multiplier;
    } catch (...) {
        parser_error("Invalid size format '" + s + "'");
    }
}

void ConfigParser::tokenize(const std::string& line, std::string& token, std::string& remainder)
{
	size_t		first_space;

	first_space = line.find_first_of(" \t");
	token = (first_space == std::string::npos)? line : line.substr(0, first_space);
	remainder = (first_space == std::string::npos)? "" : line.substr(first_space);
	trim_whitespace(token);
	trim_whitespace(remainder);
}

ConfigParser::~ConfigParser()
{

}

ConfigParser::ConfigParser(const std::string& filepath) {
    std::ifstream	file(filepath.c_str());
    std::string		line;
    std::string		token;
    std::string		remainder;
	bool			in_server_block = false;
    int				line_num = 0;

    if (!file.is_open()) {
        parser_error("Could not open configuration file '" + filepath + "'");
    }
	fill_tokens();
    while (std::getline(file, line))
	{
        line_num++;
        remove_comment(line);
        trim_whitespace(line);
        if (line.empty())
            continue;

		tokenize(line, token, remainder);
        if (!in_server_block)
		{
            if (token != "server" && remainder != "{")
				parser_error("Expected 'server {'", line_num);
            in_server_block = true;
			continue ;
        }
		if (tokens.count(token))
			(this->*tokens[token])(remainder, line_num);
		else if (token != "}")
			parser_error("Unknown directive '" + token + "' inside server block.", line_num);
		else
			in_server_block = false;
    }
    file.close();
    if (in_server_block)
        parser_error("Unexpected EOF. Missing '}' for server block.");
}

const std::string& ConfigParser::get_host() const
{
	return host;
}

const std::string& ConfigParser::get_server_name() const
{
	return server_name;
}

int ConfigParser::get_port() const
{
	return port;
}

size_t ConfigParser::get_client_max_body_size() const
{
	return client_max_body_size;
}

std::map<int, std::string> ConfigParser::get_error_pages() const
{
	return error_pages;
}

