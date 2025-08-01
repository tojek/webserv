#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "Config.hpp"
#include "ConfigParser.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include <vector>
#include <map>

class ServerManager
{
private:
    std::vector<Server*>            servers;
    std::vector<Config*>            server_configs;  // Store config pointers to avoid reallocation issues
    const ConfigParser&             config_parser;

    // Helper functions
    void    create_servers_from_config(const Config& config, size_t config_index);
    void    create_single_server(const Config& config, const ListenConfig& listen_conf);

public:
    ServerManager(const ConfigParser& parser);
    ~ServerManager();

    void    init_servers();
    void    show_status();
    void    run_single_server();  // Simple method to run just the first server
};

#endif
