#include "Response.hpp"
#include "Location.hpp"
#include <sys/wait.h>
#include <unistd.h>

void	Response::cgi_handler()
{
	set_status(HTTP_OK);
	content_type = "text/html";
	
	pipe(pipe_out);
	pipe(pipe_in);
	
	pid_t pid = fork();
	if (pid == 0)
		child_process();
	else
	{
		close(pipe_out[1]);
		close(pipe_in[0]);

		// Send body to CGI if present
		if (!body.empty())
			write(pipe_in[1], body.c_str(), body.size());
		close(pipe_in[1]);

		// Read all CGI output
		std::stringstream buf;
		char buffer[4096];
		ssize_t bytes;
		while ((bytes = read(pipe_out[0], buffer, sizeof(buffer))) > 0)
			buf.write(buffer, bytes);
		
		close(pipe_out[0]);
		waitpid(pid, NULL, 0);
		
		resource = buf.str();
	}
}

bool	Response::is_cgi()
{
	std::string	cgi_ext = location_block->get_cgi_extension();
	std::string	cgi_path = location_block->get_cgi_path();

	return (!cgi_path.empty() || 
			request_uri.find("/cgi-bin/") != std::string::npos ||
			(!cgi_ext.empty() && request_uri.find(cgi_ext) != std::string::npos));
}


void	Response::child_process()
{
	std::string executable_path;
	std::string cgi_path = location_block->get_cgi_path();

	if (!cgi_path.empty())
		executable_path = cgi_path;
	else
	{
		get_full_path();
		executable_path = resource_full_path;
	}

	// Redirect pipes
	dup2(pipe_in[0], STDIN_FILENO);
	dup2(pipe_out[1], STDOUT_FILENO);
	close(pipe_in[0]);
	close(pipe_in[1]);
	close(pipe_out[0]);
	close(pipe_out[1]);

	// Execute
	char *argv[] = { const_cast<char*>(executable_path.c_str()), NULL };
	set_up_envp();
	execve(executable_path.c_str(), argv, envp);
}

