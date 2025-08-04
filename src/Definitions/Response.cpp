#include "Response.hpp"


// NIE USUWAĆ KOMENTARZY I STD COUT !!!! :(

// std::string Response::make_response()
// {	
// 	std::ifstream index("./static/index.html");
//     std::string html;
// 	std::stringstream buffer;
// 	std::ostringstream headers;
	
// 	buffer << index.rdbuf();
// 	html = buffer.str();
//     headers << "HTTP/1.1 200 OK\r\n"
//             << "Content-Type: text/html\r\n"
//             << "Content-Length: " << html.size() << "\r\n"
//             << "Connection: close\r\n\r\n"
//             << html;

// 	index.close();
//     return headers.str();
// }

Response::Response() {}

Response::~Response() {}

std::string	Response::make_response()
{
	int	i = 0;
	std::string	methods[] = {"GET", "POST", "DELETE"};
	std::string (Response::*functions[])() = {&Response::get_method, &Response::post_method, &Response::delete_method};

	while (i < 3)
	{
		if (method == methods[i])
		{
			return (this->*(functions[i]))();
		}
		i++;
	}
	return ("");
}

void Response::init_response(Request *request, Server *server)
{
	this->request = request;
	method = request->get_method();
	request_uri = request->get_request_uri();
	http_version = request->get_http_version();
	init_host_and_port();
	// @param server_block needs to be selected from the Server class but
	// for now we are using default config node (required Host and Port from request)
	server_block = &server->get_config();
	location_block = select_location(server_block->locations);
	init_resource();
}
// select proper location block in the server block, requires request_uri
Location	*Response::select_location(std::vector<Location> locations)
{
	size_t i = 0;
	Location *ret;

	while (i < locations.size())
	{
		if (locations[i].get_location_path() == "/")
			ret = &locations[i];
		if (request_uri == locations[i].get_location_path())
			return (&locations[i]);
		i++;
	}
	return (ret);
}

// necessary to select proper server block context
void	Response::init_host_and_port()
{
	std::vector<std::string>	vec_host_port;
	std::string					host_port;

	host_port = request->get_host();
	if (host_port.find(":") == std::string::npos)
	{
		host = host_port;
		port = "80";
	}
	else
	{
		vec_host_port = ft_split(request->get_host(), ":");
		host = vec_host_port[0];
		port = vec_host_port[1];
	}
}

void	Response::init_resource()
{
	char path[1000];
	
	realpath(location_block->get_root().c_str(), path);
	root = path;
	index = location_block->get_index();
	resource_full_path = root + "/" + index;
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
	std::stringstream buffer;
	buffer << file_content.rdbuf();
	resource = buffer.str();
	content_type = "text/html"; //HARDCODED
}
// void	Response::read_location_block()
// {

// }