#include "server.h"
void* TimeoutCheck()
{
    while(1)
    {
        if(server_exit){pthread_exit(NULL);}
        sleep(1);
        int i;
        
            for(i=0;i<client_count;i++){
                sem_wait(&lock_client);
                if(clients[i].time_since_last_received > 30){
	                sem_post(&lock_client);
                    terminate(i);
                }else{
                    clients[i].time_since_last_received++;
                    sem_post(&lock_client);
                }
            }
    }

}