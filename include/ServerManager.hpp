
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

		void	launchServers();
		void	handlePollhup(int FD);
		void	handlePollin(int FD);
		void	handlePollout(int FD);
		void	cleanFd(int	FD);
		void	closeCgi(int errorNumber, int FdCGI);
		void	print_all_FD_DATA();
		void	print_FD_status(int FD);
};
