#include "declare_server.h"


int main(int argc , char **argv){

	// controllo parametri
	if(argc != 3){
		printf("Argomenti non corretti, riprovare con:\n");
		printf("Porta del Server e Directory del file\n");
    	return 0;
    }


	char block[MAX];
 	int socketToAccept, ret, port = atoi(argv[1]); 
    struct sockaddr_in servaddr, cliaddr; 
	pid_t pid;

	printf("port: %d\n", port);
      
    // Creating socket file descriptor
	socketToAccept = socket(AF_INET, SOCK_DGRAM, 0);
    errorControl(socketToAccept);
	printf("Socket di ascolto creato\n");

      
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
      
    // Filling server information 
    servaddr.sin_family    = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(port); 
      
    // Bind the socket with the server address 
    ret = bind(socketToAccept, (const struct sockaddr *)&servaddr,  sizeof(servaddr));
    errorControl(ret);

    
	char file_name[32], mode[32];
    int len, addlen; 
	uint16_t opcode, block_number;

	while(1){
		// server si mette in ascolto
		printf("\n\nServer in ascolto di ricevere una richiesta!\n");
		addlen = sizeof(cliaddr);
		len = recvfrom(socketToAccept, (char *)buffer, BUFLEN, MSG_WAITALL, ( struct sockaddr *) &cliaddr, (socklen_t *)&addlen); 
		errorControl(len);

		strcpy(file_name, buffer + 2);
		strcpy(mode, buffer + 3 + strlen(file_name));
		memcpy(&opcode, (uint16_t*)&buffer, 2);
		opcode = ntohs(opcode);

		memset(buffer + strlen(file_name), 0, 1);
		printf("Ricevuto codice %d e nome del file %s, mode %s\n", opcode, file_name, mode);
		
		if(opcode != 1){
			printf("Ricevuta richista con codice non corrispondente ad una richiesta\n");

			memset(block, 0, MAX);
			opcode = htons(5); block_number = htons(2);
			memcpy(buffer, &opcode, 2);
			memcpy(buffer+2, &block_number, 2);
			strcpy(block, "Illegal TFTP operation\n");
			strcpy(buffer + 4, block);
			
			len = sendto(socketToAccept, buffer, strlen(block)+4, MSG_CONFIRM, (const struct sockaddr *) &cliaddr, sizeof(cliaddr));
			errorControl(len);
			return 1;
		}
		

		pid = fork();

		

		if(pid == 0){
			// codice del figlio
			
			char c; 
			int cont_char = 4, socketToSend;
			uint16_t cont_block_number = 0;
			size_t dim = 0;
			memset(buffer, 0, BUFLEN);

			// Creating socket for new request
			socketToSend = socket(AF_INET, SOCK_DGRAM, 0);
			errorControl(socketToSend);


			// TXT MODE
			if(!strcmp(mode, TXT)){
				
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
					
					len = sendto(socketToSend, buffer, strlen(block)+4, MSG_CONFIRM, (const struct sockaddr *) &cliaddr, sizeof(cliaddr));
					errorControl(len);
					return 1;
					}
		
				do{
					c = fgetc(fh);
					buffer[cont_char] = c;
					cont_char++;
					
					if(cont_char == MAX + 4 || c == EOF){
					
						// PREPARO IL BUFFER E INVIO LA RISPOSTA
						uint16_t opcode = htons(3), block_number = htons(cont_block_number);
						memcpy(buffer, &opcode, 2);
						memcpy(buffer+2, &block_number, 2);
						len = sendto(socketToSend, buffer, cont_char, MSG_CONFIRM, (const struct sockaddr *) &cliaddr, sizeof(cliaddr));
						errorControl(len);
						printf("Invio pack %04x %04x\n", buffer[2], buffer[3]);
						
						memset(buffer, 0, BUFLEN);
						len = recvfrom(socketToSend, (char *)buffer, BUFLEN, MSG_WAITALL, ( struct sockaddr *) &cliaddr, (socklen_t *)&addlen);
						errorControl(len);
						printf("Ricevuto ack %04x %04x\n", buffer[2], buffer[3]);
						
						cont_char = 4;
						cont_block_number++;
					}
				}
				while (c != EOF);
				memset(buffer, 0, BUFLEN);
				fclose(fh);
			}

			// BIN MODE
			else if(!strcmp(mode, BIN)){
				
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
					
					len = sendto(socketToSend, buffer, strlen(block)+4, MSG_CONFIRM, (const struct sockaddr *) &cliaddr, sizeof(cliaddr));
					errorControl(len);
					return 1;
					}
	
				do{

					dim = fread(&buffer[cont_char], 1, 1, fh);
					cont_char++;

					if(cont_char == MAX + 4 || dim != 1){
					
						// PREPARO IL BUFFER E INVIO LA RISPOSTA
						uint16_t opcode = htons(3), block_number = htons(cont_block_number);
						memcpy(buffer, &opcode, 2);
						memcpy(buffer+2, &block_number, 2);
						len = sendto(socketToSend, buffer, cont_char, MSG_CONFIRM, (const struct sockaddr *) &cliaddr, sizeof(cliaddr));
						errorControl(len);
						printf("Invio pack %04x %04x\n", buffer[2], buffer[3]);
						
						memset(buffer, 0, BUFLEN);
						len = recvfrom(socketToSend, (char *)buffer, BUFLEN, MSG_WAITALL, ( struct sockaddr *) &cliaddr, (socklen_t *)&addlen);
						errorControl(len);
						printf("Ricevuto ack %04x %04x\n", buffer[2], buffer[3]);
						
						cont_char = 4;
						cont_block_number++;
					}
					

				}
				while (dim ==  1);
				memset(buffer, 0, BUFLEN);
				fclose(fh);
			}
			printf("Invio completato!\n");
			close(socketToSend); 
			exit(0);
		}
		else if(pid > 0){
			memset(buffer, 0, BUFLEN);
		}

		
     }

    return 0; 

}
