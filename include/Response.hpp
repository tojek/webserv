#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sys/stat.h>

#include "Request.hpp"
#include "Config.hpp"
#include "Location.hpp"

class Response
{
	public:
		Response();
		~Response();

		void			init_response(Request *request, Server *server);
		std::string		make_response();

		Request			*request;
		const Config	*server_block;
		Location		*location_block;

	private:
		std::string	method;
		std::string host;
		std::string port;
		std::string request_uri;
		std::string http_version;
		std::string	root;
		std::string code;
		std::string text;
		std::string	resource_full_path;
		std::string	resource;
		std::string content_type;
		size_t		content_size;

		struct stat *info;
		std::ifstream 		file_content;
		std::ostringstream	response_content;

		Location	*select_location(std::vector<Location> locations);
		void		init_host_and_port();
		void		init_resource();
		// std::string	make_response();
		void		parse_response();
		// void		read_location_block();

		void		get_method();
};