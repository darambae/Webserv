#include "../include/ServerManager.hpp"
#include "../include/Server.hpp"

//faire une fonction qui rajoute le fd et ses infos dans la map

ServerManager::ServerManager(const std::vector<ConfigServer> & configs) : _configs(configs) {}

void	ServerManager::launchServers() {
	//create one FD by odd of IP/port
		for (int i = 0; i < _configs.size(); ++i) {
			//cree une classe server par configServer
			_servers.push_back(new Server(_configs[i], _configs[i].getListen()));
		}
    while (true) {
        int poll_count = poll(ALL_FDS.data(), ALL_FDS.size(), -1);  // Wait indefinitely
        if (poll_count == -1)
            throw ServerManagerException("Poll failed");
		//if new connection on one port of one server
		for (int i = 0; i < ALL_FDS.size(); ++i) {
			if (ALL_FDS[i].revents & POLLIN) {
				if (FD_DATA[ALL_FDS[i].fd]->status == SERVER) {
					int new_client = FD_DATA[ALL_FDS[i].fd]->server->createClientSocket(ALL_FDS[i].fd);
					char *buffer;
					read(ALL_FDS[i].fd, buffer, 1024);
					// if (new_client != -1 && FD_DATA[new_client]->request->parseRequest() == -1)
					// 	cleanClientFd(new_client);
				}
				else {
					char *buffer;
					read(ALL_FDS[i].fd, buffer, 1024);
					std::cout << "the client with FD " << ALL_FDS[i].fd <<" send a request"<<std::endl;
				}
				// else if (FD_DATA[ALL_FDS[i].fd]->request->parseRequest() == -1)
				// 	cleanClientFd(ALL_FDS[i].fd);
			}
		}
    }
}

void	ServerManager::cleanClientFd(int FD) {
	//clean fd in _all_fds; _mapFd_data;
	close(FD);
	std::map<int, t_Fd_data*>::iterator	it = FD_DATA.find(FD);
	it->second->server->decreaseClientCount();
	// delete it->second->request;
	FD_DATA.erase(it);
	for (int i = 0; i < ALL_FDS.size(); ++i) {
		if (ALL_FDS[i].fd == FD) {
			ALL_FDS.erase(ALL_FDS.begin() + i);
			break;
		}
	}
}

ServerManager::~ServerManager() {
	for (size_t i = 0; i < _servers.size(); ++i)
	    delete _servers[i];
	if (FD_DATA.size() > 0) {
		for (std::map<int, t_Fd_data*>::iterator it = FD_DATA.begin(); it != FD_DATA.end(); ++it) {
			close(it->first);
			delete it->second;
		}
		FD_DATA.clear();
	}
	if (ALL_FDS.size() > 0)
		ALL_FDS.clear();
	std::cout<<"the ServerManager is closed"<<std::endl;
}
