/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkonarze <kkonarze@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 00:48:40 by kkonarze          #+#    #+#             */
/*   Updated: 2025/06/14 13:43:20 by kkonarze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.h"
#include <iostream>
#include <string>
#include <unistd.h>
#include <cstdio>
#include <arpa/inet.h>

Server::Server()
{

}

Server::~Server()
{
	close(server_fd);
}

Server::Server(int port)
{
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket failed");
		return ;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        return ;
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen");
        return ;
    }

	addrlen = sizeof(address);
    std::cout << "Serwer działa na http://localhost:" << port << std::endl;
}

int Server::get_server()
{
	return (server_fd);
};

sockaddr_in *Server::get_address()
{
	return (&address);
};

socklen_t *Server::get_addrlen()
{
	return (&addrlen);
};

Server::Server(Server& serv)
{
	if (this == &serv)
		return ;
	this->server_fd = serv.get_server();
	this->address = *serv.get_address();
	this->addrlen = *serv.get_addrlen();
}