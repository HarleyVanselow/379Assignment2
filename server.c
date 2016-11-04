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
	int daemon_result = daemon(1,1);
	signal(SIGTERM,terminate_server);
	char filename[30];
	sprintf(filename,"server379%d.log",getpid());//Todo: validate port # input
	f = fopen(filename,"a+");

	if (argc != 2){
		fprintf(f,"%s\n","Must specify port number");
		return -1;
	}
	if(daemon_result<0){
		fprintf(f, "%s\n", "Daemon failed to start, terminating...");
		return -1;
	}
	sem_init(&lock_master,0,1);
	sem_init(&lock_client,0,1);
	
	sscanf(argv[1],"%d",&MY_PORT);

	fprintf(f, "Server started on port %s\n",argv[1] );
	
	pthread_t threads[4];
	pthread_create(&threads[0],NULL,Accept,NULL);

	
	pthread_create(&threads[1],NULL,Receive,NULL);

	
	pthread_create(&threads[2],NULL,Send,NULL);

	
	pthread_create(&threads[3],NULL,TimeoutCheck,NULL);

	
	// All threads must be done with before the main returns
	pthread_join(threads[0],NULL);
	fprintf(f,"%s\n","Accept thread done");

	pthread_join(threads[1],NULL);
	fprintf(f,"%s\n","Receive thread done");

	pthread_join(threads[2],NULL);
	fprintf(f,"%s\n","Send thread done");

	pthread_join(threads[3],NULL);
	fprintf(f,"%s\n","TimeoutCheck thread done");

	//Close all sockets
	// printf("Sockets to terminate: %d",client_count); // TODO: terminate listener as well
	int i;
	for(i=0;i<client_count;i++)
	{
		terminate(i);
	}

 	fprintf(f, "Terminating...\n");
	fflush(f);
	return 0;
}

