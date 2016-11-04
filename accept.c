#include "server.h"
    int maxFD;
void* Accept()
{
    fprintf(f, "Accept thread started\n");
    fflush(f);
    struct sockaddr_in socketAddress;
    struct sockaddr_in remoteSocketAddress;
    socklen_t addrlen;
    int listener;
    unsigned char write_buf[256];
    unsigned char read_buf[256];
    int nbytes =0;
    int i=0; //General use iterator
    int j=0;
    int write_buf_itr =0;
    int read_buf_itr =0;
    fd_set copy_master;

    //Set up listener socket
    listener = socket(AF_INET, SOCK_STREAM, 0);
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_addr.s_addr =INADDR_ANY;// inet_addr("127.0.0.1");
    socketAddress.sin_port = htons(MY_PORT);

    int bindResult = bind(listener,(struct sockaddr *)&socketAddress, sizeof(socketAddress));
    if(bindResult <0){
        fprintf(f,"Error binding. Terminating...");
        exit(-1);
    }else{
        fprintf(f,"Binding successful. Opened socket on port: %d\n",MY_PORT);
    }
    maxFD = listener;
    
    if(listen(listener,50) == -1){ //To many queued?
        exit(-1);
    }

    FD_SET(listener,&master); //Todo: mutex
    struct timeval timeoutConfig;
    timeoutConfig.tv_sec =0;
    timeoutConfig.tv_usec =500;
    while(1){
        if(server_exit){pthread_exit(NULL);}
        //Re initialize all buffer handlers
        sem_wait(&lock_master);
            copy_master = master;
        sem_post(&lock_master);

        memset(&write_buf,0,256);
        memset(&read_buf,0,256);
        write_buf_itr =0;
        read_buf_itr=0;

        if(select(listener+1,&copy_master,NULL,NULL,&timeoutConfig) == -1){
            exit(-1);
        }
        
        if(FD_ISSET(listener,&copy_master)){
            fprintf(f,"Detected connection\n");
            addrlen = sizeof remoteSocketAddress;
            int new_socket = accept(listener,(struct sockaddr *)&remoteSocketAddress,&addrlen);
            FD_SET(new_socket,&master);
            if(new_socket>maxFD){
                maxFD = new_socket;
            }           
            write_buf[write_buf_itr++] = 0xCF;
            write_buf[write_buf_itr++] = 0xA7;
            write_buf[write_buf_itr++] = client_count & 0xFF;
            write_buf[write_buf_itr++] = client_count >> 8;
            
            sem_wait(&lock_client);
                for(i =0;i<client_count;i++){
                        int name_length = strlen(clients[i].name);
                    write_buf[write_buf_itr++] = name_length;//length of the name
                    for(j=0;j<name_length;j++){
                        write_buf[write_buf_itr++] = clients[i].name[j];
                    }
                }
            sem_post(&lock_client);
            nbytes = write_buf_itr;
            if(send(new_socket,write_buf,nbytes,0)== -1){
                fprintf(f, "%s\n", "Error on send handshake");
                exit(-1);
            }

            struct client new_client;
            read_buf_itr=0;
            write_buf_itr =0;

            int buf_size = read(new_socket,&read_buf,256);
            int new_name_length = read_buf[0];
            while(read_buf_itr++<buf_size){
                new_client.name[write_buf_itr++] = read_buf[read_buf_itr];
            }
            new_client.name[write_buf_itr] = '\0';
            new_client.socket_id = new_socket;

            sem_wait(&lock_client);
            int counter = 0;
            int valid_client = 1;
            fprintf(f, "current_clients: %d\n", client_count);
            for (counter; counter < client_count; counter++){
                if (strncmp(new_client.name, clients[counter].name, strlen(new_client.name)) == 0){
                    fprintf(f,"Rejected client with duplicate name: %s\n", new_client.name);
                    valid_client = 0;
                    sem_wait(&lock_master);
                        close(new_client.socket_id);
                        FD_CLR(new_client.socket_id,&master);
                    sem_post(&lock_master);
                    break;
                }
            }
            sem_post(&lock_client);
            if (valid_client == 1){
                fprintf(f,"A new client joined: %s, ID: %d\n", new_client.name,new_socket);
                sem_wait(&lock_client);
                    clients[client_count] = new_client;
                    client_count++;
                    send_client_change_notice(new_client.name,1);
                sem_post(&lock_client);
            }
        }
    }
}