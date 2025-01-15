#include <iostream>
#include <cstring> //memset...
#include <cstdlib> //exit ...
#include <sys/socket.h> // socket, bind, accept...
#include <netinet/in.h> // sockaddr_in
#include <unistd.h> //close...
#include <poll.h>

#define PORT 8080 //common port for http servers( under 1024 is reserved for system services)
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

#pragma once

class	Server {
	private:
		struct sockaddr_in	_serveraddr;

	public:
	Server() {
		int server_fd, new_socket, client_count = 0; //new_socket: fd for the accepted client connection
		struct sockaddr_in address; //sockaddr_in is more specific structure than 'sockaddr'
		int len_address = sizeof(address);
		char buffer[BUFFER_SIZE];
	}
	int	init_serversock();
};
