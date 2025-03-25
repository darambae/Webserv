#include "../include/Server.hpp"


Server::Server(ConfigServer & config, const std::vector<std::pair<std::string, int> > &	listen) : _config(&config), _listen(listen) {
	_client_count = 0;
	for (size_t i = 0; i < _listen.size(); ++i) {
			memset(&_address, 0, sizeof(_address));
			_len_address = sizeof(_address);
			initServerSocket(_listen[i]);//create one FD by port, bind it and make it listening
	}
}

/*one socket == one IP specific and one port.
one port can be associate with many socket if each have a different IP
but if one of them is 0.0.0.0 an error will appear for the creation of the second socket
wich try to use a IP allready used on a port*/
void	Server::initServerSocket(std::pair<std::string, int> ipPort) {
	/*create the FD*/
	std::cout<<"initiation socket of IP "<<ipPort.first<<" and port "<<ipPort.second<<" begin..."<<std::endl;
	int	new_fd = socket(AF_INET, SOCK_STREAM, 0); // SOCK_STREAM : TCP socket
	if (new_fd == -1) {
		LOG_ERROR("socket function failed to create a new fd for the IP "+ipPort.first+" and the port "+to_string(ipPort.second), true);
		return;
	}
	std::cout<<"a new server socket was created : "<<new_fd<<std::endl;
	/*in case of server crach, this setting allow to reuse the port */
	int opt = 1;
	if (setsockopt(new_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		LOG_ERROR("setsockopt failed", true);
		close(new_fd);
		return;
	}
	/*init server socket*/
	_address.sin_family = AF_INET; //IPv4
	if (inet_pton(AF_INET, ipPort.first.c_str(), &_address.sin_addr) <= 0)
    	return LOG_ERROR("Erreur : adresse IP invalide ou conversion échouée\n", 1);
	_address.sin_port = htons(ipPort.second); //Converts the port number to "network byte order"
	if (bind(new_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0) {
		LOG_ERROR("The bind of the new socket "+to_string(new_fd)+" for the IP "+ipPort.first+" and the port "+to_string(ipPort.second)+" failed", true);
		return;
	}
	std::cout<<"The socket is bind"<<std::endl;
	if (listen(new_fd, 10) < 0)//make serverfd listening new connections, 10 connections max can wait to be accepted
	    LOG_ERROR("Listen failed", true);
	if (unblockFD(new_fd) == -1)
		return;
	addFdToFds(new_fd);
	addFdData(new_fd, ipPort.first, ipPort.second, this, SERVER, NULL, NULL, NULL);
}

void	Server::addFdData(int fd, std::string ip,int port, Server* server, fd_status status, Request* request, Response* response, CgiManager* cgi) {
	FD_DATA[fd] = new Fd_data;
	FD_DATA[fd]->ip = ip;
	FD_DATA[fd]->port = port;
	FD_DATA[fd]->server = server;
	FD_DATA[fd]->status = status;
	FD_DATA[fd]->response = NULL;
	if (request)
		FD_DATA[fd]->request = request;
	else
		FD_DATA[fd]->request = NULL;
	if (response)
		FD_DATA[fd]->response = response;
	else
		FD_DATA[fd]->response = NULL;
	if (cgi)
		FD_DATA[fd]->CGI = cgi;
	else
		FD_DATA[fd]->CGI = NULL;
}

void	Server::addFdToFds(int fd_to_add) {
	struct pollfd new_socket;
    new_socket.fd = fd_to_add;
    new_socket.events = POLLIN | POLLOUT;// to check write and read in a same time (subject order)
	new_socket.revents = 0;
	ALL_FDS.push_back(new_socket);
}

int	Server::createClientSocket(int fd) {
    //if we want to save data from each client, create a sockaddr_in for each
	if (_client_count >= MAX_CLIENT) {
        std::cout << "Max clients reached, rejecting connection\n";
		return -1;
	}
	memset(&_address, 0, sizeof(_address));
	_len_address = sizeof(_address);
	int new_socket = accept(fd, (struct sockaddr *)&_address, (socklen_t *)&_len_address);
    if (new_socket < 0) {
        perror("Accept failed");
        return -1;
    }
	_client_count++;
	LOG_INFO("New client connected : " + to_string(new_socket));
	if (unblockFD(new_socket) == -1)
		return -1;
	addFdToFds(new_socket);
	addFdData(new_socket, std::string(inet_ntoa(_address.sin_addr)), _address.sin_port, this, CLIENT, NULL, NULL, NULL);
	return new_socket;
}

void	Server::decreaseClientCount() {_client_count--;}

/*this function enables to make fd non blocking, it means that accept/read/write
functions will send -1 instead to wait if there is no connection or if there is
nothing to read or if there is no place to write*/
int	Server::unblockFD(int fd) {
    return fcntl(fd, F_SETFL, FD_CLOEXEC | O_NONBLOCK);
}

Server::~Server() {
	if (!_config->getServerNames().empty())
		std::cout<<"the server : "<<_config->getServerNames()[0]<<" is closed"<<std::endl;
}
