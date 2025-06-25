/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigTokens.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkonarze <kkonarze@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 12:48:15 by kkonarze          #+#    #+#             */
/*   Updated: 2025/06/25 13:23:33 by kkonarze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

void ConfigParser::read_listen(const std::string& remainder, int line_num)
{
	std::string listen_value;
	size_t		semicolon_pos = remainder.find(';');
	size_t		colon_pos;

	if (semicolon_pos == std::string::npos)
		parser_error("Missing ';' after 'listen' directive.", line_num);
	listen_value = remainder.substr(0, semicolon_pos);
	trim_whitespace(listen_value);

	colon_pos = listen_value.find(':');
	host = (colon_pos == std::string::npos)? "0.0.0.0" : listen_value.substr(0, colon_pos);
	port = string_to_int((colon_pos == std::string::npos)? listen_value : listen_value.substr(colon_pos + 1));
}

void ConfigParser::read_server_name(const std::string& remainder, int line_num)
{
	std::string	trimmed_name;
	size_t		semicolon_pos = remainder.find(';');

	if (semicolon_pos == std::string::npos)
		parser_error("Missing ';' after 'server_name' directive.", line_num);

	trimmed_name = remainder.substr(0, semicolon_pos);
	trim_whitespace(trimmed_name);
	this->server_name = trimmed_name;
}

void ConfigParser::read_client_max_body_size(const std::string& remainder, int line_num)
{
	std::string	size_str;
	size_t		semicolon_pos = remainder.find(';');

	if (semicolon_pos == std::string::npos)
		parser_error("Missing ';' after 'client_max_body_size' directive.", line_num);

	size_str = remainder.substr(0, semicolon_pos);
	trim_whitespace(size_str);
	client_max_body_size = parse_size(size_str);
}

void ConfigParser::read_error_page(const std::string& remainder, int line_num)
{
	std::string	error_page_str;
	std::string	error_code_str;
	std::string	error_page_path;
	size_t		space_pos;
	size_t		semicolon_pos = remainder.find(';');
	int			error_code;

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

void ConfigParser::fill_tokens()
{
	tokens["listen"] = &ConfigParser::read_listen;
	tokens["server_name"] = &ConfigParser::read_server_name;
	tokens["client_max_body_size"] = &ConfigParser::read_client_max_body_size;
	tokens["error_page"] = &ConfigParser::read_error_page;
}
