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

FILE *f;
int MY_PORT;
fd_set master;
short client_count=0;
int main(int argc, char const *argv[])
{
	// daemon(1,1);
	printf("Started server\n");
	sscanf(argv[1],"%d",&MY_PORT);
	char filename[30];
	sprintf(filename,"server379%d.log",getpid());//Todo: validate port # input
	f = fopen(filename,"a+");
	fprintf(f, "Server started on port %s\n",argv[1] );
	fflush(f);
	pthread_t threads[3];
	pthread_create(&threads[0],NULL,Accept,NULL);
	pthread_create(&threads[1],NULL,Recieve,NULL);
	pthread_create(&threads[2],NULL,Send,NULL);
	pthread_join(threads[0],NULL);
	

 	fprintf(f, "Exited\n");
	/* code */
	return 0;
}

