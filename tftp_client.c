#include "declare_client.h"


// MAIN
int main(int argc , char **argv){

	// controllo paramentri
    if(argc != 3){
		printf("Argomenti non corretti, riprovare con:\n");
		printf("IP e Porta del Server\n");
    	return 0;
    }

    struct sockaddr_in     servaddr; 
	int addrlen, i, port = atoi(argv[2]);
	char serverIP[SIZE];
	strcpy(serverIP, argv[1]);
	
	// default trasfer
	strcpy(mode, BIN);

	help_cmd();
		

	while(1){

		int sd;
		
		memset(&servaddr, 0, sizeof(servaddr)); 
      
		// Filling server information 
		servaddr.sin_family = AF_INET; 
		servaddr.sin_port = htons(port); 
		inet_pton(AF_INET, serverIP, &servaddr.sin_addr);

		// Creating socket file descriptor 
		sd = socket(AF_INET, SOCK_DGRAM, 0);
		errorControl(sd);
		

		printf("\n");
		memset(cmd_string, 0, BUFLEN);
		scanf("%s", cmd_string);

		// HELP
		if(!strncmp(cmd_string, "!help", 5)){
				help_cmd();	
		}


		// MODE
		else if(!strncmp(cmd_string, "!mode", 5)){
			set_mode();
		}


		// GET
		else if(!strncmp(cmd_string, "!get", 4)){
			int ret, len=0;
			char nome_locale[SIZE], nome[SIZE];
			scanf("%s%s", nome, nome_locale);
			printf("%s  %s\n", nome, nome_locale);
			prepare_request(nome, &len);
			
		
			// client invia richiesta
			ret = sendto(sd, buffer, len, MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 
			errorControl(ret);

			// attesa risposta: primo pacchetto del file o 
			memset(buffer, 0, BUFLEN);
			addrlen = sizeof(servaddr);
			len = recvfrom(sd, (char *)buffer, BUFLEN, MSG_WAITALL, (struct sockaddr *) &servaddr, (socklen_t *)&addrlen);
			errorControl(len);

			// ricevuto primo pacchetto
			memcpy(&opcode, (uint16_t*)&buffer, 2);
			opcode = ntohs(opcode);
		

			if(opcode == 5)
				printf("ERRORE: %s", buffer+4);
		
			else if(opcode == 3){
				// ricevuto file
				FILE * fh = fopen(nome_locale ,"w");
				printf("Attendere... Download in corso!\n");
			
				// scrittura su file fino all'eof o al 512esimo byte
				for(i=0; i<len-4; i++)
						fputc(buffer[i+4], fh);

				// invio ACK
				memset(buffer+4, 0, BUFLEN-4);
				opcode = htons(4);
				memcpy(buffer, &opcode, 2);
				ret = sendto(sd, buffer, 4, MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 
				errorControl(ret);

				while(len == 516){
					addrlen = sizeof(servaddr);
					len = recvfrom(sd, (char *)buffer, BUFLEN, MSG_WAITALL, ( struct sockaddr *) &servaddr, (socklen_t *)&addrlen);
					errorControl(len);

					if(len != 516 && (!strncmp(&cmd_string[0], "txt", 3))){
						for(i=0; i<len-5; i++)
							fputc(buffer[i+4], fh);
					}

					for(i=0; i<len-4; i++)
						fputc(buffer[i+4], fh);

					memset(buffer+4, 0, BUFLEN-4);
					opcode = htons(4);
					memcpy(buffer, &opcode, 2);
					ret = sendto(sd, buffer, 4, MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 
					errorControl(ret);
				}
				
				printf("Download avvenuto con successo!\n");
				fclose(fh);
				close(sd);
			}
		}

		// QUIT

		else if(!strncmp(cmd_string, "!quit", 5)){
    		close(sd); 
			printf("Terminazione Client\n");
			return 0;
		}

		else
			printf("Commando non riconosciuto!\n");
		
	}

	return 0;
}

