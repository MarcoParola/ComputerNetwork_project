all: tftp_client tftp_server

tftp_client: tftp_client.c 
	gcc tftp_client.c declare_client.h -o tftp_client -Wall

tftp_server: tftp_server.c
	gcc tftp_server.c declare_server.h -o tftp_server -Wall

clean:
	rm tftp_client tftp_server
