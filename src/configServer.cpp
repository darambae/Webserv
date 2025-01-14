#include "../include/configServer.hpp"


configServer::configServer() {
    ip = "*"; //OR "127.0.0.1"
    port = 80;
    root = "";
    limit_client_body_size = -1;
}

void    configServer::setIp(std::string ip) {
    if (!configParser::validIp(ip))
        throw "Invalid ip address";
    this->ip = ip;
}

void    configServer::setPort(int port) {
    if (port < 0 || port > 65535)
        throw "Invalid port number";
    this->port = port;
}

void    configServer::setServerNames(std::string server_name) {
    //To do
    this->server_names.push_back(server_name);
}

void    configServer::setRoot(std::string root) {
    //To do
    this->root = root;
}

void    configServer::setLimitClientBodySize(int limit_client_body_size) {
    //To do
    this->limit_client_body_size = limit_client_body_size;
}

void    configServer::setDefaultErrorPages(errorPage default_error_page) {
    //To do
    this->default_error_pages.push_back(default_error_page);
}

void    configServer::setLocations(configLocation location) {
    //To do
    this->locations.push_back(location);
}

