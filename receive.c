#include "server.h"
#include <errno.h>

void* Receive()
{
	fprintf(f,"Recieving thread started\n");
	fd_set copy_master;
	unsigned char buf[65535];
	int size_received;
	struct timeval timeoutConfig;
	int i,j;
	while(1){
		if(server_exit){pthread_exit(NULL);}
		memset(buf,0,65535);
		int read_buf_itr=0;
		int write_buf_itr=0;
		copy_master = master;
		timeoutConfig.tv_sec =0;
		timeoutConfig.tv_usec =500;
		
		int select_result =select(maxFD+1,&copy_master,NULL,NULL,&timeoutConfig);
		
		if(select_result == -1){
			fprintf(f,"Select statement in receive thread failed with errno: %d",errno);
			exit(-1);
		}
		for(i = 0;i<client_count;i++){
			sem_wait(&lock_client);
				int client_id = clients[i].socket_id;
	    	sem_post(&lock_client);
			if(FD_ISSET(client_id,&copy_master)){

				uint16_t message_length;
				read(client_id, & message_length, 2);
				message_length = ntohs(message_length);

				size_received = read(client_id,&buf,65535);
				if(size_received < 1){
					terminate(i);
					break;
				}
				// int message_length = (buf[0] & 0xFF)+(buf[1] >> 8);
				sem_wait(&lock_client);
					if(message_length !=0 ){					
						for(read_buf_itr=0;read_buf_itr<message_length;read_buf_itr++){
							clients[i].buf[write_buf_itr++] = buf[read_buf_itr];
						}
						clients[i].buf[message_length+1] = '\0';
						fprintf(f,"Received %s from %s\n",clients[i].buf,clients[i].name);
					}
					clients[i].time_since_last_received=0;
	    		sem_post(&lock_client);
			}
		}			
	}

}