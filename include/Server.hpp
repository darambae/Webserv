#include <iostream>
#include <cstring> //memset...
#include <cstdlib> //exit ...
#include <sys/socket.h> // socket, bind, accept...
#include <netinet/in.h> // sockaddr_in
#include <unistd.h> //close...
#include <poll.h>
#include "ConfigServer.hpp"


#pragma once

class	Server {
	private:
		struct sockaddr_in	_address;//sockaddr_in is more specific structure than 'sockaddr'
		int _server_fd, _new_socket, _client_count, _len_address, _max_clients;//new_socket: fd for the accepted client connection
		ConfigServer	_config;
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
	~Server() {}
	Server(ConfigServer const & config) : _config(config) {
		_server_fd, _new_socket, _client_count = 0;
		_len_address = sizeof(_address);
		_max_clients = 1024;//by default but max is defined by system parameters(bash = ulimit -n)
		char buffer[_config.getLimitClientBodySize()];

		struct pollfd fds[_max_clients + 1]; //+1 for the server socket == client_count ?
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

    if (listen(_server_fd, 10) < 0){
        /*
            num : Specifies the "backlog," the maximum number of queued connections waiting to be accepted.
            Here, it expects 5 messages to be received.
        */
        close(_server_fd);
        throw ServerException("Listen failed");
    }
    std::cout << "Server is listening on port " << config.getPort() << std::endl;
    fds[0].fd = _server_fd;
    fds[0].events = POLLIN;  // Monitor for incoming connections
    for (int i = 1; i <= _max_clients; ++i) {
        fds[i].fd = -1;  // Set initial state as no client connected
    }
    while (true) {
        int poll_count = poll(fds, _client_count + 1, -1);  // Wait indefinitely

        if (poll_count == -1) {
            perror("Poll failed");
            break;
        }

        // Check the server socket (index 0) for new incoming connections
        if (fds[0].revents & POLLIN) {
            new_socket = accept(server_fd, (struct sockaddr *)&_address, (socklen_t *)&len_address);
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
};
