/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkonarze <kkonarze@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 01:45:54 by kkonarze          #+#    #+#             */
/*   Updated: 2025/07/29 05:33:27 by kkonarze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef SERVER_H
#define SERVER_H

#include <arpa/inet.h>
#include <sys/epoll.h>
#include <map>
#include "Client.hpp"
#include "Config.hpp"

class Client;

class Server
{
private:
	int					server_fd;

	const Config&		conf;
	struct sockaddr_in	address;
	socklen_t			addrlen;

	int					epoll_fd;
	int					num_of_fds;
	struct epoll_event	info;
	struct epoll_event	events[10];

	std::map<int, Client> clients;
	
public:
	~Server();
	Server(const Config& conf);
	Server(Server& serv);

	void		init_epoll();
	void		event_loop();

	int						get_server();
	int						get_epoll_fd();
	sockaddr_in				&get_address();
	socklen_t				&get_addrlen();
	epoll_event				&get_info();
	epoll_event				*get_events();		
	std::map<int, Client>	&get_clients();
};

#endif