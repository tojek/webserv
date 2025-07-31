/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkonarze <kkonarze@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 05:11:45 by kkonarze          #+#    #+#             */
/*   Updated: 2025/07/29 05:21:14 by kkonarze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Location.hpp"
#include <string>
#include <map>
#include <vector>

struct ListenConfig {
	std::string host;
	int port;

	ListenConfig(const std::string& h = "0.0.0.0", int p = 80) : host(h), port(p) {}
};

struct Config {
	std::vector<ListenConfig> listen_configs;  // Changed to support multiple listen directives
	size_t client_max_body_size;
	std::string server_name;
	std::map<int, std::string> error_pages;
	std::vector<Location> locations;

	// Constructor with default values
	Config() : client_max_body_size(1024 * 1024), server_name("") {}
};

#endif