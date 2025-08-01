#include "ServerManager.hpp"
#include "Server.hpp"
#include "Webserv.hpp"
#include <iostream>

ServerManager::ServerManager(const ConfigParser& parser) : config_parser(parser)
{
}

// Clean up servers & configs
ServerManager::~ServerManager()
{
    for (size_t i = 0; i < servers.size(); i++)
        delete servers[i];

    for (size_t i = 0; i < server_configs.size(); i++)
        delete server_configs[i];
}

// Initialize servers based on the config file from config_parser
void ServerManager::init_servers()
{
    const std::vector<Config>& configs = config_parser.get_servers();

    std::cout << "Found " << configs.size() << " server configuration(s)" << std::endl;

    if (configs.empty())
    {
        std::cerr << "No server configurations found!" << std::endl;
        return;
    }

    // Process each server configuration
    for (size_t i = 0; i < configs.size(); i++)
        create_servers_from_config(configs[i], i);

    std::cout << "Total servers created: " << servers.size() << std::endl;
}

    // Create one server for each listen
void ServerManager::create_servers_from_config(const Config& config, size_t config_index)
{
    std::cout << "\nProcessing server block " << (config_index + 1) << std::endl;
    std::cout << "Server name: " << (config.server_name.empty() ? "(default)" : config.server_name) << std::endl;
    std::cout << "Listen directives: " << config.listen_configs.size() << std::endl;

    for (size_t j = 0; j < config.listen_configs.size(); j++)
    {
        const ListenConfig& listen_conf = config.listen_configs[j];
        create_single_server(config, listen_conf);
    }
}

void ServerManager::create_single_server(const Config& config, const ListenConfig& listen_conf)
{
    std::cout << "Creating server " << (servers.size() + 1)
              << " for " << listen_conf.host << ":" << listen_conf.port << std::endl;

    try
    {
        // Create a config with only this one listen
        Config* single_listen_config = new Config(config);
        single_listen_config->listen_configs.clear();
        single_listen_config->listen_configs.push_back(listen_conf);

        // Store the config pointer
        server_configs.push_back(single_listen_config);

        Server* server = new Server(*single_listen_config);
        servers.push_back(server);

        std::cout << "Server created successfully for " << listen_conf.host
                 << ":" << listen_conf.port << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to create server for " << listen_conf.host
                 << ":" << listen_conf.port << " - " << e.what() << std::endl;
        throw;
    }
}

void ServerManager::show_status()
{
    std::cout << "\n=== Server Status ===" << std::endl;

    if (servers.empty())
    {
        std::cout << "No servers created." << std::endl;
        return;
    }

    for (size_t i = 0; i < servers.size(); i++)
    {
        const Config& config = servers[i]->get_config();
        const ListenConfig& listen_conf = config.listen_configs[0];

        std::cout << "Server " << (i + 1) << ":" << std::endl;
        std::cout << "  Address: " << listen_conf.host << ":" << listen_conf.port << std::endl;
        std::cout << "  Server Name: " << (config.server_name.empty() ? "(default)" : config.server_name) << std::endl;
        std::cout << "  Max Body Size: " << config.client_max_body_size << " bytes" << std::endl;
        std::cout << "  Locations: " << config.locations.size() << std::endl;
        std::cout << "  Status: READY" << std::endl;
        std::cout << std::endl;
    }
}

void ServerManager::run_single_server()
{
    if (servers.empty())
    {
        std::cerr << "No servers to run!" << std::endl;
        return;
    }

    std::cout << "Running only the first server for now..." << std::endl;
    std::cout << "Press Ctrl+C to stop the server" << std::endl;

    // For now, just run the first server to avoid complexity
    Server* server = servers[0];

    const Config& config = server->get_config();
    const ListenConfig& listen_conf = config.listen_configs[0];
    std::cout << "Starting server on " << listen_conf.host << ":" << listen_conf.port << std::endl;

    server->init_epoll();
    server->event_loop();

    std::cout << "Server stopped." << std::endl;
}
