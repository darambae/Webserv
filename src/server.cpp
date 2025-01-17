#include <iostream>
#include <cstring> //memset...
#include <cstdlib> //exit ...
#include <sys/socket.h> // socket, bind, accept...
#include <netinet/in.h> // sockaddr_in
#include <unistd.h> //close...
#include <poll.h>
#include "../include/Server.hpp"

// #define PORT 8080 //common port for http servers( under 1024 is reserved for system services)
// #define BUFFER_SIZE 1024
// #define MAX_CLIENTS 10

// void to_uppercase(char *str)
// {
//     while (*str) {
//         *str = toupper(*str);
//         ++str;
//     }
// }

Server::Server(ConfigServer const & config) : _config(config) {
	_server_fd, _new_socket, _client_count = 0;
	_len_address = sizeof(_address);
	_max_clients = 1024;//by default but max is defined by system parameters(bash = ulimit -n)
	_bufferSize = _config.getLimitClientBodySize();
	_buffer.reserve(_bufferSize);//reserve a memory place

	initServerSocket();
    while (true) {
        int poll_count = poll(_fds.data(), _fds.size(), -1);  // Wait indefinitely
        if (poll_count == -1)
            ServerException("Poll failed");
        if (_fds[0].revents & POLLIN)//means new connection to port 8080
			createNewSocket();
        // Check all clients for incoming data
        for (int i = 1; i <= _fds.size(); ++i) {
            if (_fds[i].revents & POLLIN)//a socket client receive a message
				manageRequest(i);
        }
    }
}

Server::~Server() {
	for (int i = _fds.size(); i > 0; ++i) {
		close(_fds[i].fd);
		_fds.erase(_fds.begin() + i);
	}
}

void	Server::addFdToFds(int fd_to_add) {
	if (_fds.size() >= _max_clients) {
        std::cout << "Max clients reached, rejecting connection\n";
        close(fd_to_add);
		return;
	}
	struct pollfd new_socket;
    new_socket.fd = fd_to_add;
    new_socket.events = POLLIN | POLLOUT;  // to check write and read in a same time (subject order)
    _fds.push_back(new_socket);
	_client_count++;
}

void	Server::initServerSocket() {
		_server_fd = socket(AF_INET, SOCK_STREAM, 0); // SOCK_STREAM : TCP socket
    if (_server_fd == -1)
        throw ServerException("Socket creation failed");
    std::cout << "Server socket created on sd " << _server_fd << std::endl;
/*in case of server crach, this setting allow to reuse the port */
	int opt = 1;
    if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) { //Checking if the socket is already in use
        close(_server_fd);
		throw ServerException("setsockopt failed");
	}
/*init server socket*/
	_address.sin_family = AF_INET; //IPv4
    _address.sin_addr.s_addr = INADDR_ANY; //Tells the server to bind to all available network interfaces (not just a specific IP)
    _address.sin_port = htons(_config.getPort()); //Converts the port number to "network byte order"
    if (bind(_server_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0){
        //std::cerr << "Bind failed" << std::endl; // When bind fails, on terminal "sudo lsof -i :8080" & "sudo kill 8080" can be used to free the port.
        close(_server_fd);
		throw ServerException("Bind failed");
    }
    if (listen(_server_fd, 10) < 0){//make serverfd listening new connections, 10 connections max can wait to be accepted
        close(_server_fd);
        throw ServerException("Listen failed");
    }
    std::cout << "Server is listening on port " << _config.getPort() << std::endl;
	addFdToFds(_server_fd);
	//maybe not necessary if we use vector
	//for (int i = 1; i <= _max_clients; ++i) {
    //     fds[i].fd = -1;  // Set initial state as no client connected
    // }
}

