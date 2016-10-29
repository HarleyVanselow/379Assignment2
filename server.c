#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <pthread.h>

FILE *f;
int MY_PORT;
fd_set master;
void* Accept(void* input)
{
	fprintf(f, "Accept thread started\n");
	struct sockaddr_in socketAddress;
	struct sockaddr_in remoteSocketAddress;
	socklen_t addrlen;
	int listener;

	//Set up listener socket
	listener = socket(AF_INET, SOCK_STREAM, 0);
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    socketAddress.sin_port = htons(MY_PORT);

    int bindResult = bind(listener,(struct sockaddr *)&socketAddress, sizeof(socketAddress));
    if(bindResult <0){
    	fprintf(f,"Error binding. Terminating...");
    	exit(-1);
    }else{
    	printf("Binding successful. Opened socket on port: %d\n",MY_PORT);
    	fflush(stdout);
    }
    
    if(listen(listener,50) == -1){ //To many queued?
    	exit(-1);
    }

    FD_SET(listener,&master); //Todo: mutex

    while(1){
    	printf("Started loop %d\n",listener);
    	fflush(stdout);
    	if(select(listener+1,&master,NULL,NULL,NULL) == -1){
    		exit(-1);
    	}
    	
    	printf("Made it past the select statement\n");
    	fflush(stdout);
    	if(FD_ISSET(0,&master)){ //Todo: use copy
    		printf("Detected connection\n");
    		fflush(stdout);
    		addrlen = sizeof remoteSocketAddress;
    		int new_socket = accept(listener,(struct sockaddr *)&remoteSocketAddress,&addrlen);
    		fprintf(f,"New connection from %d:%d on socket %d\n",
                            inet_ntoa(remoteSocketAddress.sin_addr),
                        	ntohs(remoteSocketAddress.sin_port), new_socket);
    	}
    }

	return NULL;
}
int main(int argc, char const *argv[])
{
	// daemon(1,1);
	char filename[30];
	sscanf(argv[1],"%d",&MY_PORT);
	sprintf(filename,"server379%d.log",getpid());//Todo: validate port # input
	f = fopen(filename,"a+");
	fprintf(f, "Server started on port %s\n",argv[1] );

	pthread_t threads[1];
	int thread_result = pthread_create(threads,NULL,Accept,NULL);
	thread_result = pthread_join(threads[0],NULL);
	


 	fprintf(f, "Exited\n");
	/* code */
	return 0;
}

