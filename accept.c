#include "server.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <pthread.h>
#include <string.h>
void* Accept(void* input)
{
	fprintf(f, "Accept thread started\n");
	struct sockaddr_in socketAddress;
	struct sockaddr_in remoteSocketAddress;
	socklen_t addrlen;
	int listener;
	int maxFD;
	unsigned char write_buf[256];
	unsigned char read_buf[256];
	int nbytes =0;
	int i=0; //General use iterator
	int j=0;
	int write_buf_itr =0;
	int read_buf_itr =0;

	//Set up listener socket
	listener = socket(AF_INET, SOCK_STREAM, 0);
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_addr.s_addr =INADDR_ANY;// inet_addr("127.0.0.1");
    socketAddress.sin_port = htons(MY_PORT);

    int bindResult = bind(listener,(struct sockaddr *)&socketAddress, sizeof(socketAddress));
    if(bindResult <0){
    	fprintf(f,"Error binding. Terminating...");
    	exit(-1);
    }else{
    	printf("Binding successful. Opened socket on port: %d\n",MY_PORT);
    	fflush(stdout);
    }
    maxFD = listener;
    
    if(listen(listener,50) == -1){ //To many queued?
    	exit(-1);
    }

    FD_SET(listener,&master); //Todo: mutex

    while(1){
    	//Re initialize all buffer handlers
    	memset(&write_buf,0,256);
    	memset(&read_buf,0,256);
    	write_buf_itr =0;
    	read_buf_itr=0;

    	if(select(listener+1,&master,NULL,NULL,NULL) == -1){
    		exit(-1);
    	}
    	
    	fflush(stdout);

    	if(FD_ISSET(listener,&master)){ //Todo: use copy
    		printf("Detected connection\n");
    		fflush(stdout);
    		addrlen = sizeof remoteSocketAddress;
    		int new_socket = accept(listener,(struct sockaddr *)&remoteSocketAddress,&addrlen);
    		FD_SET(new_socket,&master);
    		if(new_socket>maxFD){
    			maxFD = new_socket;
    		}
    		printf("New connection from %d:%d on socket %d\n",
                            inet_ntoa(remoteSocketAddress.sin_addr),
                        	ntohs(remoteSocketAddress.sin_port), new_socket);
    		fflush(stdout);
    		
    		write_buf[write_buf_itr++] = 0xCF;
    		write_buf[write_buf_itr++] = 0xA7;
    		write_buf[write_buf_itr++] = client_count & 0xFF;
    		write_buf[write_buf_itr++] = client_count >> 8;
    		
    		for(i =0;i<client_count;i++){
	    		int name_length = strlen(clients[i].name);
    			write_buf[write_buf_itr++] = name_length;//length of the name
    			for(j=0;j<name_length;j++){
    				write_buf[write_buf_itr++] = clients[i].name[j];
    			}
    		}
    		nbytes = write_buf_itr;
    		if(send(new_socket,write_buf,nbytes,0)== -1){
    			fprintf(f, "%s\n", "Error on send handshake");
    			exit(-1);
    		}

    		struct client new_client;
    		read_buf_itr=0;
    		write_buf_itr =0;

    		int buf_size = read(new_socket,&read_buf,256);
    		int new_name_length = read_buf[0];
    		while(read_buf_itr++<buf_size){
    			new_client.name[write_buf_itr++] = read_buf[read_buf_itr];
    		}
    		new_client.name[write_buf_itr] = '\0'; //Might be unneccessary?
    		new_client.socket_id = new_socket;
    		clients[client_count] = new_client;
    		client_count++;
    		printf("A new client joined: %s\n", new_client.name);
    	}
	}


	return NULL;
}