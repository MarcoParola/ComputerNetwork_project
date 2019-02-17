#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
  

#define BUFLEN 1024
#define SIZE 32
#define MAX 512
#define BIN "octet"
#define TXT "netascii"

char buffer[BUFLEN];

void errorControl(int n){

	if (n < 0)
	{ 
        perror("ERRORE!\n"); 
        exit(EXIT_FAILURE); 
	}
}
