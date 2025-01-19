#include "../include/ConfigServer.hpp"

ConfigServer::ConfigServer() {
    this->_ip = "*";
    this->_port = 80;
    this->_root = "";
    this->_limit_client_body_size = -1;
}

void    ConfigServer::setIp(const std::string& ip) {
    if (!ConfigParser::validIp(ip))
        throw "Invalid ip address";
    this->_ip = ip;
}

void    ConfigServer::setPort(const std::string& port) {
    int num = std::atoi(port.c_str());
    if (!ConfigParser::validPort(port))
        throw "Invalid port";
    this->_port = num;
}

void    ConfigServer::setServerNames(const std::string& server_name) {
    this->_server_names = splitString<std::list<std::string> >(server_name, ' ');
}

void    ConfigServer::setRoot(const std::string& root) {
    struct stat buffer;
    if (ConfigParser::validRoot(root) == false)
        throw "Invalid root";
    this->_root = root;
}


void    ConfigServer::setLimitClientBodySize(const std::string& value) {
    unsigned long num = std::stoul(value.c_str());
    unsigned long res;
    if (ConfigParser::validBodySize(value) == false)
        throw "Invalid body size or format";
    if (value[value.size() - 1] == 'k' || value[value.size() - 1] == 'K')
        res = num * 1024;
    else if (value[value.size() - 1] == 'm' || value[value.size() - 1] == 'M')
        res = num * 1024 * 1024;
    else
        res = num;
    this->_limit_client_body_size = res;
}

void    ConfigServer::setErrorPages(const std::string& line) {
    std::list<std::string> tmp_list = splitString<std::list<std::string> >(line, ' ');
    ErrorPage error_page;
    if (ConfigParser::validErrorPage(line) == false)
        throw "Invalid error page";
    while (tmp_list.size() > 0) {
        if (onlyDigits(tmp_list.front()))
            error_page.error_codes.insert(atoi(tmp_list.front().c_str()));
        else
            error_page.error_path = tmp_list.front();
        tmp_list.pop_front();
    }
    this->_error_pages.push_back(error_page);
}

void    ConfigServer::setLocations(const ConfigLocation& location) {
    //To do
    this->_locations.push_back(location);
}

std::ostream& operator<<(std::ostream& os, const ConfigServer& server) {
    os << "Server: " << server.getIp() << ":" << server.getPort() << std::endl;
    if (server.getServerNames().empty())
        os << "Server names: none" << std::endl;
    else {
        os << "Server names: ";
        printContainer(server.getServerNames());
    }
    os << "Root: " << server.getRoot() << std::endl;
    os << "Limit client body size: " << server.getLimitClientBodySize() << std::endl;
    os << "Error pages: " << std::endl;

    std::list<ErrorPage> error_pages = server.getErrorPages();
    for (std::list<ErrorPage>::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it) {
        os << "\tError codes: ";
        printContainer(it->error_codes);
        os << "\tError path: " << it->error_path << std::endl;
    }
    std::cout << std::endl;
    printContainer(server.getLocations());

    return os;
}
