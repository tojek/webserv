/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kkonarze <kkonarze@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 13:13:03 by kkonarze          #+#    #+#             */
/*   Updated: 2025/07/28 18:43:32 by kkonarze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include "Server.hpp"

class Server;
class Request;
class Response;

class Client
{
private:
	int			client_fd;
	int			blocking_flag;
	Response	*response;
public:
	Request			*request;
	static int		accept_client(Server &serv);
	static Client	*find_client(Server &serv, int event_fd);
	void			read_request(Server &serv);
	void			send_response(Server &serv);
	int				get_client_fd();
	int				get_blocking_flag();
	void			delete_request();

	Client(Server &serv);
	~Client();
	bool			connection_status;
};

#endif