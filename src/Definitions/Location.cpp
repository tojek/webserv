/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkonarze <kkonarze@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 04:02:09 by kkonarze          #+#    #+#             */
/*   Updated: 2025/07/29 05:25:48 by kkonarze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"
#include <iostream>
#include <fstream>
#include <cstdlib>

/**
 * Finds directive of location and adds it.
 *
 * @param line_num config file line on which token is.
 * @param token directive which should be added.
 * @param reminder reminder used for the directive.
 */
void Location::find_token(int line_num, std::string& token, std::string& reminder)
{
	if (tokens.count(token))
		(this->*tokens[token])(line_num, reminder);
	else
		parser_error("Unknown location directive.", line_num);
}

/**
 * Prints out a error message for parser and exits the program.
 *
 * @param message message to print on cerr.
 * @param line_num config file line on which there is error.
 */
__attribute__((noreturn)) void Location::parser_error(const std::string message, int line_num = -1)
{
	if (line_num >= 0)
		std::cerr << "Parser Error [Line " << line_num << "]: " << message << std::endl;
	else
		std::cerr << "Parser Error: " << message << std::endl;
	exit(EXIT_FAILURE);
}

/**
 * Adds token to location.
 *
 * @param token directive which should be added.
 * @param value value used for the directive.
 */
void Location::add_token(std::string token, std::string& value)
{
	directive[token] = value;
}

void	Location::fill_tokens()
{
	tokens["allowed_methods"] = &Location::read_allowed_methods;
	tokens["upload_dir"] = &Location::read_upload_dir;
	tokens["cgi_extension"] = &Location::read_cgi_extension;
	tokens["directory_listing"] = &Location::read_directory_listing;
}

Location::Location(std::string& location_path)
{
	fill_tokens();
	this->location_path = location_path;
}

Location::Location()
{
	
}
Location::~Location()
{
	
}