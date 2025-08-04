#include "Request.hpp"
#include "Response.hpp"

std::string Response::get_method()
{
	std::ostringstream headers;

	headers << http_version << " " << code << " " << text << "\r\n"
			<< "Content-Type: " << content_type << "\r\n"
			<< "Content-Length: " << resource.size() << "\r\n"
			<< "Connection: close \r\n\r\n"
			<< resource;
	// file_content.close();
	return (headers.str());
}

std::string Response::post_method()
{
	std::ostringstream headers;

	headers << http_version << " " << code << " " << text << "\r\n"
			<< "Content-Type: " << content_type << "\r\n"
			<< "Content-Length: " << resource.size() << "\r\n"
			<< "Connection: close \r\n\r\n"
			<< resource;
	// file_content.close();
	return (headers.str());}

std::string Response::delete_method()
{
	return ("");
}