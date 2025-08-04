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

    // Multiple server support
    int                             master_epoll_fd;
    struct epoll_event              events[64];
    std::map<int, Server*>          fd_to_server_map;  // Map file descriptors to their servers

    // Helper functions
    void    create_servers_from_config(const Config& config, size_t config_index);
    void    create_single_server(const Config& config, const ListenConfig& listen_conf);
    void    setup_master_epoll();
    Server* find_server_by_fd(int fd);
    Server* find_server_by_host_port(const std::string& host, int port);

    // Multiple server helper functions
    void    initialize_all_servers();
    void    handle_new_connection(int server_fd, Server* server);
    void    handle_client_request(int client_fd, Server* server);
    void    process_events(int num_events);

public:
    ServerManager(const ConfigParser& parser);
    ~ServerManager();

    void    init_servers();
    void    show_status();
    size_t  get_server_count() const;
    void    run_single_server();  // Simple method to run just the first server
    void    run_multiple_servers();  // New method to run all servers
};

#endif
