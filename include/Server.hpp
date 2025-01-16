#include <iostream>
#include <cstring> //memset...
#include <cstdlib> //exit ...
#include <sys/socket.h> // socket, bind, accept...
#include <netinet/in.h> // sockaddr_in
#include <unistd.h> //close...
#include <poll.h>
#include <vector>
#include "configServer.hpp"


#pragma once

class	Server {
	private:
		struct sockaddr_in	_address;//sockaddr_in is more specific structure than 'sockaddr'
		int _server_fd, _new_socket, _client_count,_bufferSize, _len_address, _max_clients;//new_socket: fd for the accepted client connection
		configServer	_config;
		std::string		_buffer;
		std::vector<struct pollfd> _fds;
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
	Server(configServer const & config);
	void	addFdToFds(int fd_to_add);
	void	initServerSocket();
	void	createNewSocket();
	void	manageRequest(int i);
};
