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
	headers_parsed = true;
}

void	Request::request_init(int client_fd)
{

	ssize_t n = 1;
	std::memset(buffer, 0, sizeof(buffer));

	this->client_fd = client_fd;
    size_t pos = 0;
    std::map<std::string, std::string>::const_iterator it;
	request_complete = false;
	connection_closed = false;
	body_parsed = false;

    n = recv(client_fd, buffer, sizeof(buffer), 0);

	// std::cout << LIGHT_BLUE << "bytes read: " << n << "\n" << RESET;
	if (n == 0)
	{
		connection_closed = true;
		std::cout << "connection closed by client.\n";
		request_complete = true;
		return;
	}
	if (n < 0)
	{
		std::cout << "Error reading from client socket.\n";
		return;
	}
	if (n > 0)
		raw_request.append(buffer, n);
	// std::cout << "start: "<<raw_request << "end"<< std::endl;
	pos = raw_request.find("\r\n\r\n");
	if (pos != std::string::npos && !headers_parsed)
		parse_request(raw_request.substr(0, pos + 4));
	if (headers_parsed == true)
	{
   		body = raw_request.substr(pos + 4);
		it = tokens.find("Content-Length");
    	if (it != tokens.end())
    	{
			// std::cout << "Content-Length token found!\n";
        	std::istringstream(tokens["Content-Length"]) >> body_size;

        	while (body.size() < body_size)
        	{
         		n = recv(client_fd, buffer, sizeof(buffer), 0);
				if (n == 0)
					break;
           		body.append(buffer, n);
        	}
			body_parsed = true;
    	}
    	else if (((it = tokens.find("Transfer-Encoding")) != tokens.end()))
    	{
			zero_byte_found = false;
			// std::cout << "Transfer-Encoding token found!\n";
    		if (tokens["Transfer-Encoding"] == "chunked")
            {
				if (chunked_request_parser(raw_request, pos))
					body_parsed = true;
			}
    	}
		else
			body_parsed = true;
	}
	if (headers_parsed && body_parsed)
	{
		std::cout << LIGHT_BLUE "ready to respond!!\n" RESET;
		// Debug::display_trace(tokens);
		request_complete = true;
	}
	// else
		// std::cout << "not finished\n";
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
	
	Debug::display_trace(tokens);
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

bool Request::chunked_request_parser(const std::string raw_request, size_t pos)
{
    size_t chunk_start = pos + 4; // skip past header terminator
    while (true)
    {
        // Need a CRLF to end the chunk size line
        size_t crlf_pos = raw_request.find("\r\n", chunk_start);
        if (crlf_pos == std::string::npos)
            return false; // incomplete, wait for more data

        // Extract the size line (may contain ";ext")
        std::string chunk_size_str = raw_request.substr(chunk_start, crlf_pos - chunk_start);
        if (chunk_size_str.empty())
            return false; // invalid (empty size line)

        // Strip optional ";extension"
        size_t semi = chunk_size_str.find(';');
        if (semi != std::string::npos)
            chunk_size_str = chunk_size_str.substr(0, semi);

        // Parse hex size
        size_t chunk_size = 0;
        std::istringstream iss(chunk_size_str);
        iss >> std::hex >> chunk_size;
        if (iss.fail())
            return false; // invalid hex
        size_t chunk_data_start = crlf_pos;
		// std::cout << LIGHT_PINK "body = " << body << "size: " << body.size() << RESET << std::endl;
        if (chunk_size == 0)
        {
			// std::cout << LIGHT_BLUE "found the zero byte!!\n" RESET;
            // Last-chunk: must have \r\n after trailers (or immediately)
            size_t trailer_end = raw_request.find("\r\n\r\n", chunk_data_start);
            if (trailer_end == std::string::npos)
                return false; // trailers not complete yet
            // You could parse trailers here if you want:
            // std::string trailers = raw_request.substr(chunk_data_start, trailer_end - chunk_data_start);
			std::cout << LIGHT_BLUE "chunked body completed!!\n" RESET;
            return true; // body complete
        }

        // Check if we have full chunk data + trailing CRLF
        if (raw_request.size() < chunk_data_start + chunk_size + 2)
            return false; // incomplete, need more data

        // Append chunk to body
        body.append(raw_request.substr(chunk_data_start, chunk_size));

        // Advance past chunk data + CRLF
        chunk_start = chunk_data_start + chunk_size + 2;
    }
    return false;
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