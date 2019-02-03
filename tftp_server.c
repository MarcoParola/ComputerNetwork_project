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
#define PORT     8083
#define MAX 512

char buffer[BUFLEN];

/*
	TODO 
	implementa funzione che invia pacchetto di errore ricevendo come parametro il tipo di errore

	void send_error(){
		
	}

*/

int main(int argc , char **argv){
    /*
		TODO implementa i controlli sui parametri e l'assegnazione alle variabili di tali valori 

		if(argc != 2){
        printf("Argomenti non corretti, riprovare con:\n");
        printf("IP_UDP, PORTA_UDP, IP_SERVER, PORTA_SERVER\n correggi");
        return 0;

	inserisci variabili globali per la porta e assegna i valori passati come argomento al main

    }*/

	char block[MAX];
 	int socketToAccept; 
    struct sockaddr_in servaddr, cliaddr; 
	pid_t pid;
      
    // Creating socket file descriptor 
    if ((socketToAccept = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{ 
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
    if ( bind(socketToAccept, (const struct sockaddr *)&servaddr,  sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 

    
	char file_name[128];
    int len, n, addlen; 
	uint16_t opcode, mode, block_number;
	void* pointer;

	while(1){
		// server si mette in ascolto
		printf("\n\nServer in ascolto di ricevere una richiesta!\n");
		addlen = sizeof(cliaddr);
		len = recvfrom(socketToAccept, (char *)buffer, BUFLEN, MSG_WAITALL, ( struct sockaddr *) &cliaddr, &addlen); 
		strcpy(file_name, buffer + 2);
		memcpy(&opcode, (uint16_t*)&buffer, 2);
		opcode = ntohs(opcode);
		pointer = buffer+strlen(file_name)+3;
		memcpy(&mode, (uint16_t*)pointer, 2);
		mode = ntohs(mode);
		printf("opcode: %d  mode: %d\n", opcode, mode);
		/*printf("\n mode: %04x", buffer[strlen(file_name)+1]);
		printf("%04x", buffer[strlen(file_name)+2]);
		printf("%04x", buffer[0]);
		printf("%04x\n", buffer[1]);
		printf("%04x", buffer[strlen(file_name)+3]);
		printf("%04x\n", buffer[strlen(file_name)+4]);*/

		memset(buffer + strlen(file_name), 0, 1);
		printf("Server: ricevuto codice %d e nome del file %s, mode %d\n", opcode, file_name, mode);
		
		if(opcode != 1){
			printf("Ricevuta richista con codice non corrispondente ad una richiesta\n");

			memset(block, 0, MAX);
			opcode = htons(5); block_number = htons(2);
			memcpy(buffer, &opcode, 2);
			memcpy(buffer+2, &block_number, 2);
			strcpy(block, "Illegal TFTP operation\n");
			strcpy(buffer + 4, block);
			sendto(socketToAccept, buffer, strlen(block)+4, MSG_CONFIRM, (const struct sockaddr *) &cliaddr, sizeof(cliaddr));
			return 1;

		}
		

		pid = fork();

		

		if(pid == 0){
			
			char c; 
			int cont_char = 4, socketToSend;
			uint16_t cont_block_number = 0;
			size_t dim = 0;
			memset(buffer, 0, BUFLEN);
			printf("mode : %d\n", mode);

			// Creating socket for new request
			if ((socketToSend = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
			{ 
				perror("socket creation failed"); 
				exit(EXIT_FAILURE); 
			}

			// Bind the socket with the server address 
			/*if ( bind(socketToAccept, (const struct sockaddr *)&servaddr,  sizeof(servaddr)) < 0 ) 
			{ 
				perror("bind failed"); 
				exit(EXIT_FAILURE); 
			} */

			// TXT MODE
			if(mode == 1){
				
				FILE * fh = fopen(file_name ,"r");

				if (fh == NULL)
					{
					// invio errore
					printf("File inesistente\n");
					memset(block, 0, MAX);
					uint16_t opcode = htons(5), block_number = htons(1);
					memcpy(buffer, &opcode, 2);
					memcpy(buffer+2, &block_number, 2);
					strcpy(block, "File not found\n");
					strcpy(buffer + 4, block);
					sendto(socketToSend, buffer, strlen(block)+4, MSG_CONFIRM, (const struct sockaddr *) &cliaddr, sizeof(cliaddr));
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
						sendto(socketToSend, buffer, cont_char, MSG_CONFIRM, (const struct sockaddr *) &cliaddr, sizeof(cliaddr));
						printf("dopo invio \n");
						memset(buffer, 0, BUFLEN);
						len = recvfrom(socketToSend, (char *)buffer, BUFLEN, MSG_WAITALL, ( struct sockaddr *) &cliaddr, &addlen);
						printf("ricevuto ack lunghezza %d\n", len);
						cont_char = 4;
						cont_block_number++;
					}
					//memset(buffer, 0, BUFLEN);
				}
				while (c != EOF);
			
				memset(buffer, 0, BUFLEN);
				fclose(fh);
			}
			// BIN MODE
			else if(mode == 0){
				
				FILE * fh = fopen(file_name ,"rb");

				if (fh == NULL)
					{
					// invio errore
					printf("File inesistente\n");
					memset(block, 0, MAX);
					opcode = htons(5); block_number = htons(1);
					memcpy(buffer, &opcode, 2);
					memcpy(buffer+2, &block_number, 2);
					strcpy(block, "File not found\n");
					strcpy(buffer + 4, block);
					sendto(socketToSend, buffer, strlen(block)+4, MSG_CONFIRM, (const struct sockaddr *) &cliaddr, sizeof(cliaddr));
					return 1;
					}

				// TODO implementa invio binario
	
				do{
					dim = fread(&buffer[cont_char], 1, 1, fh);
					//c = fgetc(fh);
					//buffer[cont_char] = c;
					//printf("%c", c);
					cont_char++;
					if(cont_char == MAX + 4 || dim != 1){
						// PREPARO IL BUFFER E INVIO LA RISPOSTA
						uint16_t opcode = htons(3), block_number = htons(cont_block_number);
						memcpy(buffer, &opcode, 2);
						memcpy(buffer+2, &block_number, 2);
						sendto(socketToSend, buffer, cont_char, MSG_CONFIRM, (const struct sockaddr *) &cliaddr, sizeof(cliaddr));
						printf("dopo invio \n");
						memset(buffer, 0, BUFLEN);
						len = recvfrom(socketToSend, (char *)buffer, BUFLEN, MSG_WAITALL, ( struct sockaddr *) &cliaddr, &addlen);
						printf("ricevuto ack lunghezza %d\n", len);
						//printf("invio ack %04x %04x %04x %04x %04x %04x\n\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
						cont_char = 4;
						cont_block_number++;
					}
					

				}
				while (dim ==  1);

				memset(buffer, 0, BUFLEN);
				fclose(fh);
			}

			exit(0);
		}
		else if(pid > 0){
			memset(buffer, 0, BUFLEN);
		}

		
     }

    return 0; 

//-------------------------------------------------------------------------
}
