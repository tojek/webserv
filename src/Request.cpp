#include "../include/Request.hpp"

void Request::read_request(int client_fd)
{
	size_t end = 0;
	size_t beginning;
	size_t line_num = 0;
	std::string line;
	std::string str;

	body_flag = false;

	std::memset(buffer, 0, sizeof(buffer));
	read(client_fd, buffer, sizeof(buffer) - 1);
	str = buffer;
	// std::cout << "Zapytanie:\n" << buffer << std::endl;

	while (end != std::string::npos)
	{
		beginning = end;
		end = str.find("\n", beginning);
		if (end == std::string::npos)
			break ;
		line = str.substr(beginning, end - beginning);

		// Detect empty line (headers/body separator)
		if (line == "\r" || line.empty())
		{
			body_flag = true;
			// The body starts after this line
			size_t body_start = end + 1;
			if (body_start < str.size())
			{
				std::string body = str.substr(body_start);
				parse_body(body);
				std::cout << "body: " << body << std::endl;
			}
			break ;
		}

		std::cout << line << std::endl;
		process_line(line, line_num);
		line_num++;
		end++;
	}
}

Request::Request() {}

Request::~Request() 
{
}

void Request::process_line(std::string line, int line_num)
{
	// if (line.size() == 1)
	// {
	// 	// std::cout << "here\n";
	// 	return ;
	// }
	// std::cout << "our line: " << line << std::endl;
	if (line_num == 0)
		parse_requestline(line);
	else if (body_flag == true)
		parse_body(line);
	else
		parse_header(line);
}

void Request::parse_requestline(std::string line)
{
	request_line = ft_split(line, " ");

	tokens["method"] = request_line[0];
	std::cout << "method: " << tokens["method"] << std::endl;
	tokens["request_uri"] = request_line[1];
	std::cout << "request uri: "<<tokens["request_uri"] << std::endl;
	tokens["HTTP_version"] = request_line[2];
	std::cout << "HTTP version: "<<tokens["HTTP_version"] << std::endl;
}

void Request::parse_header(std::string line)
{
	std::vector<std::string> header_line;

	if (line.find(": ") == std::string::npos)
		return ;
	header_line = ft_split(line, ": ");
	std::cout.flush();
	tokens.insert(std::pair<std::string, std::string>(header_line[0], header_line[1]));
	if (header_line[0].empty())
		return ;
	// std::cout << header_line[0] << ": " << tokens[header_line[0]] << std::endl;
}

void Request::parse_body(std::string line)
{
	std::vector<std::string> body_pairs;
	std::vector<std::string> pair;
	size_t i = 0;

	body_pairs = ft_split(line, "&");

	while (i < body_pairs.size())
	{
		pair = ft_split(body_pairs[i], "=");
		tokens[pair[0]] = pair[1];
		std::cout << pair[0] << ": " << tokens[pair[0]] << std::endl;
		i++;
	}


}