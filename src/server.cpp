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

void to_uppercase(char *str)
{
    while (*str) {
        *str = toupper(*str);
        ++str;
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
	!!!to check is revents contain POLLIN we cannot do :
	X if (revents == POLLIN)//because POLLIN correspond to a byte in revents
	and revents can contain the byte of POLLIN and the byte of POLLOUT
	so to check if revents contain one of them we do :
	-> if (fds[0].revents & POLLIN)//& = operator bit to bit => 0001 & 0001 == true
	-> if (fds[0].revents & POLLOUT)
	-> if (fds[0].renvents & (POLLIN | POLLOUT))//revents contains POLLIN or POLLOUT
	-> if (fds[0].revents & POLLIN && fds[0].revents & POLLOUT)// revents contains both
*/

