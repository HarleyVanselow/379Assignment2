#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <pthread.h>

int number_of_connected_users;

void * check_connection(unsigned char * buf){
	if (buf[0] == 207 && buf[1] == 167){
		printf("Connection Established!\n");
		fflush(stdout);	
	} else {
		printf("Connection closed by server\n");
		fflush(stdout);	
		exit(-1);
	}
}

void * connect_to_server(){

}

void * read_user_input(void * client_socket){

	char message[256];

	while(1){
			
		// This is blocking -> should make this non-blocking
		printf("Enter message: "); fflush(stdout);
		fgets(message, 256, stdin);
		if (strcmp(".quit\n\0", message) == 0){
			exit(1);
		} else {
			printf("Sending: %s", message); fflush(stdout);
			send(*((int *)client_socket),message,256,0);//Shouldnt really be 256
			memset(message,0,256);
		}

	}
}

void should_quit(char * command){
	printf("%s", command);
	fflush(stdout);
	if (command == ".quit"){
		exit(1);
	}
}

int main(int argc, char const *argv[])
{
	if (argc != 4){
		printf("Invalid parameters!\n");
		return -1;
	}

	const char * hostname = argv[1];
	int port_number = atoi(argv[2]);
	const char * username = argv[3];

	int client_socket; 
	uint32_t number;

	unsigned char buf[256];

	struct	sockaddr_in	server;

	struct	hostent		*host;

	host = gethostbyname (hostname);

	if (host == NULL) {
		perror ("Client: cannot get host description");
		exit (1);
	}


	client_socket = socket (AF_INET, SOCK_STREAM, 0);

	if (client_socket < 0) {
		perror ("Client: cannot open socket");
		exit (1);
	}

	bzero (&server, sizeof (server));
	bcopy (host->h_addr, & (server.sin_addr), host->h_length);
	server.sin_family = host->h_addrtype;
	server.sin_port = htons (port_number);

	if (connect (client_socket, (struct sockaddr*) & server, sizeof (server))) {
		perror ("Client: cannot connect to server");
		exit (1);
	}

	read (client_socket, &buf, 256);
	check_connection(buf);
	
	number_of_connected_users = buf[2];
	printf("There are already %d users connected!\n", number_of_connected_users);
	printf("Type .quit to quit the chat\n");
	fflush(stdout);
		// Now get the list of usernames 
	
	pthread_t user_input;
	
	pthread_create(&user_input, NULL, read_user_input, &client_socket);



			
			// This is blocking -> should make this non-blocking

			printf("Enter message: "); fflush(stdout);
			fgets(message, 256, stdin);
			if (strcmp(".quit\n\0", message) == 0){
				// server.close();
				// server.shutdown();
				exit(1);
			} else {
				printf("Sending: %s", message); fflush(stdout);
				send(client_socket,message,256,0);//Shouldnt really be 256
				memset(message,0,256);
			}
		}
	}
}

