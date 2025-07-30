/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkonarze <kkonarze@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 02:36:54 by kkonarze          #+#    #+#             */
/*   Updated: 2025/07/29 05:19:49 by kkonarze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef LOCATION__HPP
#define LOCATION__HPP

#include "Webserv.hpp"
#include <string>
#include <map>

class Location
{
private:
	std::string														location_path;
	std::map<std::string, std::string> 								directive;
	
	std::map<std::string, void (Location::*)(int, std::string&)>	tokens;

	void    read_upload_dir(int line_num, std::string& reminder);
	void    read_cgi_extension(int line_num, std::string& reminder);
	void    read_directory_listing(int line_num, std::string& reminder);
	void    read_allowed_methods(int line_num, std::string& reminder);
	void    validate_methods(int line_num, std::string& reminder);

	void	fill_tokens();
	__attribute__((noreturn)) void	parser_error(const std::string message, int line_num);
public:
	void	add_token(std::string token, std::string& value);
	void	find_token(int line_num, std::string& token, std::string& reminder);
	
	Location();
	Location(std::string& location_path);
	~Location();
};

#endif