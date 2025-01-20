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
#include "DataServer.hpp"

#pragma once

class	Server {
	private:
		std::vector<ConfigServer>	_config;
		std::vector<struct pollfd> _fds;

		Server() {}
	public:
	class ServerException : public std::exception {
		private:
		std::string	_message;
		public:
		ServerException(std::string const & message) : _message(message) {}
		virtual ~ServerException() throw() {}
		virtual const char* what() const throw() {return _message.c_str();}
	};
	~Server();
	Server(std::list<ConfigServer> & config);
	void	launchServers() {
		for (int i = 0; i != _config.size(); ++i) {
			addFdToFds(_config[i].createServerFd());
		}
    while (true) {
        int poll_count = poll(_fds.data(), _fds.size(), -1);  // Wait indefinitely
        if (poll_count == -1)
            throw ServerException("Poll failed");
        if (_fds[0].revents & POLLIN)//means new connection to port 8080
			createNewSocket();
        // Check all clients for incoming data
        for (int i = 1; i <= _fds.size(); ++i) {
            if (_fds[i].revents & POLLIN)//a socket client receive a message
				manageRequest(i);
        }
    }
	}
	void	addServer(ConfigServer config);
	void	addFdToFds(int fd_to_add);
	void	initServerSocket();
	void	createNewSocket();
	void	manageRequest(int i);
};
