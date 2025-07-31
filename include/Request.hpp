#pragma once

#define PINK "\033[1;38;5;205m"       // Bold Pink
#define LIGHT_PINK "\033[1;38;5;217m" // Bold Light Pink
#define LIGHT_BLUE "\033[1;38;5;153m" // Bold Light Blue
#define RESET "\033[0m"               // Reset to default color


#include "Server.hpp"
#include "Webserv.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <cstdio>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <cstring>
#include <vector>


class Request
{
	private:
		char								buffer[2048];
		std::map<std::string, std::string>	tokens;

		void	process_line(std::string& line, int line_num, bool body_flag);
		void	parse_requestline(std::string& line);
		void	parse_header(std::string& line);
		void	parse_body(std::string& line);

	public:
		Request();
		Request(int client_fd);
		~Request();

		void		parse_request();

		std::string	get_method();
		std::string	get_request_uri();
		std::string	get_host();
		std::string	get_http_version();
};