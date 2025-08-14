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
	body = request->get_body();
	method = request->get_method();
	request_uri = request->get_request_uri();
	http_version = request->get_http_version();
	server_block = &server->get_config();
	location_block = select_location(server_block->locations);
	init_resource();
}

// select proper location block in the server block, requires request_uri
const Location	*Response::select_location(const std::vector<Location> &locations)
{
	size_t i = 0;
	const Location *ret;
	std::string		uri_path;
	size_t			pos = 0;

	pos = request_uri.find("/", 1);
	if (pos == std::string::npos)
		uri_path = request_uri;
	else
		uri_path = request_uri.substr(0, pos);
	while (i < locations.size())
	{
		if (locations[i].get_location_path() == "/")
			ret = &locations[i];
		else if (uri_path == locations[i].get_location_path())
			return (&locations[i]);
		i++;
	}
	return (ret);
}

void	Response::get_full_path()
{
	size_t	pos, dot;
	char path[1000];

	pos = request_uri.find_last_of('/');
	std::string execfile = request_uri.substr(pos + 1);
	dot = request_uri.find('.');
	if (dot == std::string::npos)
		execfile = location_block->get_index();
	realpath(location_block->get_root().c_str(), path);
	root = path;
	resource_full_path = root + "/" + execfile;
}

void	Response::init_resource()
{
	if (is_cgi()) // should be passed to cgi, not static file handler
		cgi_handler();
	else
		static_file_handler();
}
