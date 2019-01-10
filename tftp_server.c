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
    struct sockaddr_in servaddr, cliaddr; 
	pid_t pid;
      
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
    
	char file_name[128];
    int len, n; 
	uint16_t opcode, mode;
	while(1){
		// server si mette in ascolto
		printf("Server in ascolto di ricevere una richiesta!\n");
		len = recvfrom(sockfd, (char *)buffer, BUFLEN, MSG_WAITALL, ( struct sockaddr *) &cliaddr, &len); 

		//buffer[n] = '\0'; 

		//printf("Server : I receive %d\n", len); 
		printf("%04x", buffer[0]);
		printf("%04x", buffer[1]);
		printf("%s\n", buffer+2);
		//printf("%04x", buffer[len - 4]);
		/*printf("%04x", buffer[lenn-3]);
		printf("%04x", buffer[*lenn -2]);
		printf("%04x\n\n", buffer[*lenn -1]);*/

		memcpy(&opcode, (uint16_t*)&buffer, 2);
		opcode = ntohs(opcode);
		strcpy(file_name, buffer + 2);

		if(opcode != 1){
			printf("Ricevuta richista con codice non corrispondente ad una richiesta");
			// invio errore 

		}

		pid = fork();

		/*if(pid > 0){

			// codice del processo figlio che invia un file

			FILE * fh = fopen( ,"r");				// TODO metti nome
			char block[512];
			
			if (fh == NULL)
		        {
				// invio errore
				printf("File inesistente\n");
				return 1;
		        }
			
			while (!feof(fh))
			{
				setbuf(fh, block);
				//sendto(sd, buffer, len, MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 
				printf("%s", block);
			}

			    
			fclose(fh);

			exit(0);
		}*/

		
     }

    return 0; 

//-------------------------------------------------------------------------
}
