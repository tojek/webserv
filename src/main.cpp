/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkonarze <kkonarze@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 12:48:19 by kkonarze          #+#    #+#             */
/*   Updated: 2025/07/29 05:33:39 by kkonarze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include "Server.hpp"
#include "ServerManager.hpp"
#include "Signal.hpp"
#include "ConfigParser.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/epoll.h>

int main(int argc, char *argv[])
{
    std::string config_filepath = "default.conf";

    if (argc > 1) {
        config_filepath = argv[1];
    }

    try {
        ConfigParser config(config_filepath);
        const std::vector<Config>& servers = config.get_servers();

        if (servers.empty()) {
            std::cerr << "Error: No servers configured" << std::endl;
            return 1;
        }

        setup_signals();

        ServerManager manager(config);
        manager.init_servers();
        manager.run_single_server();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
