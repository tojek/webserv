/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkonarze <kkonarze@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 12:48:11 by kkonarze          #+#    #+#             */
/*   Updated: 2025/07/29 05:41:38 by kkonarze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include "ConfigParser.hpp"
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
__attribute__((noreturn)) void ConfigParser::parser_error(const std::string message, int line_num = -1)
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
	unit = s[len - 1];
	switch (unit)
	{
		case 'k': case 'K':
			multiplier = 1024;
			num_str = s.substr(0, len - 1);
			break ;
		case 'm': case 'M':
			multiplier = 1024 * 1024;
			num_str = s.substr(0, len - 1);
			break ;
		case 'g': case 'G':
			multiplier = 1024 * 1024 * 1024;
			num_str = s.substr(0, len - 1);
			break ;
		default:
			if (!isdigit(unit))
				parser_error("Invalid size unit '" + std::string(1, unit) + "'");
			num_str = s;
			break ;
	}
	try
	{
		num = string_to_int(num_str);
		if (num < 0)
			parser_error("Size cannot be negative: '" + s + "'");
		return num * multiplier;
	}
	catch (...)
	{
		parser_error("Invalid size format '" + s + "'");
	}
}

void ConfigParser::tokenize(const std::string& line)
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

ConfigParser::ConfigParser()
{

}

ConfigParser::ConfigParser(const std::string& filepath)
{
	std::ifstream	file(filepath.c_str());
	std::string		line;
	int				line_num = 0;

	block_num = 0;
	current_server = NULL;  // Initialize current_server pointer

	if (!file.is_open())
		parser_error("Could not open configuration file '" + filepath + "'");
	fill_tokens();

	while (std::getline(file, line))
	{
		line_num++;
		remove_comment(line);
		trim_whitespace(line);
		if (line.empty())
			continue;
		tokenize(line);

		if (token == "}" && remainder == "")
		{
			block_num--;
			if (block_num == 0)
				current_server = NULL;  // Reset when leaving server block
		}
		else if (tokens.count(token))
			(this->*tokens[token])(line_num);
		else if (block_num == 2 && current_server != NULL)
			current_server->locations.back().find_token(line_num, token, remainder);
		else
			parser_error("Unknown directive '" + token + "' inside server block.", line_num);
	}
	file.close();
	if (block_num > 0)
		parser_error(std::string("Unexpected EOF. Missing '}' for ") + ((block_num == 1) ? "server" : "location") + " block.");

	// Validate that we have at least one server
	if (servers.empty())
		parser_error("No server blocks found in configuration file.");
}

const std::vector<Config>& ConfigParser::get_servers() const
{
	return servers;
}

//Check if a server listens on the given host and port.
bool ConfigParser::server_matches_exact(const Config& server, const std::string& host, int port) const
{
	std::vector<ListenConfig>::const_iterator it;

	for (it = server.listen_configs.begin(); it != server.listen_configs.end(); ++it)
	{
		if (it->port == port && (it->host == host || it->host == "0.0.0.0"))
			return true;
	}
	return false;
}


// Check if a server listens on the given port (ignore host).
bool ConfigParser::server_matches_port(const Config& server, int port) const
{
	std::vector<ListenConfig>::const_iterator it;

	for (it = server.listen_configs.begin(); it != server.listen_configs.end(); ++it)
	{
		if (it->port == port)
			return true;
	}
	return false;
}

/**
 * Find the appropriate server configuration based on host and port.
 * Uses a three-tier selection strategy: exact match -> port match -> first server.
 */
const Config* ConfigParser::find_server(const std::string& host, int port) const
{
	std::vector<Config>::const_iterator it;

	if (servers.empty())
		return NULL;

	// Exact host:port match
	for (it = servers.begin(); it != servers.end(); ++it)
		if (server_matches_exact(*it, host, port))
			return &(*it);

	// Port-only match
	for (it = servers.begin(); it != servers.end(); ++it)
		if (server_matches_port(*it, port))
			return &(*it);

	// Default fallback
	return &servers[0];
}
