#include "Response.hpp"

Response::Response() {}

Response::~Response() {}

std::string	Response::make_response()
{
	std::ostringstream headers;
	time_t timestamp = time(NULL);
	char date_str[100];

	// get content type of output
	content_type = get_content_type();

	std::strftime(date_str, sizeof(date_str), "%a %b %d %H:%M:%S %Y", std::localtime(&timestamp));
	headers << "HTTP/1.1 " << code << " " << text << "\r\n"
		<< "Server: SeriousServer\r\n"
		<< "Date: " << date_str << "\r\n"
		<< "Content-Type: " << content_type << "\r\n"
		<< "Content-Length: " << resource.size() << "\r\n";
		if (is_redirection == true)
			headers << "Location: " << redir_location << "\r\n";
		headers << "Connection: keep-alive\r\n\r\n";
		headers << resource;

	    // if (method != "HEAD")
        //     headers << resource;


	std::cout << "HTTP/1.1 " << code << " " << text << "\r\n"
		<< "Server: SeriousServer\r\n"
		<< "Date: " << date_str << "\r\n"
		<< "Content-Type: " << content_type << "\r\n"
		<< "Content-Length: " << resource.size() << "\r\n";
		if (is_redirection == true)
			std::cout << "Location: " << redir_location << "\r\n";
		std::cout << "Connection: keep-alive\r\n\r\n";

		return (headers.str());
}

void Response::init_response(Request *request, Server *server)
{
	this->request = request;
	body = request->get_body();
	body_size = request->get_body_size();
	body_limit_exceeded = request->is_max_body_exceeded();
	method = request->get_method();
	request_uri = request->get_request_uri();
	http_version = request->get_http_version();
	server_block = &server->get_config();
	location_block = select_location(server_block->locations);
	init_resource();
}

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

// void	Response::get_full_path()
// {
// 	size_t	pos, dot;
// 	char path[1000];

// 	pos = request_uri.find_last_of('/');
// 	std::string execfile = request_uri.substr(pos + 1);
// 	dot = request_uri.find('.');

// 	realpath(location_block->get_root().c_str(), path);
// 	root = path;

// 	if (dot == std::string::npos && execfile.empty()) // directory request
// 		resource_full_path = root;
// 	else // it's a file request
// 		resource_full_path = root + request_uri;
// 	// Debug::display1("full path", resource_full_path);
// }

void Response::get_full_path()
{
    std::string root = location_block->get_root();
    std::string location_path = location_block->get_location_path();

    // Remove trailing /
    if (!root.empty() && root[root.length() - 1] == '/')
        root = root.substr(0, root.length() - 1);

    // Remove location path from request_uri to get the relative path
    std::string relative_path = request_uri;

    // If request_uri starts with location_path, remove it
    if (relative_path.find(location_path) == 0)
    {
        relative_path = relative_path.substr(location_path.length());
    }

    // Ensure relative_path starts with /
    if (relative_path.empty() || relative_path[0] != '/')
        relative_path += "/";

    // Build full path: root + relative_path
    resource_full_path = root + relative_path;

    // Debug
    std::cout << "=== PATH DEBUG ===" << std::endl;
    std::cout << "Request URI: " << request_uri << std::endl;
    std::cout << "Location path: " << location_path << std::endl;
    std::cout << "Root: " << root << std::endl;
    std::cout << "Relative path: " << relative_path << std::endl;
    std::cout << "Full path: " << resource_full_path << std::endl;
    std::cout << "==================" << std::endl;
}

void	Response::init_resource()
{
	is_redirection = false;

	if (is_cgi() && method != "DELETE")
		cgi_handler();
	else if (body_limit_exceeded)
	{
		set_status(HTTP_PAYLOAD_TOO_LARGE);
		resource = "<html><body><h1>" + code + " " + text + "</h1></body></html>";
		content_type = "text/html";
	}
	else if (location_block->get_return() != "" && method != "DELETE")
		handle_redirection();
	else if (method == "DELETE")
		delete_method();
	else
		static_file_handler();
}

std::string	Response::get_content_type()
{
	size_t		pos;
	std::string ext;
	std::string	cont_type;

	if (is_cgi())
		return (cgi_content_type());
	set_up_contenttypes();
	pos = resource_full_path.find_last_of(".");
	if (pos != std::string::npos)
		ext = resource_full_path.substr(pos);
	else
		ext = "";
	if (cont_types.find(ext) != cont_types.end())
		cont_type = cont_types[ext];
	else
		cont_type = "text/html";
	return (cont_type);
}

void	Response::set_up_contenttypes()
{
	cont_types[".txt"] = "text/plain";
	cont_types[".html"] = "text/html";
	cont_types[".csv"] = "text/csv";
	cont_types[".js"] = "text/javascript";
	cont_types[".xml"] = "text/xml";
	cont_types[".jpeg"] = "image/jpeg";
	cont_types[".jpg"] = "image/jpeg";
	cont_types[".png"] = "image/png";
	cont_types[".gif"] = "image/gif";
	cont_types[".tiff"] = "image/tiff";
	cont_types[".tif"] = "image/tiff";
	cont_types[".djvu"] = "image/vnd.djvu";
	cont_types[".json"] = "application/json";
	cont_types[".zip"] = "application/zip";
	cont_types["tif"] = "image/tiff";
	cont_types["djvu"] = "image/vnd.djvu";
	cont_types["json"] = "application/json";
	cont_types["zip"] = "application/zip";
}

std::string Response::get_error_page(int error_code)
{
	// Check if server has configured error page for this code
	std::map<int, std::string>::const_iterator it = server_block->error_pages.find(error_code);

	if (it != server_block->error_pages.end())
	{
		std::string error_page_path = it->second;
		std::string full_path;

		// same as regular file serving
		char new_root[1000];
		realpath(location_block->get_root().c_str(), new_root);
		full_path = std::string(new_root) + error_page_path;

		// Try to read the error page file
		std::ifstream error_file(full_path.c_str());
		if (error_file.is_open())
		{
			std::stringstream buffer;
			buffer << error_file.rdbuf();
			error_file.close();
			std::string content = buffer.str();
			if (!content.empty())
				return content;
		}
	}

	// Fallback to default
	return get_default_error_page();
}

std::string Response::get_default_error_page()
{
	std::stringstream html;

	html << "<!DOCTYPE html>\n";
	html << "<html lang=\"en\">\n";
	html << "<head>\n";
	html << "    <meta charset=\"UTF-8\">\n";
	html << "    <title>" << code << " " << text << "</title>\n";
	html << "    <style>\n";
	html << "        body { font-family: Arial, sans-serif; text-align: center; padding: 50px; }\n";
	html << "        h1 { font-size: 48px; margin-bottom: 20px; color: #e74c3c; }\n";
	html << "        p { font-size: 18px; color: #7f8c8d; }\n";
	html << "    </style>\n";
	html << "</head>\n";
	html << "<body>\n";
	html << "    <h1>" << code << " " << text << "</h1>\n";
	html << "    <p>An error occurred while processing your request.</p>\n";
	html << "</body>\n";
	html << "</html>\n";

	return html.str();
}
