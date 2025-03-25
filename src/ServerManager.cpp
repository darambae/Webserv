#include "../include/ServerManager.hpp"

ServerManager::ServerManager(const std::vector<ConfigServer>& configs) : _configs(configs) {}

void	ServerManager::launchServers() {
	//create one FD by odd of IP/port
	for (size_t i = 0; i < _configs.size(); ++i) {
		//create a server class for each configServer
		_servers.push_back(new Server(_configs[i], _configs[i].getListen()));
	}
	while (stopProgram != 1) {

		int poll_count = poll(ALL_FDS.data(), ALL_FDS.size(), -1);  // Wait indefinitely
		if (poll_count == -1)
			LOG_ERROR("Poll failed", true);
		for (size_t i = 0; i < ALL_FDS.size(); ++i) {
			if (ALL_FDS[i].revents == 0)
				continue;
			else if (ALL_FDS[i].revents & POLLHUP) {
				handlePollhup(ALL_FDS[i].fd);
				continue;
			}
			else if (ALL_FDS[i].revents & POLLIN) {
				if  (FD_DATA[ALL_FDS[i].fd]->response && FD_DATA[ALL_FDS[i].fd]->response->getResponseReadyToSend() && (ALL_FDS[i].revents & POLLOUT)) {
					handlePollout(ALL_FDS[i].fd);
					continue;
				} else {
					handlePollin(ALL_FDS[i].fd);
					continue;						
				}
			}
			else if (ALL_FDS[i].revents & POLLOUT) {
				handlePollout(ALL_FDS[i].fd);
				continue;
			}
			else if (ALL_FDS[i].revents & POLLERR) {
				LOG_ERROR("POLLERR flag, error on socket : " + to_string(ALL_FDS[i].fd), true);
				cleanFd(ALL_FDS[i].fd);
				continue;
			}
			else if (ALL_FDS[i].revents & POLLNVAL) { // Invalid socket
				LOG_ERROR("POLLNVAL flag, socket invalid : " + to_string(ALL_FDS[i].fd), true);
				cleanFd(ALL_FDS[i].fd);
				continue;
			}
		}
	}
}

void	ServerManager::handlePollhup(int FD) {

	if (FD_DATA[FD]->status == CLIENT && FD_DATA[FD]->just_connected) {
		// Ignore POLLHUP for newly connected clients
		FD_DATA[FD]->just_connected = false;
	}
	cleanFd(FD);
}

void	ServerManager::handlePollin(int FD) {

	if (FD_DATA[FD]->status == SERVER) {
		int new_client = FD_DATA[FD]->server->createClientSocket(FD);
		if (new_client == -1) {
			LOG_INFO("The status of FD_DATA[" + to_string(FD) +"] : SERVER");
			cleanFd(new_client);
		} else
			FD_DATA[new_client]->just_connected = true;
	} else if (FD_DATA[FD]->status == CLIENT) {
		if (FD_DATA[FD]->request == NULL) {
			FD_DATA[FD]->request = new Request(FD);
		}
		if (FD_DATA[FD]->request->handleRequest() == -1) {
			LOG_INFO("The status of FD_DATA[" + to_string(FD) +"] : CLIENT");
			cleanFd(FD);
		}
	} else if (FD_DATA[FD]->status == CGI_parent) { //read from CGI
		if (FD_DATA[FD]->CGI->getStatus() > 0) {
			closeCgi(500, FD_DATA[FD]->request->getClientFD());
			return;
		}
		if (get_time() - FD_DATA[FD]->request->getTimeStamp() > TIME_OUT) {
			closeCgi(408, FD_DATA[FD]->request->getClientFD());
		}
		else {
			if (FD_DATA[FD]->CGI->recvFromCgi() == -1)
				closeCgi(501, FD_DATA[FD]->request->getClientFD());
			else if (FD_DATA[FD]->response->getResponseReadyToSend())//children finish and send the response to the response class
				closeCgi(0, FD_DATA[FD]->request->getClientFD());
		}
	}
}

