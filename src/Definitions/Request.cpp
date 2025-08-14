#include "../include/Request.hpp"

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

Request::Request()
{

}

Request::Request(int client_fd)
{
	this->client_fd = client_fd;
	std::memset(buffer, 0, sizeof(buffer));
	read(client_fd, buffer, sizeof(buffer) - 1);
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
	std::cout << LIGHT_BLUE<< "method: "<<RESET << tokens["method"] << std::endl;
	tokens.insert(std::pair<std::string, std::string>("request_uri", request_line[1]));
	std::cout << LIGHT_BLUE<<"request_uri: "<<RESET<<tokens["request_uri"] << std::endl;
	tokens.insert(std::pair<std::string, std::string>("HTTP_version", request_line[2]));
	std::cout << LIGHT_BLUE<<"HTTP_version: "<<RESET<<tokens["HTTP_version"] << std::endl;
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
	std::cout << LIGHT_BLUE <<  header_line[0] << RESET ": " << tokens[header_line[0]] << std::endl;
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
		std::cout << pair[0] << ": "<< RESET << tokens[pair[0]] << std::endl;
		i++;
	}
}

void Request::extract_raw_body()
{
    std::string headers;
	headers = buffer;
     size_t pos = headers.find("\r\n\r\n");
     if (pos != std::string::npos) {
        std::string header_part = headers.substr(0, pos + 4);
        parse_request(header_part);
        body = headers.substr(pos + 4);
		std::cout << PINK "body: " RESET<<body << std::endl;
		std::cout << PINK "body size: " RESET << body.size() << std::endl;
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