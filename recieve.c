#include "server.h"
void* Recieve(void* input)
{
	fprintf(f,"Recieving thread started\n");
	fflush(f);
	fd_set copy_master;
	while(1){
		copy_master = master;
		int i;
		unsigned char buf[256];
		int size_recieved;
		struct timeval timeoutConfig;
		timeoutConfig.tv_sec =4;
		timeoutConfig.tv_usec =0;

		for(i = 0;i<client_count;i++){

			int client_id = clients[i].socket_id; // Todo: mutex
			int select_result =select(client_id,&copy_master,NULL,NULL,&timeoutConfig);
			if(select_result == -1){
				fprintf(f,"Select statement in recieve thread failed");
				exit(-1);
			}
			printf("Username: %s, ClientId: %d select_result: %d\n",clients[i].name,client_id,select_result);
			if(FD_ISSET(client_id,&copy_master)){
				printf("Ready to read\n" );
				fflush(stdout);
				size_recieved = read(client_id,&buf,256);
				printf("%s\n",buf);
				fflush(stdout);
			}else{
				printf("failed\n");
				fflush(stdout);
			}
		}
	}

}