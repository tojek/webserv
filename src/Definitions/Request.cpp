#include "../include/Request.hpp"

void Request::parse_request()
{
	std::istringstream	stream(buffer);
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
		std::cout << LIGHT_BLUE <<  line << RESET << std::endl;
		process_line(line, line_num, body_flag);
	}	
}

Request::Request()
{

}

Request::Request(int client_fd)
{
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
	tokens["method"] = request_line[0];
	std::cout << LIGHT_BLUE<< "method: "<<RESET << tokens["method"] << std::endl;
	tokens["request_uri"] = request_line[1];
	std::cout << LIGHT_BLUE<<"request uri: "<<RESET<<tokens["request_uri"] << std::endl;
	tokens["HTTP_version"] = request_line[2];
	std::cout << LIGHT_BLUE<<"HTTP version: "<<RESET<<tokens["HTTP_version"] << std::endl;
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