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
#include "webserv.hpp"

class	Server;

class	ServerManager {
	private:
		const std::vector<ConfigServer> &	_configs;
		std::vector<Server>	_servers;
	public:
		ServerManager(const std::vector<ConfigServer> & configs);
		~ServerManager();
		class ServerManagerException : public std::exception {
		private:
		std::string	_message;
		public:
		ServerManagerException(std::string const & message) : _message(message) {}
		virtual ~ServerManagerException() throw() {}
		virtual const char* what() const throw() {return _message.c_str();}
		};
		void	launchServers();
		void	cleanClientFd(int	FD);
};
