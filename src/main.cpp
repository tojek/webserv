/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkonarze <kkonarze@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 12:48:19 by kkonarze          #+#    #+#             */
/*   Updated: 2025/07/16 05:18:31 by kkonarze         ###   ########.fr       */
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

int main(int argc, char* argv[]) {
	(void)argv;
	(void)argc;

    setup_signals();

    Server *server = new Server(8080);

	server->init_epoll();

	server->event_loop();

	std::cout << "\nShutting down server...\n";
	delete server;
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
// int main(int argc, char* argv[]) {
	/*MATI*/

    // std::string config_filepath = "default.conf";

    // if (argc > 1) {
    //     config_filepath = argv[1];
    // }

    // std::cout << "Attempting to parse: " << config_filepath << std::endl;

    // WebservConfig parsed_config = parse_config_file(config_filepath);

    // std::cout << "\n--- Parsed Configuration ---" << std::endl;
    // std::cout << "Server Host: " << parsed_config.main_server.host << std::endl;
    // std::cout << "Server Port: " << parsed_config.main_server.port << std::endl;
    // std::cout << "Server Name: " << parsed_config.main_server.server_name << std::endl;
    // std::cout << "Client Max Body Size: " << parsed_config.main_server.client_max_body_size << " bytes" << std::endl;

    // std::cout << "Error Pages:" << std::endl;
    // for (std::map<int, std::string>::const_iterator it = parsed_config.main_server.error_pages.begin();
    //      it != parsed_config.main_server.error_pages.end(); ++it) {
    //     std::cout << "  " << it->first << ": " << it->second << std::endl;
    // }
    // std::cout << "--------------------------" << std::endl;
	// std::cout << "Parsing successful!" << std::endl;
    //  */
	/*KACPER*/