#include "server.h"
void send_client_buffer(int i) //This method already has clients locked for it
{
	unsigned char write_buf[256];
	int write_buf_itr=0;
	int read_buf_itr=0;
	int j;
	
	
	unsigned char sender_name_length = strlen(clients[i].name);
	short message_length = strlen(clients[i].buf);

	write_buf[write_buf_itr++]=0x00;
	write_buf[write_buf_itr++]=sender_name_length; //Must be 1 byte
	for(j=0;j<sender_name_length;j++){ // Write client name to outgoing buffer
		write_buf[write_buf_itr++] = clients[i].name[j];
	}
	write_buf[write_buf_itr++] = message_length & 0xFF;
	write_buf[write_buf_itr++] = message_length >> 8;

	
	for(j=0;j<message_length;j++){
		write_buf[write_buf_itr++] = clients[i].buf[j];
	}
	
	for(j=0;j<client_count;j++){
		printf("Sending to: %d:%s\n",j,clients[j].name);
		send(clients[j].socket_id,write_buf,write_buf_itr,0);	
	}

	memset(clients[i].buf,0,256);
}
void send_client_change_notice(char* name,int joined_or_left) // Already client locked
{
	unsigned char write_buf[256];
	int write_buf_itr=0;
	int j;
	char name_length = strlen(name);
	write_buf[write_buf_itr++] = joined_or_left;
	write_buf[write_buf_itr++] = name_length;
	for(j=0;j<name_length;j++){
		write_buf[write_buf_itr++] = name[j];
	}
	
	for(j =0; j<client_count;j++){
		send(clients[j].socket_id,write_buf,write_buf_itr,0);
	}	
	
}

void* Send()
{
	fprintf(f,"Sending thread started\n");
	fflush(f);
	int i;
	while(1){
		for(i =0; i<client_count;i++){
			sem_wait(&lock_client);
				if(clients[i].buf[0] != 0){ //todo better check
					send_client_buffer(i);
				}
	    	sem_post(&lock_client);
		}			
		
	}
}