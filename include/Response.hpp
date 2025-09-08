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
#include <dirent.h> // read dir listing
#include <ctime>

enum HttpStatus {
	HTTP_OK = 200,
	HTTP_CREATED = 201,
	HTTP_NO_CONTENT = 204,
	HTTP_MOVED_PERMANENTLY = 301,
	HTTP_FOUND = 302,
	HTTP_BAD_REQUEST = 400,
	HTTP_UNAUTHORIZED = 401,
	HTTP_FORBIDDEN = 403,
	HTTP_NOT_FOUND = 404,
	HTTP_METHOD_NOT_ALLOWED = 405,
	HTTP_CONTENT_TOO_LARGE = 413,
	HTTP_URI_TOO_LONG = 414,
	HTTP_INTERNAL_SERVER_ERROR = 500,
	HTTP_NOT_IMPLEMENTED = 501,
	HTTP_BAD_GATEWAY = 502,
	HTTP_SERVICE_UNAVAILABLE = 503
};

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
		std::string			body;
		size_t				body_size;
		bool				body_limit_exceeded;

		std::string			method;
		std::string 		host;
		std::string 		port;
		std::string			request_uri;
		std::string 		http_version;
		std::string			root;
		std::string			index;
		std::string 		code;
		std::string 		text;
		std::string			resource_full_path;
		std::string			resource;
		std::string 		content_type;
		std::string			redir_location;
		size_t				content_size;

		bool				is_cgi();
		void				cgi_handler();
		void				child_process();
		char				**envp;
		void				set_up_envp();
		int					pipe_in[2];
		int					pipe_out[2];

		bool				is_redirection;
		void				handle_redirection();
		void				static_file_handler();
		std::string			generate_directory_listing(const std::string& dir_path);
		void				delete_method();
		void				get_full_path();
		struct stat 		*info;
		std::ifstream 		file_content;
		std::ostringstream	response_content;
		int					is_method_allowed();
		const Location		*select_location(const std::vector<Location> &locations);
		bool				matchesRegex(const std::string& uri, const std::string& pattern);
		void				init_resource();
		void				set_status_line(std::string code, std::string text);
		void				set_status(HttpStatus status);
		std::string			get_error_page(int error_code);
		std::string			get_default_error_page();

		std::string							get_content_type();
		void								set_up_contenttypes();
		std::map<std::string, std::string>	cont_types;
		std::string 						get_file_extension(const std::string& path);
		std::string 						get_interpreter_path(const std::string& extension);
};