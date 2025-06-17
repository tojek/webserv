#include "webserv_config.hpp"
#include "Server.h"
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
#include <../include/signal.hpp>

std::string make_response() {
	
	std::ifstream index("./static/index.html");
    std::string html;
	std::stringstream buffer;
	buffer << index.rdbuf();

	html = buffer.str();

    std::ostringstream headers;
    headers << "HTTP/1.1 200 OK\r\n"
            << "Content-Type: text/html\r\n"
            << "Content-Length: " << html.size() << "\r\n"
            << "Connection: close\r\n\r\n"
            << html;

	index.close();
    return headers.str();
}

int make_socket_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}


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

int main(int argc, char* argv[]) {
	(void)argv;
	(void)argc;
	int client_fd;
    char buffer[2048];

    setup_signals();
    std::memset(buffer, 0, sizeof(buffer));

    Server *server = new Server(8080);

    struct epoll_event info, events[10];
    int num_of_fds, watch_set; 
    
    watch_set = epoll_create1(0);
    if (watch_set == -1)
    {
        perror("epoll create wyjebany\n");
        return (1);
    }

    info.events = EPOLLIN;
    info.data.fd = server->get_server();
    if (epoll_ctl(watch_set, EPOLL_CTL_ADD, server->get_server(), &info) == -1)
    {
        perror("epoll ctl wyjebany\n");
        return (1);
    }

    // while (true) {
    while (g_signal_state.sigint == 0 && g_signal_state.sigterm == 0) {
        num_of_fds = epoll_wait(watch_set, events, 10, -1);
        if (num_of_fds == -1)
        {
            if (errno == EINTR) continue; // interrupted by signal
            perror("epoll_wait");
            return 1;
        }
        for ( int x = 0; x < num_of_fds; x++)
        {
            if ( events[x].data.fd == server->get_server())
            {
                // client_fd = accept(server->get_server(), (struct sockaddr*)server->get_address(), server->get_addrlen());
                client_fd = accept(server->get_server(), NULL, NULL);
                if (client_fd < 0) 
				{
                	perror("accept");
                	continue;
        		}
				make_socket_non_blocking(client_fd);
				info.events = EPOLLIN | EPOLLET;
                info.data.fd = client_fd;
                epoll_ctl(watch_set, EPOLL_CTL_ADD, client_fd, &info);
			}
			else
			{
				std::memset(buffer, 0, sizeof(buffer));
        		read(client_fd, buffer, sizeof(buffer) - 1);
        		std::cout << "Zapytanie:\n" << buffer << std::endl;

        		std::string response = make_response();
        		send(client_fd, response.c_str(), response.size(), 0);
        		close(client_fd);
                epoll_ctl(watch_set, EPOLL_CTL_DEL, events[x].data.fd, NULL);
        	}
        }
    }
	std::cout << "Shutting down server...\n";
	delete server;
    return 0;
}
