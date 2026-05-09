#include "user.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>

/** accepter une connection TCP depuis la socket d'écoute sl et retourner un
 * pointeur vers un struct user, dynamiquement alloué et convenablement
 * initialisé */
struct user *user_accept(int sl)
{

	struct user * usr = malloc(sizeof(struct user));

	if(! usr){
		perror("malloc");
		exit(1);
	}

	usr->address = malloc(sizeof(struct sockaddr));
	if(!(usr->address)){
		perror("malloc");
		exit(1);
	}

	usr->sock =  accept(sl, usr->address, &(usr->addr_len));
	if(usr->sock < 0){
		perror("accept");
		exit(1);
	}

	return usr;
	
}

/** libérer toute la mémoire associée à user */
void user_free(struct user *user)
{
	free(user-> address);
	free(user);

}
