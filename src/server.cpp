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

/*TO DO :
• Votre serveur doit pouvoir écouter sur plusieurs ports (cf. Fichier de configuration).
• Il doit être non bloquant et n’utiliser qu’un seul poll() (ou équivalent) pour
toutes les opérations entrées/sorties entre le client et le serveur (listen inclus).

*/

Server::Server(const ConfigServer & config, const std::vector<std::pair<std::string, int> > &	listen) : _config(config), _listen(listen) {
	_len_address = sizeof(_address);
	_client_count = 0;
	for (int i = 0; i < _listen.size(); ++i) {
		initServerSocket(_listen[i]);//create one FD by port, bind it and make it listening
	}
}

void	Server::initServerSocket(std::pair<std::string, int> ipPort) {
	std::map<int, t_Fd_data*>::iterator	it;
	if (FD_DATA.size() > 0) {
		for (it = FD_DATA.begin(); it != FD_DATA.end(); ++it) {
			if (it->second->port == ipPort.second)
				return;
		}
	}
	/*create the FD*/
	int	new_fd = socket(AF_INET, SOCK_STREAM, 0); // SOCK_STREAM : TCP socket
	if (new_fd == -1)
		throw ServerException("Socket creation failed");
	std::cout<<"a new server socket was created : "<<new_fd<<std::endl;
	/*in case of server crach, this setting allow to reuse the port */
	int opt = 1;
	if (setsockopt(new_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) { //Checking if the socket is already in use
		throw ServerException("setsockopt failed");
	}
	/*init server socket*/
	_address.sin_family = AF_INET; //IPv4
	if (inet_pton(AF_INET, ipPort.first.c_str(), &_address.sin_addr) <= 0)
    	throw ServerException("Erreur : adresse IP invalide ou conversion échouée\n");
	_address.sin_port = htons(ipPort.second); //Converts the port number to "network byte order"
	if (bind(new_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0) {
	//std::cerr << "Bind failed" << std::endl; // When bind fails, on terminal "sudo lsof -i :8080" & "sudo kill 8080" can be used to free the port.
		std::cout<<"the bind of the new socket "<<new_fd<<" for the IP "<<ipPort.first<<" and the port "<<ipPort.second<<" failed"<<std::endl;
		perror("");
		return;
		//throw ServerException("Bind failed");
	}
	std::cout<<"the socket is bind"<<std::endl;
	if (listen(new_fd, 10) < 0)//make serverfd listening new connections, 10 connections max can wait to be accepted
	    throw ServerException("Listen failed");
	std::cout << "a new socket was created for " << _config.getServerNames()[0] <<" and port " << ipPort.second << " on FD " << new_fd << std::endl;
	addFdToFds(new_fd);
	addFdData(new_fd, ipPort.first, ipPort.second, this, SERVER/*, false*/);
}

void	Server::addFdData(int fd, std::string ip,int port, Server* server, fd_status status/*, bool request*/) {
	t_Fd_data*	new_fd_data = new t_Fd_data;
	new_fd_data->ip = ip;
	new_fd_data->port = port;
	new_fd_data->server = server;
	new_fd_data->status = status;
	// if (request == true){
	// 	Request*	request = new Request(fd);
	// 	new_fd_data->request = request;
	// }
	// else
	// 	new_fd_data->request = NULL;
	FD_DATA[fd] = new_fd_data;
}

void	Server::addFdToFds(int fd_to_add) {
	struct pollfd new_socket;
    new_socket.fd = fd_to_add;
    new_socket.events = POLLIN | POLLOUT;// to check write and read in a same time (subject order)
    ALL_FDS.push_back(new_socket);
}

int	Server::createClientSocket(int fd) {
    //if we want to save data from each client, create a sockaddr_in for each
	if (_client_count >= MAX_CLIENT) {
        std::cout << "Max clients reached, rejecting connection\n";
		return -1;
	}
	int new_socket = accept(fd, (struct sockaddr *)&_address, (socklen_t *)&_len_address);
    if (new_socket < 0) {
        perror("Accept failed");
        return -1;
    }
	_client_count++;
    std::cout << "New client connected : " << "client socket(" << new_socket << ")" << std::endl;
	addFdToFds(new_socket);
	addFdData(new_socket, std::string(inet_ntoa(_address.sin_addr)), _address.sin_port, this, CLIENT/*, true*/);
	return new_socket;
}

void	Server::decreaseClientCount() {_client_count--;}

Server::~Server() {
	std::cout<<"the server : "<<_config.getServerNames()[0]<<" is closed"<<std::endl;
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

