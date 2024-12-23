#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#define PORT 8080 //common port for http servers
#define BUFFER_SIZE 1024

int main(){
    int sock;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE];
    time_t now;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1){
        std::cerr << "Socket creation on client side failed" << std::endl;
        return 1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK); //INADDR_LOOPBACK : 네트워크에서 자기 자신을 참조하는 주소로, 주로 테스트와 디버깅에 사용

    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0){
        std::cerr << "Connection failed" << std::endl;
        close(sock);
        return 1;
    }

    for (int i = 0; i < 5; ++i) { // Send 5 messages for testing
        std::string message = std::to_string(i + 1) + " : Hello, Server!\n";
        send(sock, message.c_str(), message.length(), 0);
        std::cout << "Client's message to server: " << message << std::endl;

        memset(buffer, 0, BUFFER_SIZE);
        int bytes_read = recv(sock, buffer, BUFFER_SIZE, 0);
        if (bytes_read > 0)
            std::cout << i << "th message received from server: " << buffer << std::endl;

        sleep(2); // Wait 2 seconds before sending the next message
    }
    close(sock);
    return 0;
}