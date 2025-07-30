/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigTokens.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkonarze <kkonarze@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 12:48:15 by kkonarze          #+#    #+#             */
/*   Updated: 2025/07/29 05:26:32 by kkonarze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"
#include "Webserv.hpp"
#include "Location.hpp" 
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

	conf.host = (colon_pos == std::string::npos)? "0.0.0.0" : listen_value.substr(0, colon_pos);
	conf.port = string_to_int((colon_pos == std::string::npos)? listen_value : listen_value.substr(colon_pos + 1));
	//listen_configs.push_back(config);
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
	this->conf.server_name = trimmed_name;
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
	conf.client_max_body_size = parse_size(size_str);
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
	conf.error_pages[error_code] = error_page_path;
}

void ConfigParser::read_location(int line_num)
{
    size_t		open_brace_pos = remainder.find('{');
	std::string	location_path;

	if (block_num == 0)
		parser_error("Not inside server block.", line_num);
    if (open_brace_pos == std::string::npos)
        parser_error("Missing '{' after 'location' directive.", line_num);

    location_path = remainder.substr(0, open_brace_pos);
    trim_whitespace(location_path);

	conf.locations.push_back(Location(location_path));
    block_num++;
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

	if (block_num == 2)
		conf.locations.back().add_token("root", root_value);
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
		conf.locations.back().add_token("index", index_value);
}

void ConfigParser::fill_tokens()
{
	tokens["listen"] = &ConfigParser::read_listen;
	tokens["server_name"] = &ConfigParser::read_server_name;
	tokens["client_max_body_size"] = &ConfigParser::read_client_max_body_size;
	tokens["error_page"] = &ConfigParser::read_error_page;
	tokens["location"] = &ConfigParser::read_location;
	tokens["server"] = &ConfigParser::read_server;
	tokens["root"] = &ConfigParser::read_root;
	tokens["index"] = &ConfigParser::read_index;
}

