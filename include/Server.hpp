#pragma once

#include "webserv.hpp"

class Server {
private:
	ConfigServer* _config;
	const std::vector<std::pair<std::string, int> > &_listen;
	struct sockaddr_in _address;
	int _len_address;
	int _client_count;

public:
	Server(ConfigServer &config, const std::vector<std::pair<std::string, int> > &listen);
	~Server();
	//getter
	ConfigServer*	getConfigServer() {return _config;}
	//setter
	void initServerSocket(std::pair<std::string, int> ipPort);
	void addFdData(int fd, std::string ip, int port, Server *server, fd_status status , bool request);
	void addFdToFds(int fd_to_add);
	int createClientSocket(int fd);
	void decreaseClientCount();
};
