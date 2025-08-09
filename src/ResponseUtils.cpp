#include "Response.hpp"
#include "Location.hpp"
#include <sys/wait.h>
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

	pipe(fd);
	pid = fork();
	if (pid == 0)
		child_process();
	else // server parent 
	{
		close(fd[1]);
   	 	char buffer[4096];
    	ssize_t bytes;
    	while ((bytes = read(fd[0], buffer, sizeof(buffer))) > 0) 
			buf.write(buffer, bytes);
		resource = buf.str();
		content_type = "text/plain"; // needs to be a function
    	close(fd[0]);
   		waitpid(pid, NULL, 0);
	}
}

void	Response::child_process()
{
	size_t	pos;
	char path[1000];

	pos = request_uri.find_last_of('/');
	std::string execfile = request_uri.substr(pos + 1);
	// std::cout << execfile << std::endl;
	realpath(location_block->get_root().c_str(), path);
	root = path;
	// index = location_block->get_index();
	resource_full_path = root + "/" + execfile;

	std::cout << resource_full_path << std::endl;
	close(fd[0]);
	dup2(fd[1], STDOUT_FILENO);
	close(fd[1]);
	char *argv[] = { const_cast<char*>(resource_full_path.c_str()), NULL };
	// Prepare envp array for execve:
	char *envp[] = { NULL };
	execve(resource_full_path.c_str(), argv, envp);

}

void	Response::static_file_handler()
{
	std::stringstream buffer;

	get_full_path();
	if (access(resource_full_path.c_str(), F_OK) == -1)
	{
		code = "500";
		text = "Internal Server Error";
		resource_full_path = "./static/500.html";
	}
	if (access(resource_full_path.c_str(), R_OK) == -1)
	{
		code = "403";
		text = "Forbidden";
 	}
	else
	{
		code = "200";
		text = "OK";
	}
	file_content.open(resource_full_path.c_str());
	buffer << file_content.rdbuf();
	resource = buffer.str();
	content_type = "text/html"; // needs to be a function
}