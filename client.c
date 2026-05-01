#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include "buffer/buffer.h"
#include "utils.h"

#define PORT_FREESCORD 4321

/** se connecter au serveur TCP d'adresse donnée en argument sous forme de
 * chaîne de caractère et au port donné en argument
 * retourne le descripteur de fichier de la socket obtenue ou -1 en cas
 * d'erreur. */
int connect_serveur_tcp(char *adresse, uint16_t port);

int main(int argc, char *argv[])

{	
	int fd_sock = connect_serveur_tcp(argv[1], PORT_FREESCORD);
	if(fd_sock < 0){
		fprintf(stderr, "Erreur lors de la connexion au serveur\n");
		exit(1);
	}

	while (1)
	{
		char buf[256];
		char buf_stdin[256];
			
		if(fgets(buf, 256, stdin) == NULL){
			perror("fgets");
			break;
		}

		if((write(fd_sock, buf, strlen(buf))) < 0){
			perror("write");
			break;
		}

		if(read(fd_sock, buf_stdin, strlen(buf)) < 0){
			perror("read");
			break;
		}

		buf[strlen(buf)-1] = '\0';
		printf("Message reçu : %s\n", buf_stdin);
	
	}

	close(fd_sock);
		
	
	return 0;
}

int connect_serveur_tcp(char *adresse, uint16_t port)
{
	int fd_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(fd_sock < 0){
		perror("socket");
		return(-1);
	}

	//Rentre dans une sockaddr l’adresse (par exemple IPv4) et le port du serveur.
	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);

	
	//convertir une adresse IP sous forme texte (string) en une forme binaire utilisable par les sockets.
	if (inet_pton(AF_INET, adresse, &sa.sin_addr) < 0) {
		perror("inet_pton");
		close(fd_sock);
		return(-1);
	}

	//on connecte la socket à l’adresse stockee dans sa
	if(connect(fd_sock, (struct sockaddr *) &sa, sizeof(sa))){
		perror("connect");
		close(fd_sock);
		return(-1);
	}
	return fd_sock;
	
}
