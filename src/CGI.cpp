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

	// Check if CGI path is configured
	if (!cgi_path.empty())
		return true;
		
	// Check if request is to CGI directory
	if (request_uri.find("/cgi-bin/") != std::string::npos)
		return true;
		
	// Check if request matches CGI extension
	if (!cgi_ext.empty() && request_uri.find(cgi_ext) != std::string::npos)
		return true;
		
	return false;
}


void	Response::child_process()
{
	std::string executable_path;
	std::string script_path;
	std::string cgi_path = location_block->get_cgi_path();

	get_full_path();
	script_path = resource_full_path;

	// Determine the appropriate interpreter based on file extension
	std::string extension = "";
	size_t dot_pos = script_path.find_last_of(".");
	if (dot_pos != std::string::npos) {
		extension = script_path.substr(dot_pos);
	}

	// Choose interpreter based on file extension
	if (extension == ".php")
	{
		executable_path = "/usr/bin/php";
	}
	else if (extension == ".py")
	{
		executable_path = "/usr/bin/python3";
	}
	else if (!cgi_path.empty())
	{
		executable_path = cgi_path;
	}
	else
	{
		executable_path = script_path;
		script_path = "";
	}

	// Redirect pipes
	dup2(pipe_in[0], STDIN_FILENO);
	dup2(pipe_out[1], STDOUT_FILENO);
	close(pipe_in[0]);
	close(pipe_in[1]);
	close(pipe_out[0]);
	close(pipe_out[1]);

	set_up_envp();

	// Execute with appropriate arguments
	if (!script_path.empty())
	{
		// Execute interpreter with script as argument
		char *argv[] = { 
			const_cast<char*>(executable_path.c_str()), 
			const_cast<char*>(script_path.c_str()), 
			NULL 
		};
		execve(executable_path.c_str(), argv, envp);
	}
	else
	{
		// Execute script directly
		char *argv[] = { const_cast<char*>(executable_path.c_str()), NULL };
		execve(executable_path.c_str(), argv, envp);
	}
}
