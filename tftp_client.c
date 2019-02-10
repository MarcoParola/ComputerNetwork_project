#include <stdio.h>
#include <string.h>  
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
  
#define BUFLEN 1024
#define BIN 0
#define TXT 1

// GLOBAL VARIABLES
char cmd_string[BUFLEN], buffer[BUFLEN];
uint16_t mode = TXT, opcode;


// HELP
void help_cmd(){
	printf("\nSono disponibili i seguenti comandi:\n");
	printf("!help --> mostra l'elenco dei comandi disponibili\n");
	printf("!mode {txt|bin} --> imposta il modo di trasferimento dei file (testo o binario)\n");
	printf("!get filename nome_locale --> richiede al server il nome del file <filename> e lo salva localmente col nome <nome_locale>\n");
	printf("!quit --> termina il clinet\n");
	return;
}


// MODE
void set_mode(){

	if(!strncmp(&cmd_string[6], "bin", 3)){
		mode = BIN;
		printf("Modo di trasferimento binario configurato\n");
	}

	else if(!strncmp(&cmd_string[6], "txt", 3)){
		mode = TXT;
		printf("Modo di trasferimento testuale configurato\n");
	}

	else
		printf("Modo di trasferimento non trovato\n");
}


// GET
void prepare_request(char * file_name, int* len){

	printf("nome: %s, len: %d\n", file_name, *len);
	memset(buffer, 0, BUFLEN);
	opcode = htons(1);
	mode = htons(mode);
	uint8_t endString = 0;
	int i=0;
	char * temp = buffer;

	memcpy(buffer, &opcode, 2);
	*len += 2;

	strcpy(buffer+*len, file_name);
	*len += strlen(file_name);

	memcpy(buffer + * len, &endString, 1);
	(*len)++;

	memcpy(buffer + * len, &mode, 2);
	*len += 2;

	memcpy(buffer + * len, &endString, 1);
	(*len)++;

	printf("%04x", buffer[0]);
	printf("%04x", buffer[1]);
	printf("%s", buffer+2);
	printf("%04x", buffer[*len - 4]);
	printf("%04x", buffer[*len-3]);
	printf("%04x", buffer[*len -2]);
	printf("%04x", buffer[*len -1]);
	printf("%04x", buffer[*len]);
	printf("%04x", buffer[*len+1]);
	printf("%04x", buffer[*len +2]);
	printf("%04x\n\n", buffer[*len +3]);

}



// MAIN
int main(int argc , char **argv){

	
    if(argc != 3){
		printf("Argomenti non corretti, riprovare con:\n");
		printf("IP e Porta del Server\n");
    	return 0;
    }

    struct sockaddr_in     servaddr; 
	int ret, len, addrlen, i, port = atoi(argv[2]);
	//struct sockaddr_in sv_addr; // Struttura per il server
	char serverIP[32];
	strcpy(serverIP, argv[1]);


	help_cmd();
		

	while(1){

		int sd;
		
		memset(&servaddr, 0, sizeof(servaddr)); 
      
		// Filling server information 
		servaddr.sin_family = AF_INET; 
		servaddr.sin_port = htons(port); 
		inet_pton(AF_INET, serverIP, &servaddr.sin_addr);

		// Creating socket file descriptor 
		if ( (sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		    perror("socket creation failed"); 
		    exit(EXIT_FAILURE); 
		}
		

		printf("\n");
		memset(cmd_string, 0, BUFLEN);
		fgets(cmd_string, BUFLEN, stdin);


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
			FILE * fh;
			int len=0;
			char * nome = strtok(&cmd_string[5], " ");
			char * nome_locale = strtok(NULL, " ");
			char nome_locale_estensione[strlen(nome_locale) + 3];
		

			// salvo il nome con cui devo rinominare il file in locale
			strncpy(nome_locale_estensione, nome_locale, strlen(nome_locale) - 1);		

			prepare_request(nome, &len);
			mode = ntohs(mode);
		
			// client invia richiesta
			sendto(sd, buffer, len, MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 
		
			// attesa risposta: primo pacchetto del file o 
			memset(buffer, 0, BUFLEN);
			addrlen = sizeof(servaddr);
			len = recvfrom(sd, (char *)buffer, BUFLEN, MSG_WAITALL, (struct sockaddr *) &servaddr, &addrlen);
			

			// ricevuto primo pacchetto
			memcpy(&opcode, (uint16_t*)&buffer, 2);
			opcode = ntohs(opcode);
		

			if(opcode == 5)
				printf("ERRORE: %s", buffer+4);
		
			else if(opcode == 3){
				// ricevuto file
				FILE * fh = fopen(nome_locale_estensione ,"w");
				uint16_t block_number;
				printf("Attendere... Download in corso!\n");
			
				// scrittura su file fino all'eof o al 512esimo byte
				for(i=0; i<len-4; i++)
						fputc(buffer[i+4], fh);

				// invio ACK
				memset(buffer+4, 0, BUFLEN-4);
				opcode = htons(4);
				memcpy(buffer, &opcode, 2);
				printf("invio ack %04x %04x %04x %04x %04x %04x\n\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
				sendto(sd, buffer, 4, MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 


				while(len == 516){
					addrlen = sizeof(servaddr);
					len = recvfrom(sd, (char *)buffer, BUFLEN, MSG_WAITALL, ( struct sockaddr *) &servaddr, &addrlen);

					for(i=0; i<len-4; i++)
						fputc(buffer[i+4], fh);

					memset(buffer+4, 0, BUFLEN-4);
					opcode = htons(4);
					memcpy(buffer, &opcode, 2);
					sendto(sd, buffer, 4, MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 
					printf("invio ack %04x %04x %04x %04x %04x %04x\n\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
				}
				//memset(buffer, 0, BUFLEN);
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


