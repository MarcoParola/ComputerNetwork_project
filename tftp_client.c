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
#define PORT     8080 
#define BIN 0
#define TXT 1

// GLOBAL VARIABLES
char cmd_string[BUFLEN], buffer[BUFLEN];
uint16_t mode = BIN;


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
void get_cmd(char * file_name, int* len){

	uint16_t opcode = htons(1), mod = htons(mode);
	uint8_t endString = 0;
	int i=0;
	char * temp = buffer;

	memcpy(buffer, &opcode, 2);
	*len += 2;

	strcpy(buffer+*len, file_name);
	*len += strlen(file_name);

	memcpy(buffer + * len, &endString, 1);
	(*len)++;

	memcpy(buffer + * len, &mod, 2);
	*len += 2;

	memcpy(buffer + * len, &endString, 1);
	(*len)++;

	printf("%04x", buffer[0]);
	printf("%04x", buffer[1]);
	printf("%s", buffer+2);
	printf("%04x", buffer[*len - 4]);
	printf("%04x", buffer[*len-3]);
	printf("%04x", buffer[*len -2]);
	printf("%04x\n\n", buffer[*len -1]);

}



// MAIN
int main(int argc , char **argv){

    char *hello = "Hellooo from client"; 
    struct sockaddr_in     servaddr; 
	int ret, sd, len;
	//char buf[BUFLEN];
	struct sockaddr_in sv_addr; // Struttura per il server
	// Creating socket file descriptor 
    if ( (sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
  
    memset(&servaddr, 0, sizeof(servaddr)); 
      
    // Filling server information 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(PORT); 
    servaddr.sin_addr.s_addr = INADDR_ANY; 

    /*if(argc != 2){
        printf("Argomenti non corretti, riprovare con:\n");
        printf("IP_UDP, PORTA_UDP, IP_SERVER, PORTA_SERVER\n correggi");
        return 0;
    }*/


	help_cmd();
		

	while(1){
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
			int len=0;
			char * nome = strtok(&cmd_string[5], " ");
			char * nome_locale = strtok(NULL, " ");
	
			get_cmd(nome, &len);

			// client invia messaggio
			sendto(sd, buffer, len, MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 
			printf("Client: Hello message sent :)\n"); 
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


