/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkonarze <kkonarze@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 12:48:39 by kkonarze          #+#    #+#             */
/*   Updated: 2025/06/25 13:28:26 by kkonarze         ###   ########.fr       */
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
 * Prints out a error message and exits the program.
 * 
 * @param message message to print on cerr.
 */
void error(const std::string& message)
{
    std::cerr << "Error: " << message << std::endl;
    exit(EXIT_FAILURE);
}

/**
 * Trim whitespaces ( , \t, \\n, \r, \f, \v) from a line.
 * 
 * @param s string reference to a line.
 */
void trim_whitespace(std::string &s)
{
	size_t start = s.find_first_not_of(" \t\n\r\f\v");
	size_t end = s.find_last_not_of(" \t\n\r\f\v");
	if (std::string::npos == start)
		return s.clear();
	s = s.substr(start, (end - start + 1));
}

/**
 * Remove comment (#) from a line.
 * 
 * @param s string reference to a line.
 */
void remove_comment(std::string &s)
{
	size_t comment_pos = s.find('#');
	if (comment_pos != std::string::npos)
		s = s.substr(0, comment_pos);
}

/**
 * Converts string to byte amount while units. 
 * 
 * @param s string reference to byte amount.
 * @return Byte amount multiplied by units.
 */
int string_to_int(const std::string &s)
{
	std::istringstream	iss(s); // array of s strings
	int					num;

	iss >> num;					  // Tries to extract an int from array
	if (iss.fail() || !iss.eof()) // fail if fail or not int
		error("Invalid integer value '" + s + "'");
	return num;
}
