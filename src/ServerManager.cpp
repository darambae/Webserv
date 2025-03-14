
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
			LOG_ERROR("Poll failed", true);
		//if new connection on one port of one server
		//print_all_FD_DATA();
        for (size_t i = 0; i < ALL_FDS.size(); ++i) {
            if (ALL_FDS[i].revents == 0)
                continue;
            else if (ALL_FDS[i].revents & POLLHUP) {
                handlePollhup(ALL_FDS[i].fd);
                continue;
            }
            else if (ALL_FDS[i].revents & POLLIN) {
                handlePollin(ALL_FDS[i].fd);
                continue;
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
	print_FD_status(FD);
	LOG_INFO("POLLHUP signal");
	if (FD_DATA[FD]->status == CLIENT && FD_DATA[FD]->just_connected) {
		// Ignore POLLHUP for newly connected clients
    	FD_DATA[FD]->just_connected = false;
    }

	//LOG_ERROR("the client with FD : "+to_string(ALL_FDS[i].fd)+" is disconnected", 0);
	cleanFd(FD);
}

void	ServerManager::handlePollin(int FD) {
	//print_FD_status(FD);
	//LOG_INFO("POLLIN signal");
	if (FD_DATA[FD]->status == SERVER) {
		int new_client = FD_DATA[FD]->server->createClientSocket(FD);
		if (new_client == -1) {
			LOG_INFO("The status of FD_DATA[" + to_string(FD) +"] : SERVER");
			cleanFd(new_client);
			//LOG_ERROR(": "+to_string(new_client)+" is disconnected", 0);
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
	//print_FD_status(FD);
	if (FD_DATA[FD]->status == CLIENT) {
		if (FD_DATA[FD]->CGI && FD_DATA[FD]->CGI->getStatus() > 0) { //if cgi program failed
			closeCgi(500, FD_DATA[FD]->request->getClientFD());
			return;
		}
		//LOG_INFO("The start time of this request from Client " + to_string(FD) + ": " + to_string(FD_DATA[FD]->request->getTimeStamp()));
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
			//print_FD_status(FD);
			// LOG_INFO("POLLOUT signal");
			// LOG_INFO("response ready to be sent");
			if (FD_DATA[FD]->response->sendResponse() == -1) {
				//LOG_ERROR("client FD "+to_string(FD)+" disconected for response error", 0);
				cleanFd(FD);
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
				//LOG_INFO("response send and delete");
			}
		}
	}
	else if (FD_DATA[FD]->status == CGI_children) {
		//LOG_INFO("Child process with Pollout");
		if (get_time() - FD_DATA[FD]->request->getTimeStamp() > TIME_OUT) {
			LOG_INFO("This CGI children is timeout");
			closeCgi(408, FD_DATA[FD]->request->getClientFD());
			return;
		}
		if (FD_DATA[FD]->request->getMethod() == "GET")
			return;
		else {
			//print_FD_status(FD);
			//LOG_INFO("POLLOUT signal");
			int bodysend = FD_DATA[FD]->CGI->sendToCgi();
			if (bodysend == -1) {
				//LOG_ERROR("write to send the body to CGI failed", true);
				closeCgi(501, FD_DATA[FD]->request->getClientFD());
			}
			// else if (bodysend == 0)
			// 	cleanFd(FD);
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
	if (errorNumber > 0)
		cleanFd(FdClient);
}

void	ServerManager::print_all_FD_DATA() {
	for (size_t i = 0; i < ALL_FDS.size(); ++i) {
		std::cout<<"Datas of FD "<<ALL_FDS[i].fd<<" are :\nIP : "<<FD_DATA[ALL_FDS[i].fd]->ip\
		<<"\nPORT : "<<FD_DATA[ALL_FDS[i].fd]->port\
		<<"\n status: "<<FD_DATA[ALL_FDS[i].fd]->status<<std::endl;
	}
}

void	ServerManager::print_FD_status(int FD) {
	std::string	status = "";
	if (FD_DATA[FD]->status == CLIENT)
		status = "CLIENT";
	else if (FD_DATA[FD]->status == SERVER)
		status = "SERVER";
	else if (FD_DATA[FD]->status == CGI_parent)
		status = "CGI_PARENT";
	else if (FD_DATA[FD]->status == CGI_children)
		status = "CGI_CHILDREN";
	std::cout<<"The FD "<<FD<<" with the status "<<status<<std::endl;
}

void	ServerManager::cleanFd(int FD) {
	//clean fd in _all_fds; _mapFd_data;
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
