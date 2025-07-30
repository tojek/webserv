/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkonarze <kkonarze@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 12:48:01 by kkonarze          #+#    #+#             */
/*   Updated: 2025/07/29 05:23:46 by kkonarze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "Config.hpp"
#include <string>
#include <vector>
#include <map>
#include <fstream>

class ConfigParser
{
private:
	Config		conf;

	int			block_num;
	std::string	token;
	std::string	remainder;

	std::map<std::string, void (ConfigParser::*)(int)>	tokens;

	void	read_listen(int line_num);
	void 	read_server_name(int line_num);
	void 	read_client_max_body_size(int line_num);
	void 	read_error_page(int line_num);
	void    read_location(int line_num);
	void    read_root(int line_num);
	void    read_index(int line_num);
	void	read_server(int line_num);
	void 	fill_tokens();

	size_t		parse_size(const std::string& s);
	void		tokenize(const std::string& line);
	__attribute__((noreturn)) void	parser_error(const std::string message, int line_num);
public:
	ConfigParser(const std::string& filepath);
	ConfigParser();
	~ConfigParser();

	const	Config&		get_config() const;
};


#endif