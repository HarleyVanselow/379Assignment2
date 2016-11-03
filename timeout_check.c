#include "server.h"
void* TimeoutCheck(int timeout)
{
    while(1)
    {
        sleep(1);
        int i;
        
        sem_wait(&lock_client);
            for(i=0;i<client_count;i++){
                if(clients[i].time_since_last_received > timeout){
                    terminate(i);
                }else{
                    clients[i].time_since_last_received++;
                }
            }
	    sem_post(&lock_client);
    }

}