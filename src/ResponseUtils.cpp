#include "Response.hpp"
#include "Location.hpp"
#include <sys/wait.h>
#include <sys/stat.h>
#include <cstdio>
#include <dirent.h>
#include <sstream>


void	Response::static_file_handler()
{
	get_full_path();

	// Check if path exists
	if (!is_method_allowed())
		return ;
	if (access(resource_full_path.c_str(), F_OK) == -1)
	{
		set_status(HTTP_NOT_FOUND);
		return;
	}
/*
struct stat {
    mode_t    st_mode;     // File type and permissions
    off_t     st_size;     // File size in bytes
    time_t    st_mtime;    // Last modification time
    time_t    st_atime;    // Last access time
    uid_t     st_uid;      // User ID of owner
    gid_t     st_gid;      // Group ID of owner
}
*/
	// Check if it's a directory
	struct stat path_stat;
	stat(resource_full_path.c_str(), &path_stat);

	if (S_ISDIR(path_stat.st_mode))
	{
		// Try to serve index file first
		std::string index_path = resource_full_path + "/" + location_block->get_index();

		//serve index
		if (access(index_path.c_str(), F_OK) == 0)
			resource_full_path = index_path;
		else if (location_block->get_directory_listing() == "on")
		{
			set_status(HTTP_OK);
			resource = generate_directory_listing(resource_full_path);
			content_type = "text/html";
			return;
		}
		else
		{
			set_status(HTTP_NOT_FOUND);
			content_type = "text/html";
			return;
		}
	}

	// Regular file: check if readable and give
	if (access(resource_full_path.c_str(), R_OK) == -1)
	{
		set_status(HTTP_FORBIDDEN);
		content_type = "text/html";
	}
	else
	{
		set_status(HTTP_OK);
		std::stringstream buffer;
		file_content.open(resource_full_path.c_str());
		buffer << file_content.rdbuf();
		resource = buffer.str();
	}
}

void	Response::set_up_envp()
{
	std::vector<std::string> env_vars;
	char path[1000];
	realpath("./static/uploads", path);
	std::string upload_path = path;
	env_vars.push_back("REQUEST_METHOD=" + request->get_method());
	env_vars.push_back("SCRIPT_FILENAME=" + resource_full_path);
	env_vars.push_back("UPLOAD_DIR=" + upload_path);
	
	// Extract query string from request_uri
	std::string query_string = "";
	size_t query_pos = request_uri.find('?');
	if (query_pos != std::string::npos) {
		query_string = request_uri.substr(query_pos + 1);
	}
	env_vars.push_back("QUERY_STRING=" + query_string);
	
	env_vars.push_back("CONTENT_TYPE=" + request->get_content_type());
	env_vars.push_back("CONTENT_LENGTH=" + request->get_content_size());
	env_vars.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env_vars.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env_vars.push_back("SERVER_SOFTWARE=MyCppServer/1.0");
	env_vars.push_back("REQUEST_URI=" + request_uri);
	env_vars.push_back("SERVER_NAME=" + server_block->server_name);
	
	std::string server_port = "80"; // default port
	if (!server_block->listen_configs.empty()) {
		std::stringstream ss;
		ss << server_block->listen_configs[0].port;
		server_port = ss.str();
	}
	env_vars.push_back("SERVER_PORT=" + server_port);
	
	// PATH_INFO: the part of the request URI that follows the script name
	// For CGI requests, typically the request_uri minus the location path
	std::string path_info = request_uri;
	std::string location_path = location_block->get_location_path();
	if (path_info.find(location_path) == 0) {
		path_info = path_info.substr(location_path.length());
		if (path_info.empty())
			path_info = "/";
	}
	env_vars.push_back("PATH_INFO=" + path_info);

	envp = new char*[env_vars.size() + 1];
	for (size_t i = 0; i < env_vars.size(); i++) {
		envp[i] = strdup(env_vars[i].c_str());
	}
	envp[env_vars.size()] = NULL;
}

