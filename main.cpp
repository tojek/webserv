#include "webserv_config.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    std::string config_filepath = "default.conf";

    if (argc > 1) {
        config_filepath = argv[1];
    }

    std::cout << "Attempting to parse: " << config_filepath << std::endl;

    WebservConfig parsed_config = parse_config_file(config_filepath);

    std::cout << "\n--- Parsed Configuration ---" << std::endl;
    std::cout << "Server Host: " << parsed_config.main_server.host << std::endl;
    std::cout << "Server Port: " << parsed_config.main_server.port << std::endl;
    std::cout << "Server Name: " << parsed_config.main_server.server_name << std::endl;
    std::cout << "Client Max Body Size: " << parsed_config.main_server.client_max_body_size << " bytes" << std::endl;

    std::cout << "Error Pages:" << std::endl;
    for (std::map<int, std::string>::const_iterator it = parsed_config.main_server.error_pages.begin();
         it != parsed_config.main_server.error_pages.end(); ++it) {
        std::cout << "  " << it->first << ": " << it->second << std::endl;
    }
    std::cout << "--------------------------" << std::endl;

    std::cout << "Parsing successful!" << std::endl;

    return 0;
}
