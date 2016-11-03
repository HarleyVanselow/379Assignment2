#include "server.h"
#include <errno.h>
void terminate(int i)
{
	int j;

	sem_wait(&lock_client);
	sem_wait(&lock_master);
		close(clients[i].socket_id);
		FD_CLR(clients[i].socket_id,&master);
	sem_post(&lock_master);

	printf("%s terminated, notifying...\n",clients[i].name);
	send_client_change_notice(clients[i].name,2);
	if(!(i+1 == client_count)){//Isn't end element
		for(j=i;j<client_count-1;j++){
			clients[j] = clients[j+1];
		} 
	}
	client_count--;
	sem_post(&lock_client);

	sem_wait(&lock_master);
	for(j=0;j<client_count;j++){ // Update new maxFD
		if(clients[j].socket_id>maxFD){
			maxFD=clients[j].socket_id; 
		}
	}
	sem_post(&lock_master);
}

void terminate_server()
{
	printf("Sending exit signal\n");
	fflush(stdout);
	server_exit = 1;
}