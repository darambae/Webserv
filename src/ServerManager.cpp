
#include "../include/ServerManager.hpp"
//#include "../include/Request.hpp"

//faire une fonction qui rajoute le fd et ses infos dans la map

ServerManager::ServerManager(const std::vector<ConfigServer>& configs) : _configs(configs) {}

void	ServerManager::launchServers() {
	//create one FD by odd of IP/port
	for (size_t i = 0; i < _configs.size(); ++i) {
		//cree une classe server par configServer
		_servers.push_back(new Server(_configs[i], _configs[i].getListen()));
	}
    while (stopProgram != 1) {
        int poll_count = poll(ALL_FDS.data(), ALL_FDS.size(), -1);  // Wait indefinitely
        if (poll_count == -1)
            THROW("Poll failed");
		//if new connection on one port of one server
		for (int i = ALL_FDS.size() - 1; i >= 0; --i) {
			if (ALL_FDS[i].revents == 0)
				continue;

			if (ALL_FDS[i].revents & POLLHUP) {
				LOG_INFO("POLLHUP signal");
				int hangup_fd = ALL_FDS[i].fd;
				//LOG_ERROR("the client with FD : "+to_string(ALL_FDS[i].fd)+" is disconnected", 0);
				cleanClientFd(hangup_fd);
				continue;
			}

			if (ALL_FDS[i].revents & POLLIN) {
				//LOG_INFO("POLLIN signal");
				int	readable_FD = ALL_FDS[i].fd;
				if (FD_DATA[readable_FD]->status == SERVER) {
					int new_client = FD_DATA[readable_FD]->server->createClientSocket(readable_FD);
					if (new_client != -1 && FD_DATA[new_client]->request->handleRequest() == -1) {
						LOG_INFO("The status of FD_DATA[" + to_string(readable_FD) +"] : SERVER");
						//LOG_ERROR(": "+to_string(new_client)+" is disconnected", 0);
					}
				} else if (FD_DATA[readable_FD]->status == CLIENT) {
					if (FD_DATA[readable_FD]->request->handleRequest() == -1) {
						LOG_INFO("The status of FD_DATA[" + to_string(readable_FD) +"] : CLIENT");
						//LOG_ERROR(": "+to_string(ALL_FDS[i].fd)+" is disconnected", 0);
					}
					// LOG_INFO("The status of FD_DATA[" + to_string(readable_FD) +"] : CLIENT");
					// //LOG_ERROR("the client with FD : "+to_string(ALL_FDS[i].fd)+" is disconnected", 0);
					// //cleanClientFd(ALL_FDS[i].fd);
					// cleanClientFd(readable_FD);
				}
				//else if (FD_DATA[readable_FD] == CGI) CGI can read and treat the message
				//else//it means it's the stopFD wich recv a signal
				//	stopServer();
				continue;
			}
			if (ALL_FDS[i].revents & POLLOUT) {
				//LOG_INFO("POLLOUT signal");
				int sendable_fd = ALL_FDS[i].fd;
				if (FD_DATA[sendable_fd]->status == CLIENT) {
					if (FD_DATA[sendable_fd]->response && FD_DATA[sendable_fd]->response->getResponseReadyToSend()) {
						LOG_INFO("response ready to be sent");
						FD_DATA[sendable_fd]->response->sendResponse();
						cleanClientFd(sendable_fd);
					}
				}
			// 	else if (FD_DATA[sendable_fd]->status == CGI) {
			// 	response can be send to the client (by response class or CGI?)
			// }
				continue;
			}
			// else if (ALL_FDS[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
            //     int error_fd = ALL_FDS[i].fd;
            //     LOG_INFO("Error or hangup on FD: " + to_string(error_fd));

            // }

		}
    }
}

void	ServerManager::cleanClientFd(int FD) {
	//clean fd in _all_fds; _mapFd_data;
	close(FD);
	std::map<int, Fd_data*>::iterator	it = FD_DATA.find(FD);
	it->second->server->decreaseClientCount();
	delete it->second->request;
	delete it->second;
	FD_DATA.erase(it);
	for (size_t i = 0; i < ALL_FDS.size(); ++i) {
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
		for (std::map<int, Fd_data*>::iterator it = FD_DATA.begin(); it != FD_DATA.end(); ++it) {
			close(it->first);
			if (it->second->status == CLIENT)
				delete it->second->request;
			delete it->second;
		}
		FD_DATA.clear();
	}
	if (ALL_FDS.size() > 0)
		ALL_FDS.clear();
	LOG_INFO("the ServerManager is closed");
}
