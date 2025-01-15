#include "../include/configServer.hpp"


configServer::configServer() {
    ip = "*"; //OR "127.0.0.1"
    port = 80;
    root = "";
    limit_client_body_size = -1;
}

void    configServer::setIp(const std::string& ip) {
    if (!configParser::validIp(ip))
        throw "Invalid ip address";
    this->ip = ip;
}

void    configServer::setPort(const std::string& port) {
    if (port < 0 || port > 65535)
        throw "Invalid port number";
    this->port = port;
}

void    configServer::setServerNames(const std::string& server_name) {
    //To do
    this->server_names.push_back(server_name);
}

void    configServer::setRoot(const std::string& root) {
    //To do
    this->root = root;
}

void    configServer::setLimitClientBodySize(const std::string& limit_client_body_size) {
    //To do
    this->limit_client_body_size = limit_client_body_size;
}

void    configServer::setErrorPages(const std::string& line) {
    std::list<std::string> tmp_list = splitString<std::list<std::string>>(line, ' ');
    errorPage error_page;
    while (tmp_list.size() > 0) {
        if (onlyDigits(tmp_list.front()) && stoi(tmp_list.front()) >= 300 && stoi(tmp_list.front()) <= 599)
            error_page.error_codes.insert(stoi(tmp_list.front()));
        else if (tmp_list.front().find("/") != std::string::npos)
            error_page.error_path = tmp_list.front();
        else
            throw "Invalid error page";
        tmp_list.pop_front();
    }
    getErrorPages().push_back(error_page);}

void    configServer::setLocations(const configLocation& location) {
    //To do
    this->locations.push_back(location);
}

