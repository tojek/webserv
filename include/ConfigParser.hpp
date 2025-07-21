/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkonarze <kkonarze@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 12:48:01 by kkonarze          #+#    #+#             */
/*   Updated: 2025/07/16 16:44:12 by kkonarze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <fstream>

class ConfigParser
{
public:
	struct ListenConfig {
		std::string host;
		int port;
	};

private:
	std::vector<ListenConfig> listen_configs; // List of listen configurations
	// std::string	host; // IP address or hostname
	// int			port; // Port number

	std::string server_name;
	size_t		client_max_body_size; // Maximum client body size in bytes

	int			block_num;
	std::string	token;
	std::string	remainder;
	std::string location_path;

	std::map<std::string, void (ConfigParser::*)(int)>	tokens;
	std::map<int, std::string>							error_pages; // Map of error codes to error page paths
	std::map<std::string, std::map<std::string, std::string> > locations; // Map of location paths to directives

	void	read_listen(int line_num);
	void 	read_server_name(int line_num);
	void 	read_client_max_body_size(int line_num);
	void 	read_error_page(int line_num);
	void    read_location(int line_num);
	void    read_root(int line_num);
	void    read_index(int line_num);
	void    read_allowed_methods(int line_num);
	void    read_upload_dir(int line_num);
	void    read_cgi_extension(int line_num);
	void    read_directory_listing(int line_num);
	void	read_server(int line_num);
	void	map_location(int line_num);
	void 	fill_tokens();

	size_t	parse_size(const std::string& s);
	void    validate_methods(const std::string& methods, int line_num);
	void	tokenize(const std::string& line);
	__attribute__((noreturn)) void	parser_error(const std::string message, int line_num);
public:
	ConfigParser(const std::string& filepath);
	~ConfigParser();

	const std::vector<ListenConfig>& get_listen_configs() const { return listen_configs; }
	const	std::string& get_host() const;
	const	std::string& get_server_name() const;
	int		get_port() const;
	size_t	get_client_max_body_size() const;
	std::map<int, std::string>	get_error_pages() const;
	const std::map<std::string, std::map<std::string, std::string> >& get_locations() const;
};

