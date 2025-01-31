
#pragma once

#include "webserv.hpp"

class	Server;
class	ConfigServer;
class	ServerManager {
	private:
		std::vector<ConfigServer>	_configs;
		std::vector<Server*>	_servers;
	public:
		ServerManager(const std::vector<ConfigServer>& configs);
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
