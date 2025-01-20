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
#include "DataServer.hpp"

#pragma once

class	Server {
	private:
		std::vector<ConfigServer>	_config;
		std::vector<struct pollfd> _all_fds;//stock all fds of all servers
		int	_number_of_server;//number of door, entries, port where clients can connect
		std::map<int, Request>	_mapFdRequest;
		Server() {}
	public:
	class ServerException : public std::exception {
		private:
		std::string	_message;
		public:
		ServerException(std::string const & message) : _message(message) {}
		virtual ~ServerException() throw() {}
		virtual const char* what() const throw() {return _message.c_str();}
	};
	~Server();
	Server(std::list<ConfigServer> & config);
	void	launchServers() {
		for (int i = 0; i != _config.size(); ++i) {
			addFdToFds(_config[i].createServerFd());
			//ajouter autant de fd que de port pour chaque server
			//verifier que deux servers n'aient pas le ou les memes ports, sinon 1er server servi
		}
		_number_of_server = _all_fds.size();
    while (true) {
        int poll_count = poll(_all_fds.data(), _all_fds.size(), -1);  // Wait indefinitely
        if (poll_count == -1)
            throw ServerException("Poll failed");
		//if new connection on one port of one server
		for (int i = 0; i < _number_of_server; ++i) {
			if (_all_fds[i].revents & POLLIN)
				createNewSocket(_all_fds[i].fd);//créer une class client contenant tous les fd client de ce port
		}
        // Check all clients for incoming data
        for (int i = _number_of_server; i < _all_fds.size(); ++i) {
            if (_all_fds[i].revents & POLLIN && _mapFdRequest[_all_fds[i].fd].parseRequest() == -1) {
				close(_all_fds[i].fd);
				_all_fds.erase(_all_fds.begin() + i);
			}
        }
    }
	}
	void	addServer(ConfigServer config);
	void	addFdToFds(std::vector<int> fd_to_add);
	void	initServerSocket();
	void	createNewSocket(int fd);
	void	manageRequest(int i);
};
