#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int server_socket;//on crée le fd du serveur

int	main() {
server_socket = socket(PF_INET, SOCK_STREAM, 0);//crée un nouveau socket, famille d'adresse = IPV4,type de socket = TCP, utilise le protocole par défaut soit TCP

struct sockaddr_in	server_addr;//structure qui va stocker l'addresse ip et le port
memset(&server_addr, 0, sizeof(server_addr));
server_addr.sin_family = AF_INET;//spécifie la famille d'adresses utilisées ici IPV4
server_addr.sin_port = htons(4000);//numero du port sur lequel le serveur écoutera
server_addr.sin_addr.s_addr = htonl(INADDR_ANY);//contient l'adresse IP du server, INADDR_ANY signifie : accepter des connexions sur toutes les interfaces réseau dispo (wifi, ethernet, etc...)


}

