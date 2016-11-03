#include "server.h"

FILE *f;
int MY_PORT;
fd_set master;
short client_count=0;
sem_t lock_client;
sem_t lock_master;
int server_exit = 0;
int main(int argc, char const *argv[])
{
	// daemon(1,1);

	signal(SIGTERM,terminate_server);
	printf("PID: %d\n",getpid());
	sem_init(&lock_master,0,1);
	sem_init(&lock_client,0,1);

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
	printf("Thread 0 created\n");
	fflush(stdout);
	pthread_create(&threads[1],NULL,Receive,NULL);
	printf("Thread 1 created\n");
	fflush(stdout);
	pthread_create(&threads[2],NULL,Send,NULL);
	printf("Thread 2 created\n");
	fflush(stdout);
	pthread_create(&threads[3],NULL,TimeoutCheck,NULL);
	printf("Thread 3 created\n");
	fflush(stdout);
	// All threads must be done with before the main returns
	pthread_join(threads[0],NULL);
	printf("Thread 0 done\n");
	fflush(stdout);
	pthread_join(threads[1],NULL);
	printf("Thread 1 done\n");
	fflush(stdout);
	pthread_join(threads[2],NULL);
	printf("Thread 2 done\n");
	fflush(stdout);
	pthread_join(threads[3],NULL);
	printf("Thread 3 done\n");
	fflush(stdout);
	//Close all sockets
	printf("Sockets to terminate: %d",client_count); // TODO: terminate listener as well
	int i;
	for(i=0;i<client_count;i++)
	{
		terminate(i);
	}

 	fprintf(f, "Exited\n");
	fflush(f);
	return 0;
}

