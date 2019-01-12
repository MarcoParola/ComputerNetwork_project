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
#define PORT     8081
#define MAX 512

char buffer[BUFLEN];

int main(int argc , char **argv){
    /*if(argc != 2){
        printf("Argomenti non corretti, riprovare con:\n");
        printf("IP_UDP, PORTA_UDP, IP_SERVER, PORTA_SERVER\n correggi");
        return 0;
    }*/


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
    int len, n, addlen; 
	uint16_t opcode, mode;

	while(1){
		// server si mette in ascolto
		printf("\n\n\nServer in ascolto di ricevere una richiesta!\n");
		addlen = sizeof(cliaddr);
		len = recvfrom(sockfd, (char *)buffer, BUFLEN, MSG_WAITALL, ( struct sockaddr *) &cliaddr, &addlen); 

		memcpy(&opcode, (uint16_t*)&buffer, 2);
		opcode = ntohs(opcode);
		strcpy(file_name, buffer + 2);

		memset(buffer + strlen(file_name), 0, 1);
		printf("Server: ricevuto codice %d e nome del file %s\n", opcode, file_name);
		
		if(opcode != 1){
			printf("Ricevuta richista con codice non corrispondente ad una richiesta\n");

			// invio ERRORE

		}
		


		pid = fork();

		

		if(pid == 0){
			
			FILE * fh = fopen(file_name ,"r");
			
			char block[MAX];
			char c; 
			int cont_char = 4;
			uint16_t cont_block_number = 0;
			memset(buffer, 0, BUFLEN);

			if (fh == NULL)
		        {
				// invio errore
				printf("File inesistente\n");
				return 1;
		        }
			
			do{
				c = fgetc(fh);
				buffer[cont_char] = c;
				//printf("%c", c);
				cont_char++;
				if(cont_char == MAX + 4 || c == EOF){
					// PREPARO IL BUFFER E INVIO LA RISPOSTA
					uint16_t opcode = htons(3), block_number = htons(cont_block_number);
					memcpy(buffer, &opcode, 2);
					memcpy(buffer+2, &block_number, 2);
					sendto(sockfd, buffer, cont_char, MSG_CONFIRM, (const struct sockaddr *) &cliaddr, sizeof(cliaddr));
					printf("dopo invio \n");
					memset(buffer, 0, BUFLEN);
					len = recvfrom(sockfd, (char *)buffer, BUFLEN, MSG_WAITALL, ( struct sockaddr *) &cliaddr, &addlen);
					printf("ricevuto ack lunghezza %d\n", len);
					cont_char = 4;
					cont_block_number++;
				}
			}
			while (c != EOF);
			

			    
			fclose(fh);

			exit(0);
		}
		else if(pid > 0){
			sleep(2);
		}

		
     }

    return 0; 

//-------------------------------------------------------------------------
}
