#include "server.h"
#include <errno.h>
void terminate(int i)
{
	int j;
	printf("Connection terminated\n");
	close(clients[i].socket_id);
	send_client_change_notice(clients[i].name,2);
	FD_CLR(clients[i].socket_id,&master);

	
	if(!(i+1 == client_count)){//Isn't end element
		for(j=i;j<client_count-1;j++){
			clients[j] = clients[j+1];
		} 
	}
	client_count--;

	for(j=0;j<client_count;j++){ // Update new maxFD
		if(clients[j].socket_id>maxFD){
			maxFD=clients[j].socket_id; 
		}
	}
	
	
	
}