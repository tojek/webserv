/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkonarze <kkonarze@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 12:48:19 by kkonarze          #+#    #+#             */
/*   Updated: 2025/07/16 16:02:34 by kkonarze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include "Server.hpp"
#include "Signal.hpp"
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
        // Parse the configuration file
        std::cout << "Parsing configuration file: " << config_filepath << std::endl;
        ConfigParser config(config_filepath);

        // Display the parsed configuration
        std::cout << "\n--- Server Configuration ---" << std::endl;
        std::cout << "Server Name: " << config.get_server_name() << std::endl;
        std::cout << "Client Max Body Size: " << config.get_client_max_body_size() << " bytes" << std::endl;

        // Display all listen configurations
        std::cout << "\n--- Listen Configurations ---" << std::endl;
        const std::vector<ConfigParser::ListenConfig>& listen_configs = config.get_listen_configs();
        for (size_t i = 0; i < listen_configs.size(); i++) {
            std::cout << "Listen " << i+1 << ": " << listen_configs[i].host << ":" << listen_configs[i].port << std::endl;
        }

        std::cout << "\n--- Error Pages ---" << std::endl;
        const std::map<int, std::string>& error_pages = config.get_error_pages();
        for (std::map<int, std::string>::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it) {
            std::cout << "  " << it->first << ": " << it->second << std::endl;
        }

        std::cout << "\n--- Location Blocks ---" << std::endl;
        const std::map<std::string, std::map<std::string, std::string> >& locations = config.get_locations();
        for (std::map<std::string, std::map<std::string, std::string> >::const_iterator it = locations.begin();
             it != locations.end(); ++it) {
            std::cout << "Location: " << it->first << std::endl;

            const std::map<std::string, std::string>& directives = it->second;
            for (std::map<std::string, std::string>::const_iterator dir_it = directives.begin();
                dir_it != directives.end(); ++dir_it) {
                std::cout << "  " << dir_it->first << ": " << dir_it->second << std::endl;
            }
            std::cout << std::endl;
        }

        setup_signals();

        // For now, just use the first listen configuration
        // In a more complete implementation, you would create multiple servers
        if (!listen_configs.empty()) {
            Server *server = new Server(listen_configs[0].port);
            server->init_epoll();
            server->event_loop();
            std::cout << "\nShutting down server...\n";
            delete server;
        } else {
            std::cerr << "No listen configurations found!" << std::endl;
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

	//while (g_signal_state.sigint == 0 && g_signal_state.sigterm == 0) {
    //    num_of_fds = epoll_wait(watch_set, events, 10, -1);
    //    if (num_of_fds == -1)
    //    {
    //        if (errno == EINTR) continue; // interrupted by signal
    //        perror("epoll_wait");
    //        return 1;
    //    }
    //    for ( int x = 0; x < num_of_fds; x++)
    //    {
    //        if ( events[x].data.fd == server->get_server())
    //        {
    //            // client_fd = accept(server->get_server(), (struct sockaddr*)server->get_address(), server->get_addrlen());
    //            client_fd = accept(server->get_server(), NULL, NULL);
    //            if (client_fd < 0)
	//			{
    //            	perror("accept");
    //            	continue;
    //    		}
	//			make_socket_non_blocking(client_fd);
	//			info.events = EPOLLIN | EPOLLET;
    //            info.data.fd = client_fd;
    //            epoll_ctl(watch_set, EPOLL_CTL_ADD, client_fd, &info);
	//		}
	//		else
	//		{
	//			std::memset(buffer, 0, sizeof(buffer));
    //    		read(client_fd, buffer, sizeof(buffer) - 1);
    //    		std::cout << "Zapytanie:\n" << buffer << std::endl;

    //    		std::string response = make_response();
    //    		send(client_fd, response.c_str(), response.size(), 0);
    //    		close(client_fd);
    //            epoll_ctl(watch_set, EPOLL_CTL_DEL, events[x].data.fd, NULL);
    //    	}
    //    }
    //}
