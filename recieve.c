#include "server.h"
void* Recieve(void* input)
{
	fprintf(f,"Recieving thread started\n");
	fflush(f);
	fd_set copy_master;

	while(1){
		int read_buf_itr=0;
		int write_buf_itr=0;
		copy_master = master;
		int i;
		unsigned char buf[65535];
		int size_recieved;
		struct timeval timeoutConfig;
		timeoutConfig.tv_sec =0;
		timeoutConfig.tv_usec =500;

		
		int select_result =select(maxFD+1,&copy_master,NULL,NULL,&timeoutConfig);
		if(select_result == -1){
			fprintf(f,"Select statement in recieve thread failed");
			exit(-1);
		}
		for(i = 0;i<client_count;i++){
			int client_id = clients[i].socket_id; // Todo: mutex
			if(FD_ISSET(client_id,&copy_master)){
				fflush(stdout);
				size_recieved = read(client_id,&buf,65535 );
				int message_length = (buf[0] & 0xFF)+(buf[1] >> 8);
				if(message_length !=0 ){					
					for(read_buf_itr=0;read_buf_itr<message_length;read_buf_itr++){
						clients[i].buf[write_buf_itr++] = buf[read_buf_itr+2];
					}
						clients[i].buf[message_length+1] = '\0';
						printf("%s: %s\n",clients[i].name,clients[i].buf);
						fflush(stdout);
				}else{ // Is a keep-alive message

				}
			}
		}	
		
	}

}