void	ServerManager::handlePollout(int FD) {

	if (FD_DATA[FD]->status == CLIENT) {
		if (FD_DATA[FD]->CGI && FD_DATA[FD]->CGI->getStatus() > 0) { //if cgi program failed
			closeCgi(500, FD_DATA[FD]->request->getClientFD());
			return;
		}
		if (FD_DATA[FD]->request && (get_time() - FD_DATA[FD]->request->getTimeStamp() > TIME_OUT)) {
			//LOG_INFO("This request is timeout");
			if (FD_DATA[FD]->response) {
				FD_DATA[FD]->response->setResponseStatus(408);
				FD_DATA[FD]->response->handleError();
				FD_DATA[FD]->response->sendResponse();
				cleanFd(FD);
				return;
			}
		}
		if (FD_DATA[FD]->response && FD_DATA[FD]->response->getResponseReadyToSend()) {
			if (FD_DATA[FD]->response->sendResponse() == -1) {
				cleanFd(FD);
				return ;
			}
			if (FD_DATA[FD]->response->getResponseReadyToSend() == false) {
				if (FD_DATA[FD]->CGI != NULL)
					closeCgi(0, FD);
				if (FD_DATA[FD]->response) {
					delete FD_DATA[FD]->response;
					FD_DATA[FD]->response = NULL;
				}
				if (FD_DATA[FD]->request) {
					delete FD_DATA[FD]->request;
					FD_DATA[FD]->request = NULL;
				}
			}
		}
	}
	else if (FD_DATA[FD]->status == CGI_children) {
		if (get_time() - FD_DATA[FD]->request->getTimeStamp() > TIME_OUT) {
			LOG_INFO("This CGI children is timeout");
			closeCgi(408, FD_DATA[FD]->request->getClientFD());
			return;
		}
		if (FD_DATA[FD]->request->getMethod() == "GET")
			return;
		else {
			int bodysend = FD_DATA[FD]->CGI->sendToCgi();
			if (bodysend == -1) {
				LOG_ERROR("Body sent to CGI failed", true);
				closeCgi(501, FD_DATA[FD]->request->getClientFD());
			}
		}
	}
}

void	ServerManager::closeCgi(int errorNumber, int FdClient) {
	if (errorNumber > 0) {
		FD_DATA[FdClient]->response->setResponseStatus(errorNumber);
		FD_DATA[FdClient]->response->handleError();
	}
	pid_t childPid = FD_DATA[FdClient]->CGI->getPid();
	kill(childPid, SIGKILL);
	usleep(100000);
	int status;
    while (waitpid(childPid, &status, 0) > 0) {};
	int FD_children = FD_DATA[FdClient]->CGI->getSocketsChildren();
	if (FD_DATA[FdClient]->request->getMethod() == "POST")
		cleanFd(FD_children);
	else
		close(FD_children);
	cleanFd(FD_DATA[FdClient]->CGI->getSocketsParent());
	delete FD_DATA[FdClient]->CGI;
	FD_DATA[FdClient]->CGI = NULL;
	LOG_INFO("CGI is close");
	// if (errorNumber > 0)
	// 	cleanFd(FdClient);
}

//clean fd in _all_fds; _mapFd_data;
void	ServerManager::cleanFd(int FD) {
	if (FD != -1)
		close(FD);
	if (FD_DATA[FD]->status == CLIENT) {
		FD_DATA[FD]->server->decreaseClientCount();
		if (FD_DATA[FD]->request != NULL)
			delete FD_DATA[FD]->request;
		if (FD_DATA[FD]->response)
			delete FD_DATA[FD]->response;
		if (FD_DATA[FD]->CGI)
			closeCgi(0, FD);
	}
	std::map<int, Fd_data*>::iterator	it = FD_DATA.find(FD);
	delete it->second;
	FD_DATA.erase(it);
	for (size_t i = 0; i < ALL_FDS.size(); ++i) {
		if (ALL_FDS[i].fd == FD) {
			ALL_FDS.erase(ALL_FDS.begin() + i);
			break;
		}
	}
	LOG_INFO("The FD : "+to_string(FD)+" was cleaned");
}


ServerManager::~ServerManager() {
	for (size_t i = 0; i < _servers.size(); ++i)
	    delete _servers[i];
	if (FD_DATA.size() > 0) {
		for (std::map<int, Fd_data*>::iterator it = FD_DATA.begin(); it != FD_DATA.end(); ++it) {
			close(it->first);
			if (it->second->request) {
				delete it->second->request;
				it->second->request = NULL;
			}
			if (it->second->response) {
				delete it->second->response;
				it->second->response = NULL;
			}
			if (it->second->CGI) {
				closeCgi(0, it->first);
			}
			delete it->second;
		}
		FD_DATA.clear();
	}
	if (ALL_FDS.size() > 0)
		ALL_FDS.clear();
	LOG_INFO("the ServerManager is closed");
}
