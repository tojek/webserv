/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationTokens.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkonarze <kkonarze@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 03:50:24 by kkonarze          #+#    #+#             */
/*   Updated: 2025/07/29 04:21:05 by kkonarze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"
#include <iostream>
#include <sstream>

void Location::read_allowed_methods(int line_num, std::string& reminder)
{
	std::string	methods_value;
	size_t		semicolon_pos = reminder.find(';');

	if (semicolon_pos == std::string::npos)
		parser_error("Missing ';' after 'allowed_methods' directive.", line_num);

	methods_value = reminder.substr(0, semicolon_pos);
	trim_whitespace(methods_value);

	validate_methods(line_num, methods_value);

	directive["allowed_methods"] = methods_value;
}

void Location::validate_methods(int line_num, std::string& reminder)
{
	std::string method;
	std::istringstream methods_stream(reminder);
	bool has_valid_method = false;

	if (reminder.empty())
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

void Location::read_upload_dir(int line_num, std::string& reminder)
{
	std::string	upload_dir_value;
	size_t		semicolon_pos = reminder.find(';');

	if (semicolon_pos == std::string::npos)
		parser_error("Missing ';' after 'upload_dir' directive.", line_num);

	upload_dir_value = reminder.substr(0, semicolon_pos);
	trim_whitespace(upload_dir_value);

	if (upload_dir_value.empty())
		parser_error("Empty 'upload_dir' directive.", line_num);

	directive["upload_dir"] = upload_dir_value;
}

void Location::read_cgi_extension(int line_num, std::string& reminder)
{
	std::string	cgi_extension_value;
	size_t		semicolon_pos = reminder.find(';');

	if (semicolon_pos == std::string::npos)
		parser_error("Missing ';' after 'cgi_extension' directive.", line_num);

	cgi_extension_value = reminder.substr(0, semicolon_pos);
	trim_whitespace(cgi_extension_value);

	if (cgi_extension_value.empty())
		parser_error("Empty 'cgi_extension' directive.", line_num);

	directive["cgi_extension"] = cgi_extension_value;
}

void Location::read_directory_listing(int line_num, std::string& reminder)
{
	std::string	directory_listing_value;
	size_t		semicolon_pos = reminder.find(';');

	if (semicolon_pos == std::string::npos)
		parser_error("Missing ';' after 'directory_listing' directive.", line_num);

	directory_listing_value = reminder.substr(0, semicolon_pos);
	trim_whitespace(directory_listing_value);

	if (directory_listing_value != "on" && directory_listing_value != "off")
		parser_error("Invalid 'directory_listing' value. Expected 'on' or 'off'.", line_num);
	
	directive["directory_listing"] = directory_listing_value;
}
