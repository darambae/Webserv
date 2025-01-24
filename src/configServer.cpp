#include "../include/ConfigServer.hpp"

ConfigServer::ConfigServer() {
    this->_root = "";
    this->_limit_client_body_size = -1;
}

void    ConfigServer::setListen(const std::string& ip, const std::string& port) {
    int num = std::atoi(port.c_str());
    if (!ConfigParser::validIp(ip))
        THROW("Invalid ip");
    if (!ConfigParser::validPort(port))
        THROW("Invalid port");
    this->_listen.push_back(std::make_pair(ip, num));
}

void    ConfigServer::setServerNames(const std::string& server_name) {
    this->_server_names = splitString<std::vector<std::string> >(server_name, ' ');
}

void    ConfigServer::setRoot(const std::string& root) {
    struct stat buffer;
    if (ConfigParser::validRoot(root) == false)
        THROW("Invalid root"); 
    this->_root = root;
}

void    ConfigServer::setLimitClientBodySize(const std::string& value) {
    unsigned long num = std::strtoul(value.c_str(), NULL, 10);
    unsigned long res;
    if (ConfigParser::validBodySize(value) == false)
        THROW("Invalid body size or format");
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
    if (ConfigParser::validErrorPage(line) == false)
        THROW("Invalid error page");
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

std::ostream& operator<<(std::ostream& os, const ConfigServer& server) {
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
