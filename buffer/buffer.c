#include "buffer.h"

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

struct buffer {
	int fd;
	size_t buffsz;
	char * buf;
	size_t cursor;
	size_t valid_sz; //taille des caracteres valides
	int eof; // 1 si la fin du fichier est atteinte , 0 sinon
	
};

buffer *buff_create(int fd, size_t buffsz)
{
	buffer * res = malloc(sizeof(buffer));
	if(!res) {
		perror("malloc");
		return NULL;
	}

	res -> fd = fd;
	res -> buffsz = buffsz;
	res -> cursor = 0;
	res -> valid_sz = 0;
	res -> eof = 0;

	res -> buf = malloc(sizeof(char) * buffsz);
	if(!(res -> buf)){
		perror("malloc");
		free(res);
		return NULL;
	}

	return res;
}

int buff_getc(buffer *b)
{
	size_t n;

	if(b->eof == 1){
		return EOF;
	}
	if(b -> cursor >=  b -> valid_sz){ //tous les caractere valides du buffer sont consommés
		if((n = read(b->fd, b->buf, b->buffsz)) == 0){
			b ->eof = 1;
			return EOF;
		}

		if (n < 0){
			perror("read");
			return EOF;
		}
		b->valid_sz = n;
		b->cursor = 0;
		
	
	}

	return b->buf[b->cursor++];


}

int buff_ungetc(buffer *b, int c)
{
	if (b->cursor == 0){
		return EOF;
	}

	
	b->cursor--;
	b->buf[b->cursor] = c;

	return c;
}

void buff_free(buffer *b)
{
	free(b->buf);
	free(b);
}

int buff_eof(const buffer *buff)
{
	
	return buff-> eof;
}

int buff_ready(const buffer *buff)
{
	
	if((buff->valid_sz) > 0 && (buff->cursor)  <  (buff->valid_sz)){
		return 1;
	}
	return 0;
}


char *buff_fgets(buffer *b, char *dest, size_t size)
{
	int i = 0;
	int c;
	while(i < (size - 1)){
		c = buff_getc(b);
		
		if(c == EOF){ //si on atteint la fin du fichier
			if(i == 0){
				return NULL; // sans que des caracteres soient lus
			}else{
				
				break;
			}	

		}else if (c == '\n'){
			dest[i] = c;
			i++;
			break;
			
		}
		dest[i] = c;
		i++;
		
	}
	dest[i] = '\0';
	
	return(dest);
	
}



char *buff_fgets_crlf(buffer *b, char *dest, size_t size)
{
	int i = 0;
	int c1, c2;
	while(i < (size - 2)){
		c1 = buff_getc(b);

		
		if(c1 == EOF){ //si on atteint la fin du fichier
			if(i == 0){ 
				return NULL; // sans que des caracteres soient lus
			}else{
				break;
			}	
			
		}else if (c1 == '\r'){
			if((c2 = buff_getc(b)) == '\n'){
				dest[i] = c1;
				dest[i + 1] = c2; 
				i+=2;
				break;

			}else{
				if(c2 != EOF){
					buff_ungetc(b, c2);
				}
			}	
			
			
		}
		
		dest[i] = c1;
		i++;
			
	}
	dest[i] = '\0';
	return(dest);
	
}
