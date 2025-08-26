#include "Response.hpp"
#include "Location.hpp"
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>

void	Response::cgi_handler()
{
	set_status(HTTP_OK);
	pid_t	pid;
	std::stringstream buf;

	pipe(pipe_out);
	pipe(pipe_in);
	pid = fork();
	if (pid == 0)
		child_process();
	else
	{
		close(pipe_out[1]);
		close(pipe_in[0]);

   	 	char buffer[4096];
    	ssize_t bytes;
		write(pipe_in[1], body.c_str(), body.size());
		close(pipe_in[1]);
    	while ((bytes = read(pipe_out[0], buffer, sizeof(buffer))) > 0)
			buf.write(buffer, bytes);
		resource = buf.str();
    	close(pipe_out[0]);
   		waitpid(pid, NULL, 0);
	}
}

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


void	Response::child_process()
{
	get_full_path();

	close(pipe_in[1]);
	dup2(pipe_in[0], STDIN_FILENO);
	close(pipe_in[0]);
	close(pipe_out[0]);
	dup2(pipe_out[1], STDOUT_FILENO);
	close(pipe_out[1]);
	char *argv[] = { const_cast<char*>(resource_full_path.c_str()), NULL };
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

std::string	Response::cgi_content_type()
{
	std::string cont_type = "text/html";

	std::istringstream header_stream(resource);
	std::string line;
	while (std::getline(header_stream, line))
	{
   		if (line.find("Content-Type:") == 0)
		{
       		cont_type = line.substr(strlen("Content-Type:"));
        	break;
    	}
	}
	return(cont_type);
}