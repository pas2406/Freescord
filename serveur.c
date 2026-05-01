#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list/list.h"
#include "user.h"

#define PORT_FREESCORD 4321
#define NB_CLIENTS 124

/** Gérer toutes les communications avec le client renseigné dans
 * user, qui doit être l'adresse d'une struct user */
void *handle_client(void *user);
/** Créer et configurer une socket d'écoute sur le port donné en argument
 * retourne le descripteur de cette socket, ou -1 en cas d'erreur */
int create_listening_sock(uint16_t port);

int main(int argc, char *argv[])
{
	
	
	int sock_l = create_listening_sock(PORT_FREESCORD);
	if(sock_l < 0){
		fprintf(stderr, "Erreur lors de la création de la socket d'écoute\n");
		exit(1);
	}
	
	
	
	while(1){
		
		char buf[256];
		ssize_t n;
		struct sockaddr_in sa_clt;
		socklen_t sl = sizeof(sa_clt);
		

		int fds_clt = accept(sock_l, (struct sockaddr *) &sa_clt, &sl);
		if (fds_clt< 0) { 
			perror("accept");
			close(sock_l);
			exit(1);
		}
		

		while (1)
		{
			
			if ((n = read(fds_clt, buf, 256)) < 0) {
				perror("read");
				break;
	
			}
	
			
			if(n == 0){
				fprintf(stderr, "Client déconnecté\n");
				break;
			}
			
			//printf("Message reçu : %s\n", buf);	
			if((write(fds_clt, buf, n)) < 0){
				perror("write");
				break;
			}
		}

		close(fds_clt);
		




	}
	close(sock_l);


}

void *handle_client(void *clt)
{
	return clt;
}

int create_listening_sock(uint16_t port)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0){
		perror("socket");
		close(sock);
		return(-1);
	}

	struct sockaddr_in s_addr;
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(port);
	s_addr.sin_addr.s_addr = INADDR_ANY;

	int opt = 1;
	// Permet de réutiliser l'adresse même si elle est encore en TIME_WAIT
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

	//on attache la socket à l'adresse et au port
	if (bind(sock, (struct sockaddr *) &s_addr, sizeof(s_addr)) < 0) {
		perror("bind");
		close(sock);
		return(-1);
	}

	if(listen(sock, NB_CLIENTS) < 0){
		perror("listen");
		close(sock);
		return(-1);
	}
	return sock;
}
