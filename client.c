/* Papa Alioune SECK 12410002
Je déclare qu'il s'agit de mon propre travail.
Ce travail a été réalisé intégralement par un être humain. */


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
#define SIZE_MESSAGE 514

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

	printf("\t\t\t FREESCORD \t\t\t\n");
	printf("Ecivez votre message\n");

	
	buffer * buff_sock = buff_create(fd_sock, SIZE_MESSAGE); //buffer du socket

	while (1)
	{
		
		char msg_srv[SIZE_MESSAGE]; //message du client
		char msg_clt[SIZE_MESSAGE]; //message du serveur
		

		struct pollfd fds[2] =
				{ { .fd = 0, .events = POLLIN },
				{ .fd = fd_sock, .events = POLLIN } };

		
		if(poll(fds, 2, -1) < 0){
			perror("poll");
			exit(1);
		}

		//le client envoie une ligne
		if(fds[0].revents & POLLIN){
			if(fgets(msg_clt, SIZE_MESSAGE, stdin) == NULL){
				perror("fgets");
				break;
			}

			lf_to_crlf(msg_clt); //on converit avant de l'envoer
			if((write(fd_sock, msg_clt, strlen(msg_clt))) < 0){
				perror("write");
				break;
			}
		}

		//on reupere le message renvoyé par le seveur
		if (fds[1].revents & POLLIN){
			if((buff_fgets_crlf(buff_sock, msg_srv, SIZE_MESSAGE)) == NULL){
				perror("read");
				break;
			}

			
			crlf_to_lf(msg_srv);
			printf("Message reçu : %s\n", msg_srv);
		}
	}

	buff_free(buff_sock);
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
