#include <stdio.h>
#include <string.h>  
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>  
#include <arpa/inet.h>    
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
  
#define BUFLEN 1024
#define SIZE 32
#define BIN "octet"
#define TXT "netascii"

// GLOBAL VARIABLES
char cmd_string[BUFLEN], buffer[BUFLEN], mode[SIZE];
uint16_t opcode;


// HELP
void help_cmd(){
	printf("\nSono disponibili i seguenti comandi:\n");
	printf("!help --> mostra l'elenco dei comandi disponibili\n");
	printf("!mode {txt|bin} --> imposta il modo di trasferimento dei file (testo o binario)\n");
	printf("!get filename nome_locale --> richiede al server il nome del file <filename> e lo salva localmente col nome <nome_locale>\n");
	printf("!quit --> termina il client\n");
	return;
}


// MODE
void set_mode(){

	memset(cmd_string, 0, BUFLEN);
	scanf("%s", cmd_string);	
	
	if(!strncmp(&cmd_string[0], "bin", 3)){
		strcpy(mode, BIN);
		printf("Modo di trasferimento binario configurato\n");
	}

	else if(!strncmp(&cmd_string[0], "txt", 3)){
		strcpy(mode, TXT);
		printf("Modo di trasferimento testuale configurato\n");
	}

	else
		printf("Modo di trasferimento non trovato\n");
}


// GET
void prepare_request(char * file_name, int* len){

	memset(buffer, 0, BUFLEN);
	opcode = htons(1);
	uint8_t endString = 0;

	memcpy(buffer, &opcode, 2);
	*len += 2;

	strcpy(buffer+*len, file_name);
	*len += strlen(file_name);

	memcpy(buffer + * len, &endString, 1);
	(*len)++;

	strcpy(buffer+*len, mode);
	*len += strlen(mode);

	memcpy(buffer + * len, &endString, 1);
	(*len)++;

}


void errorControl(int n){

	if (n < 0)
	{ 
        perror("ERRORE!\n"); 
        exit(EXIT_FAILURE); 
	}
}
