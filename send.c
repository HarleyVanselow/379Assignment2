#include "server.h"
void* Send(void* input)
{
	fprintf(f,"Sending thread started\n");
	fflush(f);
	int i,j,k;
	while(1){
		unsigned char write_buf[256];
		int write_buf_itr=0;
		int read_buf_itr=0;

		for(i =0; i<client_count;i++){
			if(clients[i].buf[0] != 0){
				unsigned char sender_name_length = strlen(clients[i].name);
				short message_length = strlen(clients[i].buf);

				write_buf[write_buf_itr++]=0x00;
				write_buf[write_buf_itr++]=sender_name_length; //Must be 1 byte
				for(k=0;k<sender_name_length;k++){ // Write client name to outgoing buffer
					write_buf[write_buf_itr++] = clients[i].name[k];
				}
				write_buf[write_buf_itr++] = message_length & 0xFF;
				write_buf[write_buf_itr++] = message_length >> 8;

				
				for(k=0;k<message_length;k++){
					write_buf[write_buf_itr++] = clients[i].buf[k];
				}
				
				for(j=0;j<client_count;j++){
					printf("Sending to: %d:%s\n",j,clients[j].name);
					send(clients[j].socket_id,write_buf,write_buf_itr,0);	
				}
				memset(clients[i].buf,0,256);
			}
		}			
		
	}
}