/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkonarze <kkonarze@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 13:13:03 by kkonarze          #+#    #+#             */
/*   Updated: 2025/07/16 17:47:22 by kkonarze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include "Server.hpp"

class Server;

class Client
{
private:
	int			client_fd;
	char		buffer[2048];
	std::string	response;
	int			blocking_flag;

public:
	static int		accept_client(Server &serv);
	static Client	*find_client(Server &serv, int event_fd);

	void			read_request();
	void			send_response(Server &serv);
	int				get_client_fd();
	int				get_blocking_flag();
	Client(Server &serv);
	~Client();
};

#endif