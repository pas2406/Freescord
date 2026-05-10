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
#include "list/list.h"
#include "user.h"

#define PORT_FREESCORD 4321
#define SZ_MESSAGE 514
#define NB_CLIENTS 124

//tube variable globale
int tube[2];


/** Gérer toutes les communications avec le client renseigné dans
 * user, qui doit être l'adresse d'une struct user */
void *handle_client(void *user);
/** Créer et configurer une socket d'écoute sur le port donné en argument
 * retourne le descripteur de cette socket, ou -1 en cas d'erreur */
int create_listening_sock(uint16_t port);


void * repeteur(void * arg);

int main(int argc, char *argv[])
{
	if(pipe(tube) < 0){
		perror("pipe");
		exit(-1);
	}
	
	struct list * user_l = list_create(); 
	
	int sock_l = create_listening_sock(PORT_FREESCORD);
	if(sock_l < 0){
		fprintf(stderr, "Erreur lors de la création de la socket d'écoute\n");
		exit(1);
	}
	
	
	pthread_t thr;
	pthread_create(&thr, NULL, repeteur, user_l );
	pthread_detach(thr);
	
	
	printf("\t\t\t SERVEUR FREESCORD \t\t\t\n");
	
	
	while(1){
		

		struct user * clt = user_accept(sock_l);

		//on ajoute le client accepté dans la liste
		user_l = list_add(user_l, clt);

		pthread_t th;

		pthread_create(&th, NULL, handle_client, clt);

		pthread_detach(th);
		

	}
	


}

void *handle_client(void *clt)
{

	struct user * u_clt = clt;
	
	char buf[SZ_MESSAGE];
	ssize_t n;

	while (1)
	{
		
		if ((n = read(u_clt->sock, buf, SZ_MESSAGE)) < 0) {
			perror("read");
			break;

		}

		
		if(n == 0){
			fprintf(stderr, "Client déconnecté\n");
			break;
		}
		
		

		//on recopie le message recu dans le tube
		if((write(tube[1], buf, n)) < 0){
			perror("write");
			break;
		}

	}

	close(u_clt->sock);
	user_free(u_clt);


	
	return NULL;
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

	// Permet de réutiliser l'adresse même si elle est encore en TIME_WAIT
	int opt = 1;
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



void * repeteur(void * arg){

	
	struct list * user_l = arg;

	while(1){
		char buf[256];
		ssize_t n;

		if((n = read(tube[0], buf, 255)) < 0){
			perror("read");
			exit(1);
		}

		//faire un mutex plus tard pour proteger la liste user parceque sinon imaginons le client se termine et appelle remove node et change prev et next
		struct node * curr = user_l -> first; // pour parcourir la liste

		while(curr != NULL){
			
			struct user * clt = (struct user *)(curr -> elt);
			if(write(clt-> sock, buf, n)< 0){
				perror("write");                                                                                                 
				exit(2);
			}

			curr = curr -> next;
		}

	}



	return NULL;
}