#include "Response.hpp"
#include "Location.hpp"
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <iostream>

void	Response::cgi_handler()
{
	set_status(HTTP_OK);
	pid_t	pid;
	std::stringstream buf;

	std::cout << "=== CGI DEBUG ===" << std::endl;
	std::cout << "Request URI: " << request_uri << std::endl;
	std::cout << "Body size: " << body.size() << std::endl;
	std::cout << "Body content: '" << body << "'" << std::endl;
	std::cout << "================" << std::endl;

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
		std::cout << "CGI OUTPUT: '" << resource << "'" << std::endl;
    	close(pipe_out[0]);
   		waitpid(pid, NULL, 0);
		
		// Process CGI headers (Status, Content-Type, etc.)
		processCgiHeaders();
	}
}

bool	Response::is_cgi()
{
	std::string	cgi_ext;
	std::string	cgi_path;

	cgi_ext = location_block->get_cgi_extension();
	cgi_path = location_block->get_cgi_path();

	std::cout << "=== CGI DETECTION ===" << std::endl;
	std::cout << "Request URI: " << request_uri << std::endl;
	std::cout << "CGI extension: '" << cgi_ext << "'" << std::endl;
	std::cout << "CGI path: '" << cgi_path << "'" << std::endl;

	// check if there is cgi path directive
	if (!cgi_path.empty())
	{
		std::cout << "CGI detected via cgi_path" << std::endl;
		return (true);
	}

	if (request_uri.find("/cgi-bin/") != std::string::npos)
	{
		std::cout << "CGI detected via /cgi-bin/ in URI" << std::endl;
		return (true);
	}
	else if (cgi_ext != "" && request_uri.find(cgi_ext) != std::string::npos)
	{
		std::cout << "CGI detected via extension" << std::endl;
		return (true);
	}
	std::cout << "No CGI detected" << std::endl;
	return (false);
}


void	Response::child_process()
{
    std::string executable_path;
    std::string cgi_path = location_block->get_cgi_path();

    // Use cgi_path if specified, otherwise use the requested file
    if (!cgi_path.empty())
        executable_path = cgi_path;
    else
    {
        get_full_path();
        executable_path = resource_full_path;
    }

    // Debug output before execve
    std::cerr << "CHILD: Executing: " << executable_path << std::endl;
    std::cerr << "CHILD: Working directory: " << getcwd(NULL, 0) << std::endl;

	close(pipe_in[1]);
	dup2(pipe_in[0], STDIN_FILENO);
	close(pipe_in[0]);
	close(pipe_out[0]);
	dup2(pipe_out[1], STDOUT_FILENO);
	close(pipe_out[1]);

	char *argv[] = { const_cast<char*>(executable_path.c_str()), NULL };
	set_up_envp();
	execve(executable_path.c_str(), argv, envp);
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
			// Trim leading whitespace
			size_t start = cont_type.find_first_not_of(" \t");
			if (start != std::string::npos)
				cont_type = cont_type.substr(start);
        	break;
    	}
	}
	return(cont_type);
}

void Response::processCgiHeaders()
{
	std::istringstream header_stream(resource);
	std::string line;
	std::string headers_section;
	std::string body_section;
	bool headers_done = false;
	
	// Separate headers and body
	while (std::getline(header_stream, line))
	{
		if (!headers_done && (line.empty() || line == "\r"))
		{
			headers_done = true;
			continue;
		}
		
		if (!headers_done)
		{
			// Process Status header
			if (line.find("Status:") == 0)
			{
				std::string status_line = line.substr(strlen("Status:"));
				// Trim leading whitespace
				size_t start = status_line.find_first_not_of(" \t");
				if (start != std::string::npos)
				{
					status_line = status_line.substr(start);
					// Parse status code
					std::istringstream ss(status_line);
					int status_code;
					if (ss >> status_code)
					{
						// Map status codes to HttpStatus
						switch (status_code)
						{
							case 200: set_status(HTTP_OK); break;
							case 201: set_status(HTTP_CREATED); break;
							case 204: set_status(HTTP_NO_CONTENT); break;
							case 400: set_status(HTTP_BAD_REQUEST); break;
							case 403: set_status(HTTP_FORBIDDEN); break;
							case 404: set_status(HTTP_NOT_FOUND); break;
							case 405: set_status(HTTP_METHOD_NOT_ALLOWED); break;
							case 500: set_status(HTTP_INTERNAL_SERVER_ERROR); break;
							default: set_status(HTTP_INTERNAL_SERVER_ERROR); break;
						}
					}
				}
			}
		}
		else
		{
			// This is body content
			body_section += line + "\n";
		}
	}
	
	// Update resource to contain only the body (no headers)
	resource = body_section;
}