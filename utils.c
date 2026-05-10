#include <string.h>

/* Changer une ligne se terminant par CRLF en une ligne se terminant par LF.
 * La ligne doit être terminée par un carcactère nul.
 * Retourne la ligne modifiée ou NULL en cas d'erreur. */
char *crlf_to_lf(char *line_with_crlf)
{

	
	if(! line_with_crlf){
		return NULL;
	}

	int len = strlen(line_with_crlf);
	if(len < 2){
		return NULL;
	}
	
	
	//on verifie si on a bien une ligne qui se termine par crlf
	if(line_with_crlf[len - 1] != '\n' || line_with_crlf[len - 2] != '\r' ){
		return NULL;
	}

	line_with_crlf[len - 2] = '\n';
	line_with_crlf[len - 1] = '\0';

	return line_with_crlf;
}


/* Changer une ligne se terminant par LF en une ligne se terminant par CRLF.
 * La ligne doit être terminée par un carcactère nul.
 * Attention, l'utilisateur doit vérifier que le tableau peut contenir
 * un caractère supplémentaire.
 * Retourne la ligne modifiée ou NULL en cas d'erreur. */
char *lf_to_crlf(char *line_with_lf)
{

	if( !line_with_lf){
		return NULL;
	}

	int len = strlen(line_with_lf);

	if(len < 1){
		return NULL;
	}

	if(line_with_lf[len - 1] != '\n'){
		return NULL;
	}


	line_with_lf[len - 1] = '\r';
	line_with_lf[len] = '\n';
	line_with_lf[len + 1] = '\0';

	return line_with_lf;
}
