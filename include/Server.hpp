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
		std::vector<DataServer>	_data_server;
		std::list<ConfigServer>	_config;
		std::string		_buffer;
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
	void	addServer(ConfigServer config);
	void	addFdToFds(int fd_to_add);
	void	initServerSocket();
	void	createNewSocket();
	void	manageRequest(int i);
};
