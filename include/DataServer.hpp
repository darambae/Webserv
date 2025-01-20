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

	_server_fd = socket(AF_INET, SOCK_STREAM, 0); // SOCK_STREAM : TCP socket
    if (_server_fd == -1)
        throw ServerException("Socket creation failed");
    std::cout << "Server socket created on sd " << _server_fd << std::endl;
/*in case of server crach, this setting allow to reuse the port */
	int opt = 1;
    if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) { //Checking if the socket is already in use
        close(_server_fd);
		throw ServerException("setsockopt failed");
	}
/*init server socket*/
	_address.sin_family = AF_INET; //IPv4
    _address.sin_addr.s_addr = INADDR_ANY; //Tells the server to bind to all available network interfaces (not just a specific IP)
    _address.sin_port = htons(_config.getPort()); //Converts the port number to "network byte order"
    if (bind(_server_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0){
        //std::cerr << "Bind failed" << std::endl; // When bind fails, on terminal "sudo lsof -i :8080" & "sudo kill 8080" can be used to free the port.
        close(_server_fd);
		throw ServerException("Bind failed");
    }
    if (listen(_server_fd, 10) < 0){//make serverfd listening new connections, 10 connections max can wait to be accepted
        close(_server_fd);
        throw ServerException("Listen failed");
    }
    std::cout << "Server is listening on port " << _config.getPort() << std::endl;
	addFdToFds(_server_fd);
	//maybe not necessary if we use vector
	//for (int i = 1; i <= _max_clients; ++i) {
    //     fds[i].fd = -1;  // Set initial state as no client connected
    // }
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
