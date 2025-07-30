/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkonarze <kkonarze@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 13:16:44 by kkonarze          #+#    #+#             */
/*   Updated: 2025/07/29 01:06:16 by kkonarze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Server.hpp"
#include "Request.hpp"
#include "Webserv.hpp"
#include <iostream>
#include <string>
#include <unistd.h>
#include <cstdio>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <cstring>

int Client::accept_client(Server &serv)
{
	Client client(serv);
	std::map<int, Client> &clients = serv.get_clients();
	
	if (client.get_client_fd() < 0)
		return (-1);
	if (client.get_blocking_flag())
		return (-1);
	clients.insert(std::make_pair(client.get_client_fd(), client));
	return (0);
}

Client *Client::find_client(Server &serv, int event_fd)
{
	std::map<int, Client> &clients = serv.get_clients();
	std::map<int, Client>::iterator it = clients.find(event_fd);

	if (it == clients.end())
		return (NULL);
	return &(it->second);
}

void Client::read_request()
{
	if (request != NULL)
		delete request;
	request = new Request(client_fd);
	request->parse_request();
}

void Client::send_response(Server &serv)
{
	std::string response = make_response();

	send(client_fd, response.c_str(), response.size(), 0);
	close(client_fd);
	epoll_ctl(serv.get_epoll_fd(), EPOLL_CTL_DEL, client_fd, NULL);
}

int	Client::get_client_fd()
{
	return (client_fd);
}

int	Client::get_blocking_flag()
{
	return (blocking_flag);
}

Client::Client(Server &serv)
{
	epoll_event	info;

	request = NULL;
	client_fd = accept(serv.get_server(), NULL, NULL);
	if (client_fd < 0) 
	{
		perror("accept");
		return ;
	}
	if (make_socket_non_blocking(client_fd) < 0)
	{
		perror("non blocking");
		blocking_flag = 1;
		return ;
	}
	blocking_flag = 0;
	info = serv.get_info();
	info.events = EPOLLIN | EPOLLET;
	info.data.fd = client_fd;
	epoll_ctl(serv.get_epoll_fd(), EPOLL_CTL_ADD, client_fd, &info);
}

Client::~Client()
{
	if (request != NULL)
		delete request;
}
