#pragma once

#define PINK "\033[1;38;5;205m"       // Bold Pink
#define LIGHT_PINK "\033[1;38;5;217m" // Bold Light Pink
#define LIGHT_BLUE "\033[1;38;5;153m" // Bold Light Blue
#define RESET "\033[0m"               // Reset to default color


#include "Server.hpp"
#include "Webserv.hpp"
#include <iostream>
#include <string>
#include <unistd.h>
#include <cstdio>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <cstring>
#include <vector>


class Request
{
	public:
		Request();
		// Request(int client_fd);
		~Request();
		void read_request(int client_fd);
	private:
		char buffer[2048];
		std::map<std::string, std::string> tokens;
		
		std::vector<std::string>	request_line;
		std::vector<std::string>	headers;
		std::vector<std::string>	body;
		bool 						body_flag;


		void process_line(std::string line, int line_num);
		void parse_requestline(std::string line);
		void parse_header(std::string line);
		void parse_body(std::string line);
};