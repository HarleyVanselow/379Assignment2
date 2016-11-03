#include "server.h"

FILE *f;
int MY_PORT;
fd_set master;
short client_count=0;
sem_t lock_client;
sem_t lock_master;
int main(int argc, char const *argv[])
{
	sem_init(&lock_master,0,1);
	sem_init(&lock_client,0,1);
	// daemon(1,1);

	if (argc != 2){
		printf("Must specify port number\n");
		return -1;
	}
	printf("Started server\n");
	sscanf(argv[1],"%d",&MY_PORT);
	char filename[30];
	sprintf(filename,"server379%d.log",getpid());//Todo: validate port # input
	f = fopen(filename,"a+");
	fprintf(f, "Server started on port %s\n",argv[1] );
	fflush(f);
	pthread_t threads[4];
	pthread_create(&threads[0],NULL,Accept,NULL);
	pthread_create(&threads[1],NULL,Receive,NULL);
	pthread_create(&threads[2],NULL,Send,NULL);
	pthread_create(&threads[3],NULL,TimeoutCheck(30),NULL);
	pthread_join(threads[0],NULL);
	

 	fprintf(f, "Exited\n");
	
	return 0;
}

