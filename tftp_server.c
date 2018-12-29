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
#define PORT     8080 

char buffer[BUFLEN];

int main(int argc , char **argv){
    /*if(argc != 2){
        printf("Argomenti non corretti, riprovare con:\n");
        printf("IP_UDP, PORTA_UDP, IP_SERVER, PORTA_SERVER\n correggi");
        return 0;
    }*/

	/*int ret, sd, len;
	char buf[BUFLEN];
	struct sockaddr_in my_addr, cl_addr;
	int addrlen = sizeof(cl_addr);
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	memset(&my_addr, 0, sizeof(my_addr)); // Pulizia
	my_addr.sin_family = AF_INET ;
	my_addr.sin_port = htons(4242);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	ret = bind(sd, (struct sockaddr*)&my_addr, sizeof(my_addr));
	while(1){
		len = recvfrom(sd, buf, BUFLEN, 0,(struct sockaddr*)&cl_addr, &addrlen);
		printf("%s, %d\n", buf, len);
	}*/

//-------------------------------------------------------------------------

 	int sockfd; 
    char *hello = "Hellooo from server"; 
    struct sockaddr_in servaddr, cliaddr; 
      
    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
      
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
      
    // Filling server information 
    servaddr.sin_family    = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(PORT); 
      
    // Bind the socket with the server address 
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
      
    int len, n; 
	
	// server si mette in ascolto
    n = recvfrom(sockfd, (char *)buffer, BUFLEN, MSG_WAITALL, ( struct sockaddr *) &cliaddr, &len); 
    buffer[n] = '\0'; 

    printf("Server : I receive ' %s '\n", buffer); 
      
    return 0; 

//-------------------------------------------------------------------------
}
