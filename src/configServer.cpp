#include "../include/ConfigServer.hpp"


ConfigServer::ConfigServer() {
    ip = "*"; //OR "127.0.0.1"
    port = 80;
    root = "";
    limit_client_body_size = -1;
}

void    ConfigServer::setIp(const std::string& ip) {
    if (!ConfigParser::validIp(ip))
        throw "Invalid ip address";
    this->ip = ip;
}

void    ConfigServer::setPort(const std::string& port) {
    int num = std::atoi(port.c_str());
    if (!ConfigParser::validPort(port))
        throw "Invalid port";
    this->port = num;
}

void    ConfigServer::setServerNames(const std::string& server_name) {
    if (server_name.empty())
        this->server_names.push_back("");
    else {
        std::vector<std::string> tmp_list = splitString<std::vector<std::string> >(server_name, ' ');
        this->server_names.insert(getServerNames().end(), tmp_list.begin(), tmp_list.end());
    }
}

void    ConfigServer::setRoot(const std::string& root) {
    struct stat buffer;
    if (ConfigParser::validRoot(root) == false)
        throw "Invalid root";
    this->root = root;
}


void    ConfigServer::setLimitClientBodySize(const std::string& value) {
    int num = std::atoi(value.c_str());
    unsigned long res;

    if (ConfigParser::validBodySize(value) == false)
        throw "Invalid body size or format";
    if (onlyDigits(value))
        res = num;
    else if (value[value.size() - 1] == 'k' || value[value.size() - 1] == 'K')
        res = num * 1024;
    else if (value[value.size() - 1] == 'm' || value[value.size() - 1] == 'M')
        res = num * 1024 * 1024; 
    this->limit_client_body_size = res;
}

void    ConfigServer::setErrorPages(const std::string& line) {
    std::list<std::string> tmp_list = splitString<std::list<std::string> >(line, ' ');
    errorPage error_page;
    if (ConfigParser::validErrorPage(line) == false)
        throw "Invalid error page";
    while (tmp_list.size() > 0) {
        if (onlyDigits(tmp_list.front()))
            error_page.error_codes.insert(atoi(tmp_list.front().c_str()));
        else
            error_page.error_path = tmp_list.front();
        tmp_list.pop_front();
    }
    this->error_pages.push_back(error_page);
}

void    ConfigServer::setLocations(const ConfigLocation& location) {
    //To do
    this->locations.push_back(location);
}

std::ostream& operator<<(std::ostream& os, const ConfigServer& server) {
    os << "Server: " << server.getIp() << ":" << server.getPort() << std::endl;
    os << "Root: " << server.getRoot() << std::endl;
    os << "Limit client body size: " << server.getLimitClientBodySize() << std::endl;
    os << "Error pages: " << std::endl;
    // std::vector<ErrorPage>::const_iterator it;
    // for (it = server.getErrorPages().begin(); it != server.getErrorPages().end(); ++it) {
    //     os << "Error path: " << it->error_path << std::endl;
    //     os << "Error codes: ";

    //     std::vector<int>::const_iterator it2;
    //     for (it2 = it->error_codes.begin(); it2 != it->error_codes.end(); ++it2) {
    //         os << *it2 << " ";
    //     }
    //     os << std::endl;
    // }
    // os << "Locations: " << std::endl;
    // std::vector<ConfigLocation>::const_iterator it;
    // for (it = server.getLocations().begin(); it != server.getLocations().end(); ++it)
    //     os << *it << std::endl;
    return os;
}