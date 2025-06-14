/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkonarze <kkonarze@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 01:45:54 by kkonarze          #+#    #+#             */
/*   Updated: 2025/05/18 22:55:56 by kkonarze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef SERVER_H
#define SERVER_H

#include <arpa/inet.h>

class Server
{
private:
	int server_fd;
	struct sockaddr_in address;
	socklen_t addrlen;
public:
	Server();
	~Server();
	Server(int port);
	Server(Server& serv);

	int			get_server();
	sockaddr_in	*get_address();
	socklen_t	*get_addrlen();
};

#endif