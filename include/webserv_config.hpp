#pragma once

#include <string>
#include <vector>
#include <map>

struct ServerConfig {
    std::string host;              			// IP address or hostname
    int         port;              			// Port number
    std::string server_name;
    size_t      client_max_body_size; 		// Maximum client body size in bytes
    std::map<int, std::string> error_pages; // Map of error codes to error page paths

    ServerConfig() : host("0.0.0.0"), port(80), server_name(""), client_max_body_size(1048576) {}
};

struct WebservConfig {
    ServerConfig main_server;
};

// Function declaration for the configuration parser
WebservConfig parse_config_file(const std::string& filepath);

