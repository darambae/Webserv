#pragma once
#include <iostream>
#include <cstring> //memset...
#include <cstdlib> //exit ...
#include <sys/socket.h> // socket, bind, accept...
#include <netinet/in.h> // sockaddr_in
#include <unistd.h> //close...
#include <poll.h>
#include <vector>
#include "ConfigServer.hpp"
#include <algorithm>
#include <list>
#include <map>
#include "Request.hpp"

class	DataServer {
	private:
	struct sockaddr_in	_address_server;
	int _fd_server, _client_count, _bufferSize, _len_address, _max_clients;
	ConfigServer	_config_server;
	std::string	_buffer;
	Request	_request_server;
	public:
	DataServer() {
	_fd_server, _client_count = 0;
	_len_address = sizeof(_address_server);
	_max_clients = 1024;//by default but max is defined by system parameters(bash = ulimit -n)
	_bufferSize = _config_server.getLimitClientBodySize();
	_buffer.reserve(_bufferSize);//reserve a memory place

	initServerSocket();
    while (true) {
        int poll_count = poll(_fds.data(), _fds.size(), -1);  // Wait indefinitely
        if (poll_count == -1)
            ServerException("Poll failed");
        if (_fds[0].revents & POLLIN)//means new connection to port 8080
			createNewSocket();
        // Check all clients for incoming data
        for (int i = 1; i <= _fds.size(); ++i) {
            if (_fds[i].revents & POLLIN)//a socket client receive a message
				manageRequest(i);
        }
    }
	}
};
