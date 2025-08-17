#include "Response.hpp"
#include "Location.hpp"
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>

bool	Response::is_cgi()
{
	std::string	cgi_ext;

	cgi_ext = location_block->get_cgi_extension();

	if (request_uri.find("/cgi-bin/") != std::string::npos)
		return (true);
	else if (cgi_ext != "" && request_uri.find(cgi_ext) != std::string::npos)
		return (true);
	return (false);
}

void	Response::cgi_handler()
{
	code = "200"; // TO BE REMOVED
	text = "OK"; // TO BE REMOVED
	// std::cout << "cgi here hello!\n";
	pid_t	pid;
	std::stringstream buf;

	pipe(pipe_out);
	pipe(pipe_in);
	pid = fork();
	if (pid == 0)
		child_process();
	else // server parent
	{
		std::cout << PINK"body size: " << RESET << body.size() << std::endl;
		close(pipe_out[1]);
		close(pipe_in[0]);

   	 	char buffer[4096];
    	ssize_t bytes;
		write(pipe_in[1], body.c_str(), body.size());
		close(pipe_in[1]);
    	while ((bytes = read(pipe_out[0], buffer, sizeof(buffer))) > 0)
			buf.write(buffer, bytes);
		resource = buf.str();
		content_type = "text/html"; // needs to be a function
    	close(pipe_out[0]);
   		waitpid(pid, NULL, 0);
	}
}

void	Response::child_process()
{
	// std::cout << "DEBUG: content_type=" << request->get_content_type() << std::endl;
	// std::cout << "DEBUG: content_size=" << request->get_content_size() << std::endl;
	get_full_path();

	close(pipe_in[1]);
	dup2(pipe_in[0], STDIN_FILENO);
	close(pipe_in[0]);
	// std::cout << resource_full_path << std::endl;
	close(pipe_out[0]);
	dup2(pipe_out[1], STDOUT_FILENO);
	close(pipe_out[1]);
	char *argv[] = { const_cast<char*>(resource_full_path.c_str()), NULL };
	// Prepare envp array for execve:
	set_up_envp();
	execve(resource_full_path.c_str(), argv, envp);
	if (envp != NULL) {
		for (size_t i = 0; envp[i] != NULL; ++i) {
			free(envp[i]);
		}
		delete[] envp;
		envp = NULL;
	}
}

// -----------old--------------
// void	Response::static_file_handler()
// {
// 	std::stringstream buffer;

// 	get_full_path();
// 	if (access(resource_full_path.c_str(), F_OK) == -1)
// 	{
// 		code = "500";
// 		text = "Internal Server Error";
// 		resource_full_path = "./static/500.html";
// 	}
// 	if (access(resource_full_path.c_str(), R_OK) == -1)
// 	{
// 		code = "403";
// 		text = "Forbidden";
//  	}
// 	else
// 	{
// 		code = "200";
// 		text = "OK";
// 	}
// 	file_content.open(resource_full_path.c_str());
// 	buffer << file_content.rdbuf();
// 	resource = buffer.str();
// 	content_type = "text/html"; // needs to be a function
// }
// ---------------------


void	Response::static_file_handler()
{
	get_full_path();

	// Check if path exists
	if (access(resource_full_path.c_str(), F_OK) == -1)
	{
		code = "404";
		text = "Not Found";
		resource = "<html><body><h1>404 Not Found</h1></body></html>";
		content_type = "text/html";
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
		if (location_block->get_directory_listing() == "on")
		{
			code = "200";
			text = "OK";
			resource = generate_directory_listing(resource_full_path);
			content_type = "text/html";
		}
		else
		{
			code = "403";
			text = "Forbidden";
			resource = "<html><body><h1>403 Forbidden</h1></body></html>";
			content_type = "text/html";
		}
		return;
	}

	// Regular file: check if readable and give
	if (access(resource_full_path.c_str(), R_OK) == -1)
	{
		code = "403";
		text = "Forbidden";
		resource = "<html><body><h1>403 Forbidden</h1></body></html>";
		content_type = "text/html";
	}
	else
	{
		code = "200";
		text = "OK";

		std::stringstream buffer;
		file_content.open(resource_full_path.c_str());
		buffer << file_content.rdbuf();
		resource = buffer.str();
		content_type = "text/html"; // needs to be a function
	}
}

// necessary to select proper server block context
// void	Response::init_host_and_port()
// {
// 	std::vector<std::string>	vec_host_port;
// 	std::string					host_port;

// 	host_port = request->get_host();
// 	if (host_port.find(":") == std::string::npos)
// 	{
// 		host = host_port;
// 		port = "80";
// 	}
// 	else
// 	{
// 		vec_host_port = ft_split(request->get_host(), ":");
// 		host = vec_host_port[0];
// 		port = vec_host_port[1];
// 	}
// }

void	Response::set_up_envp()
{
	std::vector<std::string> env_vars;
	char path[1000];
	realpath("./uploads", path);
	std::string upload_path = path;
	env_vars.push_back("REQUEST_METHOD=" + request->get_method());
	env_vars.push_back("SCRIPT_FILENAME=" + resource_full_path);
	env_vars.push_back("UPLOAD_DIR=" + upload_path);
	env_vars.push_back("QUERY_STRING=");
	env_vars.push_back("CONTENT_TYPE=" + request->get_content_type());
	env_vars.push_back("CONTENT_LENGTH=" + request->get_content_size());
	env_vars.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env_vars.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env_vars.push_back("SERVER_SOFTWARE=MyCppServer/1.0");

	// Allocate char* array
	envp = new char*[env_vars.size() + 1];
	for (size_t i = 0; i < env_vars.size(); i++) {
		envp[i] = strdup(env_vars[i].c_str()); // duplicate string to char*
	}
	envp[env_vars.size()] = NULL; // null-terminate array
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
				html << "<li><a href=\"" << name << "\">" << name << "</a></li>";
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
