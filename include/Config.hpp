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

struct Config {
	std::string host;
	int port;
	size_t client_max_body_size;
	std::string server_name;
	std::map<int, std::string> error_pages;
	std::vector<Location> locations;
};

#endif