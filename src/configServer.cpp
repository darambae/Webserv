#include "../include/configServer.hpp"

configServer::configServer() {
    ip = "*"; //OR "127.0.0.1"
    port = 80;
    root = "";
    limit_client_body_size = -1;
}

void    configServer::setIp(std::string ip) {
    if (!validIp(ip))
        throw "Invalid ip address";
    this->ip = ip;
}

void    configServer::setPort(int port) {
    if (port < 0 || port > 65535)
        throw "Invalid port number";
    this->port = port;
}

void    configServer::setServerNames(std::vector<std::string> server_names) {
    //To do
    this->server_names = server_names;
}

void    configServer::setRoot(std::string root) {
    //To do
    this->root = root;
}

void    configServer::setLimitClientBodySize(int limit_client_body_size) {
    //To do
    this->limit_client_body_size = limit_client_body_size;
}

void    configServer::setDefaultErrorPages(std::list<errorPage> default_error_pages) {
    //To do
    this->default_error_pages = default_error_pages;
}

void    configServer::setLocations(std::vector<configLocation> locations) {
    //To do
    this->locations = locations;
}


bool    configServer::validIp(std::string ip) {
    std::string  segment;
    size_t  pos;
    int     num;
    
    if (ip == "localhost")
        return true;
    while ((pos = ip.find(".")) != std::string::npos) {
        segment = ip.substr(0, pos);
        num = stoi(segment);
        if (num < 0 || num > 255)
            return false;
        ip = ip.substr(pos + 1);
    }
    num = stoi(ip);
    if (num < 0 || num > 255)
        return false;
    return true;
}