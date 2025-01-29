#pragma once

#include "ConfigServer.hpp"
// #include "Request.hpp"
#include "ServerManager.hpp"
#include "webserv.hpp"

class	Server {
	private:
		const ConfigServer &	_config;
		const std::vector<std::pair<std::string, int> > &	_listen;
		struct sockaddr_in	_address;
		int	_len_address;
		int	_client_count;
	public:
		Server(const ConfigServer & config, const std::vector<std::pair<std::string, int> > & listen);
		~Server();
		class ServerException : public std::exception {
			private:
				std::string	_message;
			public:
				ServerException(std::string const & message) : _message(message) {}
				virtual ~ServerException() throw() {}
				virtual const char* what() const throw() {return _message.c_str();}
		};
		void	initServerSocket(std::pair<std::string, int> ipPort);
		void	addFdData(int fd, std::string ip,int port, Server* server, fd_status status/*, bool request*/);
		void	addFdToFds(int fd_to_add);
		int	createClientSocket(int fd);
		void	decreaseClientCount();
};
