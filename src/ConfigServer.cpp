#include "../include/ConfigServer.hpp"

ConfigServer::ConfigServer() {
    this->_root = "";
    this->_limit_client_body_size = -1;
}

void    ConfigServer::setListen(const std::string& ip, const std::string& port) {
    int num = std::atoi(port.c_str());
    ConfigParser::validIp(ip);
    ConfigParser::validPort(port);
    std::string ip_string;
    if (ip == "localhost")
        ip_string = "127.0.0.1";
    else if (ip == "*")
        ip_string = "0.0.0.0";
    else
        ip_string = ip;
    this->_listen.push_back(std::make_pair(ip_string, num));
}

void    ConfigServer::setServerNames(const std::string& server_name) {
    this->_server_names = splitString<std::vector<std::string> >(server_name, ' ');
}

void    ConfigServer::setRoot(const std::string& root) {
    struct stat buffer;
    ConfigParser::validRoot(root);
    this->_root = root;
}

void    ConfigServer::setLimitClientBodySize(const std::string& value) {
    unsigned long num = std::strtoul(value.c_str(), NULL, 10);
    unsigned long res;
    ConfigParser::validBodySize(value);
    if (value[value.size() - 1] == 'k' || value[value.size() - 1] == 'K')
        res = num * 1024;
    else if (value[value.size() - 1] == 'm' || value[value.size() - 1] == 'M')
        res = num * 1024 * 1024;
    else
        res = num;
    this->_limit_client_body_size = res;
}

void    ConfigServer::setErrorPages(const std::string& line) {
    std::vector<std::string> tmp_vector = splitString<std::vector<std::string> >(line, ' ');
    ErrorPage error_page;
    ConfigParser::validErrorPage(line);
    while (tmp_vector.size() > 0) {
        if (onlyDigits(tmp_vector.front()))
            error_page.error_codes.insert(atoi(tmp_vector.front().c_str()));
        else
            error_page.error_path = tmp_vector.front();
        tmp_vector.erase(tmp_vector.begin());
    }
    this->_error_pages.push_back(error_page);
}

void    ConfigServer::setLocations(const ConfigLocation& location) {
    this->_locations.push_back(location);
}

void    ConfigServer::setDefaultErrorPages() {
    ErrorPage error_page;
    error_page.error_codes.insert(404);
    error_page.error_path = "/data/www/errors/404.html";
    this->_error_pages.push_back(error_page);
}

void    ConfigServer::setDefaultListen() {
    this->_listen.push_back(std::make_pair("0.0.0.0", 8080));
}

std::ostream& operator<<(std::ostream& os, ConfigServer server) {
    os << "Server : " << std::endl;
    if (!server.getListen().empty()) {
        std::vector<std::pair<std::string, int> > listens = server.getListen();
        for (std::vector<std::pair<std::string, int> >::const_iterator it = listens.begin(); it != listens.end(); ++it) {
            os << "\tListen: " << it->first << ":" << it->second << std::endl;
        }
    }
    if (server.getServerNames().empty())
        os << "\tServer names: none" << std::endl;
    else {
        os << "\tServer names: ";
        printContainer(server.getServerNames());
    }
    if (!server.getRoot().empty())
        os << "\tRoot: " << server.getRoot() << std::endl;
    if (server.getLimitClientBodySize() != -1)
        os << "\tLimit client body size: " << server.getLimitClientBodySize() << std::endl;
    if (!server.getErrorPages().empty()) {
        os << "\tError pages: " << std::endl;
        std::vector<ErrorPage> error_pages = server.getErrorPages();
        for (std::vector<ErrorPage>::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it) {
            os << "\t\tError codes: ";
            printContainer(it->error_codes);
            os << "\t\tError path: " << it->error_path << std::endl;
        }
        std::cout << std::endl;
    }
    printContainer(server.getLocations());
    return os;
}

