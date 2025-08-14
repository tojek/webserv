#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sys/stat.h>
#include <stdlib.h>     // realpath
#include "Request.hpp"
#include "Config.hpp"
#include "Location.hpp"

class Response
{
	public:
		Response();
		~Response();

		void				init_response(Request *request, Server *server);
		std::string			make_response();

		Request				*request;
		const Config		*server_block;
		const Location		*location_block;

	private:
		std::string	body;

		std::string	method;
		std::string host;
		std::string port;
		std::string request_uri;
		std::string http_version;
		std::string	root;
		std::string	index;
		std::string code;
		std::string text;
		std::string	resource_full_path;
		std::string	resource;
		std::string content_type;
		size_t		content_size;

		bool			is_cgi();
		void			cgi_handler();
		void			child_process();
		char			**envp;
		void			set_up_envp();
		int				pipe_in[2];
		int				pipe_out[2];

		void			static_file_handler();

		void			get_full_path();
		struct stat *info;
		std::ifstream 		file_content;
		std::ostringstream	response_content;

		const Location	*select_location(const std::vector<Location> &locations);
		void			init_host_and_port();
		void			init_resource();
		// std::string	make_response();
		void			parse_response();
		// void			read_location_block();

		void			return_method();

		std::string		get_method();
		std::string		post_method();
		std::string		delete_method();
};