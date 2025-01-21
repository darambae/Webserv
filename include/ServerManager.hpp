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
#include "Server.hpp"

class	ServerManager {
	private:
		std::vector<ConfigServer> &	_configs;
		std::vector<Server>	_servers;
		std::vector<struct pollfd> _all_fds;//stock all fds of all servers
		int	_serverFds;//number of door, entries, port where clients can connect
		std::map<int, Request>	_mapFdRequest;
		std::map<int, Server*>	_mapFDServer;
	public:
		ServerManager(std::vector<ConfigServer> & configs);
		~ServerManager() {}
		class ServerManagerException : public std::exception {
		private:
		std::string	_message;
		public:
		ServerManagerException(std::string const & message) : _message(message) {}
		virtual ~ServerManagerException() throw() {}
		virtual const char* what() const throw() {return _message.c_str();}
		};
		void	launchServers();
		void	addFdsToMapFdServer(Server* server,std::vector<struct pollfd> pollfd_vector);
		void	addClientToMapFdServer(Server* server, int new_client);
		void	cleanClientFd(int	FD);
};
