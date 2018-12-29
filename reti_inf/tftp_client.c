#include <stdio.h>
#include <string.h>  
#include <stdlib.h>
/*#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
  */
#define BUFLEN 1024
#define BIN 0
#define TXT 1

// GLOBAL VARIABLES
char cmd_string[BUFLEN], buffer[BUFLEN];
int mode = BIN;


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


int main(int argc , char **argv){
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
		printf("%s, %d\n", cmd_string, strlen(cmd_string));
	

		if(!strncmp(cmd_string, "!help", 5)){
				printf("help");
				help_cmd();	
		}

		// MODE
	
		else if(!strncmp(cmd_string, "!mode", 5)){
			set_mode();
		}
		// GET

		else if(!strncmp(cmd_string, "!get", 4)){
			printf("get");
		}


		// QUIT

		else if(!strncmp(cmd_string, "!quit", 5)){
			printf("Terminazione Client\n");
			return 0;
		}

		else
			printf("Commando non riconosciuto!\n");
		
	}

	return 0;
}


