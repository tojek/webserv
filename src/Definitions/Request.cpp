#include "Request.hpp"
#include "Debug.hpp"

void Request::parse_request(std::string headers)
{
	std::istringstream	stream(headers);
    std::string			line;
	size_t 				line_num = 0;
	bool				body_flag = false;

	while (std::getline(stream, line))
	{
		line_num++;
		if (line == "\r" || line.empty())
		{
			body_flag = true;
			continue ;
		}
		process_line(line, line_num, body_flag);
	}	
}

void	Request::request_init(int client_fd)
{

	ssize_t n = 1;
	std::memset(buffer, 0, sizeof(buffer));

	this->client_fd = client_fd;
    size_t pos = 0;
    std::map<std::string, std::string>::const_iterator it;
	request_complete = false;

    n = recv(client_fd, buffer, sizeof(buffer), 0);

	std::cout << LIGHT_BLUE << "bytes read: " << n << "\n" << RESET;
	if (n == 0)
	{
		std::cout << "0 bytes read, connection closed by client.\n";
		request_complete = true;
		return;
	}
	if (n < 0)
	{
		std::cout << "Error reading from client socket.\n";
		return;
	}
	// raw_request += buffer;
	if (n > 0)
		raw_request.append(buffer, n);
	std::cout << "start: "<<raw_request << "end"<< std::endl;
	pos = raw_request.find("\r\n\r\n");
	std::cout << "after finding pos = " << pos << std::endl;
	if (pos != std::string::npos)
	{
		parse_request(raw_request.substr(0, pos + 4));
		headers_parsed = true;
		std::cout << LIGHT_BLUE << "headers parsed\n" << RESET;
	}
	if (headers_parsed == true)
	{
   		body = raw_request.substr(pos + 4);
		it = tokens.find("Content-Length");
    	if (it != tokens.end())
    	{
        	std::istringstream(tokens["Content-Length"]) >> body_size;

        	while (body.size() < body_size)
        	{
         		n = recv(client_fd, buffer, sizeof(buffer), 0);
				if (n == 0)
					break;
           		body.append(buffer, n);
        	}
    	}
    	else if (((it = tokens.find("Transfer-Encoding")) != tokens.end()))
    	{
    	    if (tokens["Transfer-Encoding"] == "chunked")
            chunked_request_parser(raw_request, pos);
    	}
	}
	if (headers_parsed)
	{
		// Debug::display_trace(tokens);
		request_complete = true;
	}
	else
		std::cout << "not finished\n";
}

Request::Request() {}

Request::Request(int client_fd)
{
	headers_parsed = false;
	request_complete = false;
	request_init(client_fd);
}

Request::~Request() 
{

}

void Request::process_line(std::string& line, int line_num, bool body_flag)
{
	if (line_num == 1)
		parse_requestline(line);
	else if (!body_flag)
		parse_header(line);
	else
		parse_body(line);
}

void Request::parse_requestline(std::string& line)
{
	std::vector<std::string> request_line;

	request_line = ft_split(line, " ");
	tokens.insert(std::pair<std::string, std::string>("method", request_line[0]));
	tokens.insert(std::pair<std::string, std::string>("request_uri", request_line[1]));
	tokens.insert(std::pair<std::string, std::string>("HTTP_version", request_line[2]));
	
	// Debug::display_trace(tokens);
}

void Request::parse_header(std::string& line)
{
	std::vector<std::string> header_line;

	if (line.find(": ") == std::string::npos)
		return ;
	header_line = ft_split(line, ": ");
	tokens.insert(std::pair<std::string, std::string>(header_line[0], header_line[1]));
	if (header_line[0].empty())
		return ;
	Debug::display1(header_line[0], header_line[1]);
}

void Request::parse_body(std::string& line)
{
	std::vector<std::string> body_pairs;
	std::vector<std::string> pair;
	size_t i = 0;

	body_pairs = ft_split(line, "&");

	while (i < body_pairs.size())
	{
		pair = ft_split(body_pairs[i], "=");
		tokens[pair[0]] = pair[1];
		i++;
	}
}

void Request::chunked_request_parser(std::string raw_request, size_t pos)
{
	size_t chunk_start = pos + 4;
	while (true)
	{
		// Find the next CRLF to get the chunk size line
		size_t crlf_pos = raw_request.find("\r\n", chunk_start);
		if (crlf_pos == std::string::npos)
			break; // Incomplete chunk size line, need more data
		std::string chunk_size_str = raw_request.substr(chunk_start, crlf_pos - chunk_start);
		if (chunk_size_str.empty())
			break; // End of chunks
		std::istringstream iss(chunk_size_str);
		size_t chunk_size = 0;
		iss >> std::hex >> chunk_size;
		if (chunk_size == 0)
			break; // Last chunk
		// The chunk data starts after the CRLF
		size_t chunk_data_start = crlf_pos + 2;
		if (raw_request.size() < chunk_data_start + chunk_size + 2)
			break; // Incomplete chunk data, need more data
		body.append(raw_request.substr(chunk_data_start, chunk_size));
		chunk_start = chunk_data_start + chunk_size + 2; // Move past chunk data and trailing CRLF
		}
}

std::string Request::get_method()
{
	return (tokens["method"]);
}

std::string Request::get_request_uri()
{
	return (tokens["request_uri"]);
}

std::string Request::get_host()
{
	return (tokens["Host"]);
}

std::string Request::get_http_version()
{
	return (tokens["HTTP_version"]);
}

std::string Request::get_content_size()
{
	return (tokens["Content-Length"]);
}

std::string Request::get_content_type()
{
	return (tokens["Content-Type"]);
}

std::string	Request::get_body()
{
	return (body);
}

std::string Request::get_connection()
{
	std::map<std::string, std::string>::const_iterator it = tokens.find("Connection");
	if (it == tokens.end() || it->second.empty())
		return "keep-alive";
	return it->second;
}

size_t	Request::get_body_size()
{
	return (body_size);
}

bool	Request::is_request_complete()
{
	return (request_complete);
}