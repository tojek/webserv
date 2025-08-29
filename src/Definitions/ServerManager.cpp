#include "ServerManager.hpp"
#include "Server.hpp"
#include "Webserv.hpp"
#include "Signal.hpp"
#include "Request.hpp"
#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>

ServerManager::ServerManager(const ConfigParser& parser) : config_parser(parser), master_epoll_fd(-1)
{
}

// Clean up servers & configs
ServerManager::~ServerManager()
{
    for (size_t i = 0; i < servers.size(); i++)
        delete servers[i];

    for (size_t i = 0; i < server_configs.size(); i++)
        delete server_configs[i];

    if (master_epoll_fd != -1)
        close(master_epoll_fd);
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

        // Check if we already have a server for this host:port
        if (!server_exists_for_port(listen_conf.host, listen_conf.port))
        {
            create_single_server(config, listen_conf);
        }
        else
        {
            std::cout << "Server already exists for " << listen_conf.host
                     << ":" << listen_conf.port << " - skipping socket creation" << std::endl;
        }
    }
}

bool ServerManager::server_exists_for_port(const std::string& host, int port)
{
    for (size_t i = 0; i < servers.size(); i++)
    {
        const Config& config = servers[i]->get_config();
        if (!config.listen_configs.empty())
        {
            const ListenConfig& listen_conf = config.listen_configs[0];
            if (listen_conf.port == port &&
                (listen_conf.host == host || listen_conf.host == "0.0.0.0" || host == "0.0.0.0"))
                return true;
        }
    }
    return false;
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

size_t ServerManager::get_server_count() const
{
    return servers.size();
}

void ServerManager::setup_master_epoll()
{
    master_epoll_fd = epoll_create1(0);
    if (master_epoll_fd == -1)
        error("Failed to create master epoll instance");

    // Add all server file descriptors to the master epoll
    for (size_t i = 0; i < servers.size(); i++)
    {
        Server* server = servers[i];
        int server_fd = server->get_server();
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = server_fd;

        if (epoll_ctl(master_epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1)
        {
            std::cerr << "Failed to add server fd " << server_fd << " to master epoll" << std::endl;
            error("epoll_ctl failed");
        }

        // Map server fd to server instance
        fd_to_server_map[server_fd] = server;

        std::cout << "Added server fd " << server_fd << " to master epoll" << std::endl;
    }
}

Server* ServerManager::find_server_by_fd(int fd)
{
    std::map<int, Server*>::iterator it = fd_to_server_map.find(fd);
    if (it != fd_to_server_map.end())
        return it->second;

    // If not a server fd, search through all servers client fds
    for (size_t i = 0; i < servers.size(); i++)
    {
        std::map<int, Client>& clients = servers[i]->get_clients();
        if (clients.find(fd) != clients.end())
            return servers[i];
    }

    return NULL;
}

Server* ServerManager::find_server_by_host_port(const std::string& host, int port)
{
    // get the matching config
    const Config* config = config_parser.find_server(host, port);
    if (!config)
        return NULL;

    // Find the server that matches this config
    for (size_t i = 0; i < servers.size(); i++)
    {
        const Config& server_config = servers[i]->get_config();
        if (&server_config == config)
            return servers[i];
    }

    return NULL;
}

// void ServerManager::initialize_all_servers()
// {
//     std::cout << "Initializing all servers..." << std::endl;

//     for (size_t i = 0; i < servers.size(); i++)
//     {
//         servers[i]->init_epoll();
//         const Config& config = servers[i]->get_config();
//         const ListenConfig& listen_conf = config.listen_configs[0];
//         std::cout << "Server " << (i + 1) << " ready on "
//                   << listen_conf.host << ":" << listen_conf.port << std::endl;
//     }
// }

void ServerManager::handle_new_connection(int server_fd, Server* server)
{
	(void)server_fd;

    int client_fd = Client::accept_client(*server);
    if (client_fd > 0)
    {
        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = client_fd;
        std::cout << LIGHT_BLUE << "New connection on server fd " << server_fd << " accepted, client fd: " << client_fd << RESET << std::endl;
        if (epoll_ctl(master_epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1)
        {
            std::cerr << "Failed to add client fd " << client_fd << " to master epoll" << std::endl;
            close(client_fd);
        }
        else
        {
            std::cout << "New client connection accepted on fd " << client_fd << std::endl;
        }
    }
}


void ServerManager::handle_client_request(int client_fd, Server* server)
{
    Client* client = Client::find_client(*server, client_fd);
    if (client)
    {
        client->read_request();
        if (client->request->is_request_complete())
        {
            std::cout << "ready to response\n";
            client->send_response(*server);
            
            if (client->connection_status == false)
            {
                // delete client;
               epoll_ctl(master_epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
               std::cout << "Handled request and closed connection on fd " << client_fd << std::endl;
            }
            else
               std::cout << "Keeping connection alive on fd " << client_fd << std::endl;
            client->delete_request();

        }
    }
    else
        std::cerr << "Warning: Could not find client for fd " << client_fd << std::endl;
}

void ServerManager::process_events(int num_events)
{
    for (int i = 0; i < num_events; i++)
    {
        int event_fd = events[i].data.fd;
        Server* server = find_server_by_fd(event_fd);

        if (!server)
        {
            std::cerr << "Warning: Could not find server for fd " << event_fd << std::endl;
            continue;
        }

        // Check if this is a server socket (new connection)
        if (fd_to_server_map.find(event_fd) != fd_to_server_map.end())
            handle_new_connection(event_fd, server);
        else
            handle_client_request(event_fd, server);
    }
}

void ServerManager::event_loop()
{
    while (g_signal_state.sigint == 0 && g_signal_state.sigterm == 0)
    {
        int num_events = epoll_wait(master_epoll_fd, events, 64, -1);

        if (num_events == -1 && errno != EINTR)
            return error("master epoll_wait error.");

        if (num_events > 0)
            process_events(num_events);
    }
}

void ServerManager::run_multiple_servers()
{
    if (servers.empty())
    {
        std::cerr << "No servers to run!" << std::endl;
        return;
    }

    std::cout << "Starting multiple server mode..." << std::endl;
    std::cout << "Running " << servers.size() << " server(s)" << std::endl;
    std::cout << "Press Ctrl+C to stop all servers" << std::endl;

    // initialize_all_servers();
    setup_master_epoll();
	event_loop();

    std::cout << "All servers stopped." << std::endl;
}
