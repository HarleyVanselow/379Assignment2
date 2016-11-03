#include "server.h"
void* TimeoutCheck(int timeout)
{
    while(1)
    {
        sleep(1);
        int i;
        
            for(i=0;i<client_count;i++){
                sem_wait(&lock_client);
                if(clients[i].time_since_last_received > timeout){
	                sem_post(&lock_client);
                    terminate(i);
                }else{
                    clients[i].time_since_last_received++;
                    sem_post(&lock_client);
                }
            }
    }

}