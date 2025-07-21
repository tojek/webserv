/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigTokens.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mwojtcza <mwojtcza@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 12:48:15 by kkonarze          #+#    #+#             */
/*   Updated: 2025/07/16 15:45:29 by mwojtcza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include <sstream>

void ConfigParser::read_listen(int line_num)
{
	std::string listen_value;
	size_t		semicolon_pos = remainder.find(';');
	size_t		colon_pos;

	if (block_num == 0)
		parser_error("Not inside server block.", line_num);
	if (semicolon_pos == std::string::npos)
		parser_error("Missing ';' after 'listen' directive.", line_num);

	listen_value = remainder.substr(0, semicolon_pos);
	trim_whitespace(listen_value);

	colon_pos = listen_value.find(':');

	ListenConfig config;

	config.host = (colon_pos == std::string::npos)? "0.0.0.0" : listen_value.substr(0, colon_pos);
	config.port = string_to_int((colon_pos == std::string::npos)? listen_value : listen_value.substr(colon_pos + 1));
	listen_configs.push_back(config);
}

void ConfigParser::read_server_name(int line_num)
{
	std::string	trimmed_name;
	size_t		semicolon_pos = remainder.find(';');

	if (block_num == 0)
		parser_error("Not inside server block.", line_num);
	if (semicolon_pos == std::string::npos)
		parser_error("Missing ';' after 'server_name' directive.", line_num);

	trimmed_name = remainder.substr(0, semicolon_pos);
	trim_whitespace(trimmed_name);
	this->server_name = trimmed_name;
}

void ConfigParser::read_client_max_body_size(int line_num)
{
	std::string	size_str;
	size_t		semicolon_pos = remainder.find(';');

	if (block_num == 0)
		parser_error("Not inside server block.", line_num);
	if (semicolon_pos == std::string::npos)
		parser_error("Missing ';' after 'client_max_body_size' directive.", line_num);

	size_str = remainder.substr(0, semicolon_pos);
	trim_whitespace(size_str);
	client_max_body_size = parse_size(size_str);
}

void ConfigParser::read_error_page(int line_num)
{
	std::string	error_page_str;
	std::string	error_code_str;
	std::string	error_page_path;
	size_t		space_pos;
	size_t		semicolon_pos = remainder.find(';');
	int			error_code;

	if (block_num == 0)
		parser_error("Not inside server block.", line_num);
	if (semicolon_pos == std::string::npos)
		parser_error("Missing ';' after 'error_page' directive.", line_num);

	error_page_str = remainder.substr(0, semicolon_pos);
	trim_whitespace(error_page_str);
	space_pos = error_page_str.find(' ');
	if (space_pos == std::string::npos)
		parser_error("Invalid 'error_page' format. Expected 'error_page CODE PATH;'", line_num);

	error_code_str = error_page_str.substr(0, space_pos);
	error_page_path = error_page_str.substr(space_pos + 1);
	trim_whitespace(error_code_str);
	trim_whitespace(error_page_path);

	error_code = string_to_int(error_code_str);
	error_pages[error_code] = error_page_path;
}

void ConfigParser::read_location(int line_num)
{
    size_t open_brace_pos = remainder.find('{');

	if (block_num == 0)
		parser_error("Not inside server block.", line_num);
    if (open_brace_pos == std::string::npos)
        parser_error("Missing '{' after 'location' directive.", line_num);

    location_path = remainder.substr(0, open_brace_pos);
    trim_whitespace(location_path);

    locations[location_path] = std::map<std::string, std::string>();

    block_num++;
}

void ConfigParser::map_location(int line_num)
{
	std::string value;
	size_t semicolon_pos = remainder.find(';');

	if (semicolon_pos == std::string::npos)
		parser_error("Missing ';' after '" + token + "' directive in location block.", line_num);

	value = remainder.substr(0, semicolon_pos);
	trim_whitespace(value);

	locations[location_path][token] = value;
}

void ConfigParser::read_server(int line_num)
{
	if (block_num != 0)
		parser_error("Server block inside of server block on line: ", line_num);
	if (remainder != "{")
		parser_error("Expected 'server {'", line_num);
	block_num++;
}

void ConfigParser::read_root(int line_num)
{
	std::string	root_value;
	size_t		semicolon_pos = remainder.find(';');

	if (block_num == 0)
		parser_error("Not inside server block.", line_num);
	if (semicolon_pos == std::string::npos)
		parser_error("Missing ';' after 'root' directive.", line_num);

	root_value = remainder.substr(0, semicolon_pos);
	trim_whitespace(root_value);

	// If inside a location block, store the root directive in the location map
	if (block_num == 2)
		locations[location_path]["root"] = root_value;
}

void ConfigParser::read_index(int line_num)
{
	std::string	index_value;
	size_t		semicolon_pos = remainder.find(';');

	if (block_num == 0)
		parser_error("Not inside server block.", line_num);
	if (semicolon_pos == std::string::npos)
		parser_error("Missing ';' after 'index' directive.", line_num);

	index_value = remainder.substr(0, semicolon_pos);
	trim_whitespace(index_value);

	if (block_num == 2)
		locations[location_path]["index"] = index_value;
}

void ConfigParser::validate_methods(const std::string& methods, int line_num)
{
	// Split the methods string into individual methods
	std::string method;
	std::istringstream methods_stream(methods);
	bool has_valid_method = false;

	// Validate methods
	if (methods.empty())
		parser_error("Empty 'allowed_methods' directive.", line_num);

	while (methods_stream >> method)
	{
		if (method != "GET" && method != "POST" && method != "DELETE")
			parser_error("Invalid method '" + method + "' in 'allowed_methods' directive.", line_num);
		has_valid_method = true;
	}

	if (!has_valid_method)
		parser_error("No valid methods in 'allowed_methods' directive.", line_num);
}

void ConfigParser::read_allowed_methods(int line_num)
{
	std::string	methods_value;
	size_t		semicolon_pos = remainder.find(';');

	if (block_num == 0)
		parser_error("Not inside server block.", line_num);
	if (semicolon_pos == std::string::npos)
		parser_error("Missing ';' after 'allowed_methods' directive.", line_num);

	methods_value = remainder.substr(0, semicolon_pos);
	trim_whitespace(methods_value);

	validate_methods(methods_value, line_num);

	// Store the allowed methods in the location map
	if (block_num == 2)
		locations[location_path]["allowed_methods"] = methods_value;
}

void ConfigParser::read_upload_dir(int line_num)
{
	std::string	upload_dir_value;
	size_t		semicolon_pos = remainder.find(';');

	if (block_num == 0)
		parser_error("Not inside server block.", line_num);
	if (semicolon_pos == std::string::npos)
		parser_error("Missing ';' after 'upload_dir' directive.", line_num);

	upload_dir_value = remainder.substr(0, semicolon_pos);
	trim_whitespace(upload_dir_value);

	if (upload_dir_value.empty())
		parser_error("Empty 'upload_dir' directive.", line_num);

	// Store the upload directory in the location map
	if (block_num == 2)
		locations[location_path]["upload_dir"] = upload_dir_value;
}

void ConfigParser::read_cgi_extension(int line_num)
{
	std::string	cgi_extension_value;
	size_t		semicolon_pos = remainder.find(';');

	if (block_num == 0)
		parser_error("Not inside server block.", line_num);
	if (semicolon_pos == std::string::npos)
		parser_error("Missing ';' after 'cgi_extension' directive.", line_num);

	cgi_extension_value = remainder.substr(0, semicolon_pos);
	trim_whitespace(cgi_extension_value);

	if (cgi_extension_value.empty())
		parser_error("Empty 'cgi_extension' directive.", line_num);

	// Store the CGI extension in the location map
	if (block_num == 2)
		locations[location_path]["cgi_extension"] = cgi_extension_value;
}

void ConfigParser::read_directory_listing(int line_num)
{
	std::string	directory_listing_value;
	size_t		semicolon_pos = remainder.find(';');

	if (block_num == 0)
		parser_error("Not inside server block.", line_num);
	if (semicolon_pos == std::string::npos)
		parser_error("Missing ';' after 'directory_listing' directive.", line_num);

	directory_listing_value = remainder.substr(0, semicolon_pos);
	trim_whitespace(directory_listing_value);

	if (directory_listing_value != "on" && directory_listing_value != "off")
		parser_error("Invalid 'directory_listing' value. Expected 'on' or 'off'.", line_num);

	// Store the directory listing setting in the location map
	if (block_num == 2)
		locations[location_path]["directory_listing"] = directory_listing_value;
}

void ConfigParser::fill_tokens()
{
	tokens["listen"] = &ConfigParser::read_listen;
	tokens["server_name"] = &ConfigParser::read_server_name;
	tokens["client_max_body_size"] = &ConfigParser::read_client_max_body_size;
	tokens["error_page"] = &ConfigParser::read_error_page;
	tokens["location"] = &ConfigParser::read_location;
	tokens["default"] = &ConfigParser::map_location;
	tokens["server"] = &ConfigParser::read_server;
	tokens["root"] = &ConfigParser::read_root;
	tokens["index"] = &ConfigParser::read_index;
	tokens["allowed_methods"] = &ConfigParser::read_allowed_methods;
	tokens["upload_dir"] = &ConfigParser::read_upload_dir;
	tokens["cgi_extension"] = &ConfigParser::read_cgi_extension;
	tokens["directory_listing"] = &ConfigParser::read_directory_listing;
}

