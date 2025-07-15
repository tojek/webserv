/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkonarze <kkonarze@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 12:48:01 by kkonarze          #+#    #+#             */
/*   Updated: 2025/06/25 12:48:02 by kkonarze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <fstream>

class ConfigParser
{
private:
	std::string	host; // IP address or hostname
	std::string server_name;
	size_t		client_max_body_size; // Maximum client body size in bytes
	int			port; // Port number

	std::map<std::string, void (ConfigParser::*)(const std::string&, int)>	tokens;
	std::map<int, std::string>							error_pages; // Map of error codes to error page paths
	std::map<std::string, std::map<std::string, std::string> > locations; // Map of location paths to directives

	void	read_listen(const std::string& remainder, int line_num);
	void 	read_server_name(const std::string& remainder, int line_num);
	void 	read_client_max_body_size(const std::string& remainder, int line_num);
	void 	read_error_page(const std::string& remainder, int line_num);
	void    read_location(const std::string& remainder, int line_num);
	void 	fill_tokens();
	size_t	parse_size(const std::string& s);
	void	tokenize(const std::string& line, std::string& token, std::string& reminder);
	__attribute__((noreturn)) void	parser_error(const std::string& message, int line_num);
public:
	ConfigParser(const std::string& filepath);
	~ConfigParser();

	const	std::string& get_host() const;
	const	std::string& get_server_name() const;
	int		get_port() const;
	size_t	get_client_max_body_size() const;
	std::map<int, std::string>	get_error_pages() const;
	const std::map<std::string, std::map<std::string, std::string> >& get_locations() const;
};

