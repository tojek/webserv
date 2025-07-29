/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkonarze <kkonarze@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 12:48:39 by kkonarze          #+#    #+#             */
/*   Updated: 2025/07/29 05:25:19 by kkonarze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstring> 
#include <unistd.h>
#include <arpa/inet.h>
#include <cstdio>
#include <sys/epoll.h>
#include <fcntl.h>
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

/**
 * Makes the socket non blocking.
 * 
 * @param fd File descriptor that should be non blocking.
 * @return Returns -1 on error, 0 on correct change.
 */
int make_socket_non_blocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

/**
 * Makes response for the server.
 * 
 * @return Returns response as a string.
 */
std::string make_response()
{	
	std::ifstream index("./static/index.html");
    std::string html;
	std::stringstream buffer;
	std::ostringstream headers;
	
	buffer << index.rdbuf();
	html = buffer.str();
    headers << "HTTP/1.1 200 OK\r\n"
            << "Content-Type: text/html\r\n"
            << "Content-Length: " << html.size() << "\r\n"
            << "Connection: close\r\n\r\n"
            << html;

	index.close();
    return headers.str();
}

/**
 * Splits string by delimiter.
 * 
 * @param str String to be splited.
 * @param delimiter Delimiter by which string is splitted.
 * @return Returns splited spaces in vector of strings
 */
std::vector<std::string>	ft_split(std::string str, std::string delimiter)
{
	size_t						end = 0, start = 0;
	std::string					sub;
	std::vector<std::string>	result;

	while (end != std::string::npos)
	{
		end = str.find(delimiter, start);
		sub = str.substr(start, end - start);
		start = end + delimiter.size();
		result.push_back(sub);
	}
	return (result);
}