/*
struct dirent {
    ino_t d_ino;           // Inode number of the file
    off_t d_off;           // Offset to the next dirent (not always present)
    unsigned short d_reclen; // Length of this record
    unsigned char d_type;   // File type (DT_REG, DT_DIR, etc.)
    char d_name[];         // Null-terminated filename (flexible array)
};
*/
std::string	Response::generate_directory_listing(const std::string& dir_path)
{
	std::ostringstream html;

	html << "<html><head><title>Directory Listing</title></head><body>";
	html << "<h1>Directory Listing</h1><ul>";

	DIR *dir = opendir(dir_path.c_str());
	if (dir != NULL)
	{
		struct dirent *entry;
		while ((entry = readdir(dir)) != NULL)
		{
			std::string name = entry->d_name;
			if (name != "." && name != "..")
			{
				std::string url = request_uri;
				if (!url.empty() && url[url.length() - 1] != '/')
					url += "/";
				url += name;

				html << "<li><a href=\"" << url << "\">" << name << "</a></li>";
			}
		}
		closedir(dir);
	}
	else
	{
		html << "<li>Error reading directory</li>";
	}

	html << "</ul></body></html>";
	return html.str();
}


void	Response::delete_method()
{
	if (!is_method_allowed())
		return ;
	if (std::remove(resource_full_path.c_str()) == -1)
	{
		set_status(HTTP_FORBIDDEN);
		return ;
	}
	else
	{
		resource = "";
		set_status(HTTP_NO_CONTENT);
		return ;
	}
}

void	Response::set_status_line(std::string code, std::string text)
{
	this->code = code;
	this->text = text;
}

void	Response::set_status(HttpStatus status)
{
	switch(status) {
		case HTTP_OK:
			set_status_line("200", "OK");
			break;
		case HTTP_CREATED:
			set_status_line("201", "Created");
			break;
		case HTTP_NO_CONTENT:
			set_status_line("204", "No Content");
			break;
		case HTTP_MOVED_PERMANENTLY:
			set_status_line("301", "Moved Permanently");
			break;
		case HTTP_FOUND:
			set_status_line("302", "Found");
			break;
		case HTTP_BAD_REQUEST:
			set_status_line("400", "Bad Request");
			resource = get_error_page(400);
			break;
		case HTTP_UNAUTHORIZED:
			set_status_line("401", "Unauthorized");
			resource = get_error_page(401);
			break;
		case HTTP_FORBIDDEN:
			set_status_line("403", "Forbidden");
			resource = get_error_page(403);
			break;
		case HTTP_NOT_FOUND:
			set_status_line("404", "Not Found");
			resource = get_error_page(404);
			break;
		case HTTP_METHOD_NOT_ALLOWED:
			set_status_line("405", "Method Not Allowed");
			resource = get_error_page(405);
			break;
		case HTTP_CONTENT_TOO_LARGE:
			set_status_line("413", "Content Too Large");
			resource = get_error_page(413);
			break;
		case HTTP_URI_TOO_LONG:
			set_status_line("414", "URI Too Long");
			resource = get_error_page(414);
			break;
		case HTTP_INTERNAL_SERVER_ERROR:
			set_status_line("500", "Internal Server Error");
			resource = get_error_page(500);
			break;
		case HTTP_NOT_IMPLEMENTED:
			set_status_line("501", "Not Implemented");
			resource = get_error_page(501);
			break;
		case HTTP_BAD_GATEWAY:
			set_status_line("502", "Bad Gateway");
			resource = get_error_page(502);
			break;
		case HTTP_SERVICE_UNAVAILABLE:
			set_status_line("503", "Service Unavailable");
			resource = get_error_page(503);
			break;
		default:
			set_status_line("500", "Internal Server Error");
			resource = get_error_page(500);
			break;
	}
}

int	Response::is_method_allowed()
{
	std::string methods = location_block->get_allowed_methods();

	if (methods.find(method) == std::string::npos)
	{
		set_status(HTTP_METHOD_NOT_ALLOWED);
		return (0);
	}
	return (1);
}

void	Response::handle_redirection()
{
	std::string	ret_directive;
	size_t		pos;

	ret_directive = location_block->get_return();
	is_redirection = true;
	set_status(location_block->get_return() == "301" ? HTTP_MOVED_PERMANENTLY : HTTP_FOUND);
	pos = ret_directive.find_last_of(" ");
	redir_location = ret_directive.substr(pos + 1);
	resource = "<html><body><h1>" + code + " " + text + "</h1></body></html>";
	content_type = "text/html";
}