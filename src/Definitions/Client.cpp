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
#include "Response.hpp"
#include "Webserv.hpp"
#include <iostream>
#include <string>
#include <unistd.h>
#include <cstdio>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <cstring>

//changed to return client fd after accepting
int Client::accept_client(Server &serv)
{
	Client client(serv);
	std::map<int, Client> &clients = serv.get_clients();

	if (client.get_client_fd() < 0)
		return (-1);
	if (client.get_blocking_flag())
		return (-1);

	int client_fd = client.get_client_fd();
	clients.insert(std::make_pair(client_fd, client));
	return (client_fd);  // Return the client fd instead of 0
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
	{
		std::cout << LIGHT_BLUE << "old request form\n" RESET;
		request->request_init(client_fd);
	}
	else
	{
		std::cout << LIGHT_BLUE << "new request form\n" RESET;
		request = new Request(client_fd);
	}
	if (request->get_connection() == "close")
		connection_status = false;
	else
		connection_status = true;
}

void Client::send_response(Server &serv)
{
	if (response != NULL)
		delete response;
	response = new Response();
	response->init_response(request, &serv);
	std::string resrc = response->make_response();
	send(client_fd, resrc.c_str(), resrc.size(), 0);
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
	response = NULL;

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
	// epoll_ctl(serv.get_epoll_fd(), EPOLL_CTL_ADD, client_fd, &info);
}

Client::~Client()
{
	if (request != NULL)
		delete request;
	if (response != NULL)
		delete response;
}

void Client::delete_request()
{
	std::cout << "deleting request\n";
	if (request != NULL)
		delete request;
	request = NULL;
	if (response != NULL)
		delete response;
	response = NULL;
}