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

int MAX_CLIENT = 1024;//by default but max is defined by system parameters(bash = ulimit -n)

#pragma once

class	Server {
	private:
		ConfigServer &	_config;
		std::vector<std::pair<std::string, int> > &	_listen;
		struct sockaddr_in	_address;
		int	_len_address;
		std::vector<struct pollfd> _ServerFds;//stock all fds of all servers
		std::vector<struct pollfd> _ClientFds;
		std::map<int, Request>	_clientFdRequest;
		static std::map<int, int>	mapPortFd;
	public:
		Server(ConfigServer & config, std::vector<std::pair<std::string, int> > & listen);
		~Server();
		class ServerException : public std::exception {
			private:
				std::string	_message;
			public:
				ServerException(std::string const & message) : _message(message) {}
				virtual ~ServerException() throw() {}
				virtual const char* what() const throw() {return _message.c_str();}
		};
		void	initServerSocket(std::pair<std::string, int> ipPort);
		void	addFdToServerFds(int fd_to_add);
		std::vector<struct pollfd> &	getServerFds() {return _ServerFds;}
		int	createClientSocket(int fd);
};
