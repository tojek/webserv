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


std::string Response::make_response()
{
	std::ostringstream headers;

	headers << http_version << " " << code << " " << "OK" << "\r\n"
			<< "Content-Type: " << content_type << "\r\n"
			<< "Content-Length: " << resource.size() << "\r\n"
			<< "Connection: close \r\n\r\n"
			<< resource;
	file_content.close();
	return (headers.str());
}

void Response::init_response(Request *request, Server *server)
{
	this->request = request;

	method = request->get_method();
	// request_uri is necessary to select_location()
	request_uri = request->get_request_uri();
	http_version = request->get_http_version();
	init_host_and_port();
	// @param config needs to be selected from the Server class but
	// for now we are using default config node (required Host and Port from request)
	server_block = &server->get_config();
	root = "www/"; // currently hardcoded
	// resource = std::ifstream(root + "index.html");
	init_resource();
	// required request_uri
	code = "200";
	text = "OK";
	location_block = select_location(server_block->locations);
	// read_location_block();
}

// select proper location block in the server block, requires request_uri
Location	*Response::select_location(std::vector<Location> locations)
{
	size_t i = 0;

	// while (i < locations.size())
	// {
	// 	if (request_uri == locations[i].get_location_path())
	// 		return (&locations[i]);
	// 	i++;
	// }
	std::cout << locations[i].get_location_path() << std::endl;
	return (&locations[i]);
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
	//check if resource is accessible -> if not, return proper code
	// resource_full_path = "~/home/zosia/" + root + "index.html";
	resource_full_path = "/home/zosia/www/index.html";
	file_content.open(resource_full_path.c_str());
	std::stringstream buffer;
	buffer << file_content.rdbuf();
	resource = buffer.str();
	// stat(resource_full_path.c_str(), info);
	// content_size = info->st_size;
	// content_type = get_file_type(resource_full_path);
	content_type = "text/html";
}
// void	Response::read_location_block()
// {

// }