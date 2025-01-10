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

int main(){
    int server_fd, new_socket, client_count = 0; //new_socket: fd for the accepted client connection
    struct sockaddr_in address; //sockaddr_in is more specific structure than 'sockaddr'
    int len_address = sizeof(address);
    char buffer[BUFFER_SIZE];

    struct pollfd fds[MAX_CLIENTS + 1]; //+1 for the server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0); // SOCK_STREAM : TCP socket
    if (server_fd == -1){
        std::cerr << "Socket creation failed" << std::endl;
        return 1;
    }
    std::cout << "Server socket created on sd " << server_fd << std::endl;
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) { //Checking if the socket is already in use
        perror("setsockopt failed");
        close(server_fd);
        return 1;
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
*/
    address.sin_family = AF_INET; //IPv4
    address.sin_addr.s_addr = INADDR_ANY; //Tells the server to bind to all available network interfaces (not just a specific IP)
    address.sin_port = htons(PORT); //Converts the port number to "network byte order"
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0){  
        //std::cerr << "Bind failed" << std::endl; // When bind fails, on terminal "sudo lsof -i :8080" & "sudo kill 8080" can be used to free the port.
        perror("Bind failed");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 10) < 0){ 
        /*
            num : Specifies the "backlog," the maximum number of queued connections waiting to be accepted. 
            Here, it expects 5 messages to be received.
        */ 
        std::cerr << "Listen failed" << std::endl;
        close(server_fd);
        return 1;
    }
    std::cout << "Server is listening on port " << PORT << std::endl;
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;  // Monitor for incoming connections
    for (int i = 1; i <= MAX_CLIENTS; ++i) {
        fds[i].fd = -1;  // Set initial state as no client connected
    }
    while (true) {
        int poll_count = poll(fds, client_count + 1, -1);  // Wait indefinitely

        if (poll_count == -1) {
            perror("Poll failed");
            break;
        }

        // Check the server socket (index 0) for new incoming connections
        if (fds[0].revents & POLLIN) {
            new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&len_address);
            if (new_socket < 0) {
                perror("Accept failed");
                continue;
            }
            std::cout << "New client connected : " << "client socket(" << new_socket << ")" << std::endl;

            // Add the new client socket to the fds array
            bool added = false;
            for (int i = 1; i <= MAX_CLIENTS; ++i) {
                if (fds[i].fd == -1) {
                    fds[i].fd = new_socket;
                    fds[i].events = POLLIN;
                    client_count++;
                    added = true;
                    break;
                }
            }
            if (!added) {
                std::cout << "Max clients reached, rejecting connection\n";
                close(new_socket);
            }
        }
        const char *http_response =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Content-Length: 38\r\n"
    "\r\n"
    "<html>\n"
    "    <h1>Hello, World!</h1>\n"
    "</html>";
;
        // Check all clients for incoming data
        for (int i = 1; i <= MAX_CLIENTS; ++i) {
            if (fds[i].fd != -1 && (fds[i].revents & POLLIN)) {
                memset(buffer, 0, BUFFER_SIZE);
                int bytes_received = recv(fds[i].fd, buffer, BUFFER_SIZE, 0); // Using recv instead of read
                if (bytes_received == 0) {
                    std::cout << "Client disconnected\n";
                    close(fds[i].fd);
                    fds[i].fd = -1;
                    client_count--;
                } else if (bytes_received < 0) {
                    perror("Recv failed");
                } else {
                    std::cout << "Client's Received: " << buffer << std::endl;

                    // Convert message to uppercase and send it back
                    to_uppercase(buffer);
                    //send(fds[i].fd, buffer, strlen(buffer), 0);
                    send(fds[i].fd, http_response, strlen(http_response), 0);
                    std::cout << "HTTP response sent to client " << fds[i].fd << std::endl;
                    //std::cout << "Echoed back: " << buffer << "\n";
                }
            }
        }
    }
    for (int i = 0; i <= MAX_CLIENTS; ++i) {
        if (fds[i].fd != -1) {
            close(fds[i].fd);
        }
    }
    close(server_fd);
    return 0;
}
