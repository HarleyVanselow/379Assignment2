#include "server.h"
void* Send(void* input)
{
	fprintf(f,"Sending thread started\n");
	fflush(f);
	int i,j;
	while(1){
		unsigned char* write_buf[256];
		int write_buf_itr=0;
		int read_buf_itr=0;

		for(j=0; j<client_count;j++){
			for(i =0; i<client_count;i++){
				if(clients[i].buf[0] != 0){
					int sender_name_length = strlen(clients[i].name);
					write_buf[write_buf_itr++]=0x00;
					write_buf[write_buf_itr++]=sender_name_length;
					// for(read_buf_itr=0;read_buf_itr<)

					send(clients[j].socket_id,clients[i].buf,256,0);//Shouldnt really be 256
					memset(clients[i].buf,0,256);
				}
				
			}			
		}
	}
}