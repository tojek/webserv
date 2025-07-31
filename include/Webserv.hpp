/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkonarze <kkonarze@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 12:48:05 by kkonarze          #+#    #+#             */
/*   Updated: 2025/07/29 05:25:20 by kkonarze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <string>

#include <vector>

// Utils.cpp
void						error(const std::string& message);
void						trim_whitespace(std::string& s);
void						remove_comment(std::string& s);
int							string_to_int(const std::string& s);
int							make_socket_non_blocking(int fd);
std::vector<std::string>	ft_split(std::string str, std::string delimiter);
std::string					get_file_type(std::string file_path);

// hardcode
std::string make_response();

#endif