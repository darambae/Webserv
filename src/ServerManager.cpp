#include "../include/ServerManager.hpp"

ServerManager::ServerManager(std::vector<ConfigServer> & configs) : _configs(configs) {}

void	ServerManager::launchServers() {
	//create one FD by odd of IP/port
		for (int i = 0; i < _configs.size(); ++i) {
			//cree une classe server par configServer
			Server	server(_configs[i], _configs[i].getListen());
			_servers.push_back(server);
			addFdsToMapFdServer(&server, server.getServerFds());
			_all_fds.insert(_all_fds.end(), server.getServerFds().begin(), server.getServerFds().end());
		}
		_serverFds = _all_fds.size();
    while (true) {
        int poll_count = poll(_all_fds.data(), _all_fds.size(), -1);  // Wait indefinitely
        if (poll_count == -1)
            throw ServerManagerException("Poll failed");
		//if new connection on one port of one server
		for (int i = 0; i < _serverFds; ++i) {
			if (_all_fds[i].revents & POLLIN) {
				int new_client = _mapFDServer[_all_fds[i].fd]->createClientSocket(_all_fds[i].fd);
				addClientToMapFdServer(_mapFDServer[_all_fds[i].fd], new_client);
			}
		}
        // Check all clients for incoming data
        for (int i = _serverFds; i < _all_fds.size(); ++i) {
            if (_all_fds[i].revents & POLLIN && _mapFdRequest[_all_fds[i].fd].parseRequest() == -1)
				cleanClientFd(_all_fds[i].fd);
        }
    }
}

void	ServerManager::addFdsToMapFdServer(Server* server,std::vector<struct pollfd> pollfd_vector) {
	for (int i = 0; i < pollfd_vector.size(); ++i) {
		_mapFDServer[pollfd_vector[i].fd] = server;
	}
}

void	ServerManager::cleanClientFd(int	FD) {
	//clean fd in _all_fds; _mapFdRequest; _mapFDServer; _clientFdRequest
}
