#include "../include/ConfigServer.hpp"


ConfigServer::ConfigServer() {
    _ip = "*"; //OR "127.0.0.1"
    _port = 80;
    _root = "";
    _limit_client_body_size = -1;
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
    if (server_name.empty())
        this->_server_names.push_back("");
    else {
        std::vector<std::string> tmp_list = splitString<std::vector<std::string> >(server_name, ' ');
        this->_server_names.insert(getServerNames().end(), tmp_list.begin(), tmp_list.end());
    }
}

void    ConfigServer::setRoot(const std::string& root) {
    struct stat buffer;
    if (ConfigParser::validRoot(root) == false)
        throw "Invalid root";
    this->_root = root;
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
    this->_limit_client_body_size = res;
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
    this->_error_pages.push_back(error_page);
}

void    ConfigServer::setLocations(const ConfigLocation& location) {
    //To do
    this->_locations.push_back(location);
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

int	ConfigServer::createServerFd() {
	_fd_server, _client_count = 0;
	_len_address = sizeof(_address);
	_max_clients = 1024;//by default but max is defined by system parameters(bash = ulimit -n)

	_fd_server = socket(AF_INET, SOCK_STREAM, 0); // SOCK_STREAM : TCP socket
    if (_fd_server == -1)
        throw ServerException("Socket creation failed");
    std::cout << "Server socket created on sd " << _fd_server << std::endl;
/*in case of server crach, this setting allow to reuse the port */
	int opt = 1;
    if (setsockopt(_fd_server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) { //Checking if the socket is already in use
        close(_fd_server);
		throw ServerException("setsockopt failed");
	}
/*init server socket*/
	_address.sin_family = AF_INET; //IPv4
    _address.sin_addr.s_addr = INADDR_ANY; //Tells the server to bind to all available network interfaces (not just a specific IP)
    _address.sin_port = htons(getPort()); //Converts the port number to "network byte order"
    if (bind(_fd_server, (struct sockaddr *)&_address, sizeof(_address)) < 0){
        //std::cerr << "Bind failed" << std::endl; // When bind fails, on terminal "sudo lsof -i :8080" & "sudo kill 8080" can be used to free the port.
        close(_fd_server);
		throw ServerException("Bind failed");
    }
    if (listen(_fd_server, 10) < 0){//make serverfd listening new connections, 10 connections max can wait to be accepted
        close(_fd_server);
        throw ServerException("Listen failed");
    }
    std::cout << "Server is listening on port " << _port << std::endl;
	return (_fd_server);
};
