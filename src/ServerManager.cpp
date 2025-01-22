#include "../include/ServerManager.hpp"
#include "../include/Server.hpp"

//faire une fonction qui rajoute le fd et ses infos dans la map

ServerManager::ServerManager(std::vector<ConfigServer> & configs) : _configs(configs) {}

void	ServerManager::launchServers() {
	//create one FD by odd of IP/port
		for (int i = 0; i < _configs.size(); ++i) {
			//cree une classe server par configServer
			Server	server(_configs[i], _configs[i].getListen());
			_servers.push_back(server);
			//addFdsToMapFdServer(&server, server.getServerFds());
			_all_fds.insert(_all_fds.end(), server.getServerFds().begin(), server.getServerFds().end());
		}
		//_serverFds = _all_fds.size();
    while (true) {
        int poll_count = poll(_all_fds.data(), _all_fds.size(), -1);  // Wait indefinitely
        if (poll_count == -1)
            throw ServerManagerException("Poll failed");
		//if new connection on one port of one server
		for (int i = 0; i < _all_fds.size(); ++i) {
			if (_all_fds[i].revents & POLLIN) {
				if (FD_DATA[_all_fds[i].fd].status == SERVER) {
					int new_client = FD_DATA[_all_fds[i].fd].server.createClientSocket(_all_fds[i].fd);
					FD_DATA[new_client].request->parseRequest();
				}
				else if (FD_DATA[_all_fds[i].fd].request->parseRequest() == -1)
					cleanClientFd(_all_fds[i].fd);
			}
		}
    }
}

void	ServerManager::cleanClientFd(int	FD) {
	//clean fd in _all_fds; _mapFd_data;
}
