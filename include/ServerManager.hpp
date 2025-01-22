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

class	Server;

enum fd_status
{
	CLIENT,
	SERVER,
};

typedef struct s_FD_data
{
	enum fd_status	status;
	Server*			server;
	Request*		request;
	int				port;
	std::string		ip;
}			t_Fd_data;

std::map<int, t_Fd_data*>	FD_DATA;
std::vector<struct pollfd> ALL_FDS;//stock all fds of all servers

class	ServerManager {
	private:
		std::vector<ConfigServer> &	_configs;
		std::vector<Server>	_servers;
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
		void	cleanClientFd(int	FD);
};