void	Server::createNewSocket() {
	//if we want to save data from each client, create a sockaddr_in for each
	int new_socket = accept(_server_fd, (struct sockaddr *)&_address, (socklen_t *)&_len_address);
    if (new_socket < 0) {
        perror("Accept failed");
        return;
    }
    std::cout << "New client connected : " << "client socket(" << new_socket << ")" << std::endl;
    addFdToFds(new_socket);
}

void	Server::manageRequest(int i) {
	std::fill(_buffer.begin(), _buffer.end(), '\0');
    int bytes_received = recv(_fds[i].fd, const_cast<char *>(_buffer.c_str()), _bufferSize, 0);
    if (bytes_received == 0) {//client closed properly the connection
        std::cout << "Client disconnected\n";
        close(_fds[i].fd);
        _fds.erase(_fds.begin() + i);
        _client_count--;
        } else if (bytes_received < 0) {
            perror("Recv failed");
        } else {
			const char *http_response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: 38\r\n"
			"\r\n"
			"<html>\n"
			"    <h1>Hello, World!</h1>\n"
			"</html>";
            std::cout << "Client's Received: " << _buffer << std::endl;
            // Convert message to uppercase and send it back
            std::transform(_buffer.begin(), _buffer.end(), _buffer.begin(), ::toupper);
            //send(fds[i].fd, buffer, strlen(buffer), 0);
            send(_fds[i].fd, http_response, strlen(http_response), 0);
            std::cout << "HTTP response sent to client " << _fds[i].fd << std::endl;
            //std::cout << "Echoed back: " << buffer << "\n";
        }
}
/*
And this is the important bit: a pointer to a struct sockaddr_in can be cast to a pointer
to a struct sockaddr and vice-versa. So even though connect() wants a struct sockaddr*, you
 can still use a struct sockaddr_in and cast it at the last minute!
// (IPv4 only--see struct sockaddr_in6 for IPv6)
struct sockaddr_in {
    short int          sin_family;  // Address family, AF_INET
    unsigned short int sin_port;    // Port number
    struct in_addr     sin_addr;    // Internet address
    unsigned char      sin_zero[8]; // Same size as struct sockaddr
};
This structure makes it easy to reference elements of the socket address. Note that sin_zero
(which is included to pad the structure to the length of a struct sockaddr) should be set to
all zeros with the function memset(). Also, notice that sin_family corresponds to sa_family
in a struct sockaddr and should be set to “AF_INET”. Finally, the sin_port must be in Network
 Byte Order (by using htons()!)

******poll() :****
	#include <poll.h>
	int poll(struct pollfd *fds, nfds_t nfds, int timeout);
->Used to check sockets, to know if we can write or read on or if there is error
	- Socket (or fd) are stocked in struct pollfd *fds:
 		struct pollfd {
    		int   fd;       // Descripteur de fichier (socket, fichier, etc.)
    		short events;   // Événements à surveiller (POLLIN, POLLOUT, etc.)
    		short revents;  // Événements détectés
		};
		fd = socket;
		events = events to check if it occurs;
			- POLLIN = data readable on this fd;
			- POLLOUT = fd writable;
			- POLLERR = error on the socket;
			- POLLHUP = client disconnected;
		revents = events detected after poll();
	- nfds is the number of elements in fds
	- timeout = how many time we wait in ms until an event occurs :
		0 = without wait;
		-1 = infinite wait until event;
		> 0 = wait .. ms before to return;
	- int return is the number of events wich occur
	!!!to check if revents contain POLLIN we cannot do :
	X if (revents == POLLIN)//because POLLIN correspond to a byte in revents
	and revents can contain the byte of POLLIN and the byte of POLLOUT
	so to check if revents contain one of them we do :
	-> if (fds[0].revents & POLLIN)//& = operator bit to bit => 0001 & 0001 == true
	-> if (fds[0].revents & POLLOUT)
	-> if (fds[0].renvents & (POLLIN | POLLOUT))//revents contains POLLIN or POLLOUT
	-> if (fds[0].revents & POLLIN && fds[0].revents & POLLOUT)// revents contains both
*/

