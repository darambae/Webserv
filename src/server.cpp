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
/*i comment this step because in case of double use of one IP with one port, socket will launch an error*/
	// std::map<int, Fd_data*>::iterator	it;
	// if (FD_DATA.size() > 0) {
	// 	for (it = FD_DATA.begin(); it != FD_DATA.end(); ++it) {
	// 		if (it->second->port == ipPort.second && it->second->ip == ipPort.first)
	// 			return;
	// 	}
	// }
	/*create the FD*/
	std::cout<<"initiation socket of IP "<<ipPort.first<<" and port "<<ipPort.second<<" begin..."<<std::endl;
	int	new_fd = socket(AF_INET, SOCK_STREAM, 0); // SOCK_STREAM : TCP socket
	if (new_fd == -1) {
		//std::cout<<"socket function failed to create a new fd for the IP "<<ipPort.first<<" and the port "<<ipPort.second<<std::endl;
		LOG_ERROR("socket function failed to create a new fd for the IP "+ipPort.first+" and the port "+to_string(ipPort.second), true);
		//perror("error message : ");
		return;
	}
	std::cout<<"a new server socket was created : "<<new_fd<<std::endl;
	/*in case of server crach, this setting allow to reuse the port */
	int opt = 1;
	if (setsockopt(new_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		LOG_ERROR("setsockopt failed", true);

		//perror("setsockopt failed");
		return;
	}
	/*init server socket*/
	_address.sin_family = AF_INET; //IPv4
	if (inet_pton(AF_INET, ipPort.first.c_str(), &_address.sin_addr) <= 0)
    	return LOG_ERROR("Erreur : adresse IP invalide ou conversion échouée\n", 1);
	_address.sin_port = htons(ipPort.second); //Converts the port number to "network byte order"
	if (bind(new_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0) {
	//std::cerr << "Bind failed" << std::endl; // When bind fails, on terminal "sudo lsof -i :8080" & "sudo kill 8080" can be used to free the port.
		// std::cout<<"the bind of the new socket "<<new_fd<<" for the IP "<<ipPort.first<<" and the port "<<ipPort.second<<" failed"<<std::endl;
		// perror("");
		LOG_ERROR("The bind of the new socket "+to_string(new_fd)+" for the IP "+ipPort.first+" and the port "+to_string(ipPort.second)+" failed", true);
		return;
		//throw ServerException("Bind failed");
	}
	std::cout<<"The socket is bind"<<std::endl;
	if (listen(new_fd, 10) < 0)//make serverfd listening new connections, 10 connections max can wait to be accepted
	    LOG_ERROR("Listen failed", true);
	//std::cout << "a new socket was created for " << _config.getServerNames()[0] <<" and port " << ipPort.second << " on FD " << new_fd << std::endl;
	LOG_INFO("A new socket was created for " + _config->getServerNames()[0] + " and port " + to_string(ipPort.second) + " on FD " + to_string(new_fd));
	if (unblockFD(new_fd) == -1)
		return;
	addFdToFds(new_fd);
	addFdData(new_fd, ipPort.first, ipPort.second, this, SERVER, false);
}

void	Server::addFdData(int fd, std::string ip,int port, Server* server, fd_status status, bool request) {
	FD_DATA[fd] = new Fd_data;
	FD_DATA[fd]->ip = ip;
	FD_DATA[fd]->port = port;
	FD_DATA[fd]->server = server;
	FD_DATA[fd]->status = status;
	FD_DATA[fd]->response = NULL;
	if (request == true)
		FD_DATA[fd]->request = new Request(fd);
	else
		FD_DATA[fd]->request = NULL;
}

void	Server::addFdToFds(int fd_to_add) {
	struct pollfd new_socket;
    new_socket.fd = fd_to_add;
    new_socket.events = POLLIN | POLLOUT;// to check write and read in a same time (subject order)
	//new_socket.revents = 0;
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
    //std::cout << "New client connected : " << "client socket(" << new_socket << ")" << std::endl;
	LOG_INFO("New client connected : " + to_string(new_socket));
	if (unblockFD(new_socket) == -1)
		return -1;
	addFdToFds(new_socket);
	addFdData(new_socket, std::string(inet_ntoa(_address.sin_addr)), _address.sin_port, this, CLIENT, true);
	return new_socket;
}

void	Server::decreaseClientCount() {_client_count--;}

/*this function enables to make fd non blocking, it means that accept/read/write
functions will send -1 instead to wait if there is no connection or if there is
nothing to read or if there is no place to write*/
int	Server::unblockFD(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

Server::~Server() {
	std::cout<<"the server : "<<_config->getServerNames()[0]<<" is closed"<<std::endl;
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

