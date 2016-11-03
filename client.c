#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/queue.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <inttypes.h>

int number_of_connected_users;
int client_socket;

typedef struct entry {
 TAILQ_ENTRY(entry) entries;
 char * username;        
} entry;

TAILQ_HEAD(tailhead, entry) head;
struct tailhead *headp;                    
entry *np;

void * check_connection(unsigned char * buf){
    if (buf[0] == 207 && buf[1] == 167){
        printf("Connection Established!\n");
        fflush(stdout); 
    } else {
        printf("Connection closed by server\n");
        fflush(stdout); 
        exit(-1);
    }
}

void * read_user_input(void * client_socket){

    char message[255];

    while(1){
        // This is blocking -> should make this non-blocking
        fgets(message, 255, stdin);
        // int i;
        // for (i = 0; i < strlen(message); i ++){
        //     printf("%d\n", message[i]);fflush(stdout);
        // }
        
        if (strcmp(message, ".users\n") == 0){
            printf("Current users: \n");
            for (np = head.tqh_first; np != NULL; np = np->entries.tqe_next){
                printf("%s\n", np->username);fflush(stdout);
            }

        } else {
            char message_to_send[256];
            uint16_t message_length= strlen(message);
            snprintf(message_to_send, sizeof message_to_send, "%c%c%s", message_length & 0xFF, message_length >> 8, message);
            // printf("sending message: %s\n", message_to_send);

            // printf("Sending: %s", message_to_send); fflush(stdout);
            send(*((int *)client_socket),message_to_send,256,0);//Shouldnt really be 256

            memset(message_to_send,0,256);
        }
    }
}


void * received_messages(void * client_socket){
    unsigned char message_type;
    int username_length;
    uint16_t message_length;
    char buf[256];
    while (1){

        //wait until we get something
        read (*((int *)client_socket), &message_type, 1);
            // printf("message_type: %c\n", message_type); fflush(stdout);
        if (message_type == 0x00) {

            read(*((int *)client_socket), &username_length, 1);
            char sender_username[username_length];
            read(*((int *)client_socket), &sender_username, username_length);
            sender_username[username_length] = '\0';

            read(*((int *)client_socket), &message_length, 2);
            char received_message[message_length];
            read(*((int *)client_socket), &received_message, message_length);
            received_message[message_length] = '\0';

            printf("%s: %s\n", sender_username, received_message);
            fflush(stdout);
        } else if (message_type == 0x01) {
            read(*((int *)client_socket), &username_length, 1);
            char * updated_username = malloc (username_length+1);
            read(*((int *)client_socket), updated_username, username_length);
            updated_username[username_length] = '\0';
            printf("%s has joined the chat\n", updated_username);
            fflush(stdout);

            entry * new_entry = (entry *)malloc(sizeof(struct entry));
            // printf("5\n"); fflush(stdout);
            new_entry->username = updated_username;
            // printf("6\n"); fflush(stdout);

            TAILQ_INSERT_TAIL(&head, new_entry, entries);
            free(new_entry);
        } else if (message_type == 0x02) {
            read(*((int *)client_socket), &username_length, 1);
            char * updated_username = malloc(username_length+1);
            read(*((int *)client_socket), updated_username, username_length);
            updated_username[username_length] = '\0';
            printf("%s has left the chat\n", updated_username);
            fflush(stdout);
            for (np = head.tqh_first; np != NULL; np = np->entries.tqe_next){
                // printf("%s\n", np->username);fflush(stdout);
                printf("checking username: %s\n", np->username);
                if (strcmp(np->username, updated_username) == 0){
                    printf("removing node\n");
                    TAILQ_REMOVE(&head, np, entries);
                    free(np);
                }
            }
        }
        message_type = 0;
        username_length = 0;
        message_length = 0;
        
    }
}

void * send_username(void * client_socket, const char * username){
    char message_to_send[256];
    uint8_t username_length = strlen(username);
    snprintf(message_to_send, sizeof message_to_send, "%c%s", username_length, username);
    // printf("Your Username: %s\n",message_to_send );
        send(*((int *)client_socket),message_to_send,256,0);//Shouldnt really be 256
    }

void close_client(int sig){
    printf("Closing client\n");fflush(stdout);
    close(client_socket);
    shutdown(client_socket, 2);
    exit(1);
}


int main(int argc, char const *argv[]){
        if (argc != 4){
            printf("Invalid parameters!\n");
            return -1;
        }

        const char * hostname = argv[1];
        int port_number = atoi(argv[2]);
        const char * username = argv[3];

        struct sigaction onSigInt;
        onSigInt.sa_handler = close_client;
        sigemptyset(&onSigInt.sa_mask);
        onSigInt.sa_flags = 1;

        sigaction(SIGINT, &onSigInt, NULL);

        uint32_t number;

        unsigned char buf[256];

        struct  sockaddr_in server;

        struct  hostent     *host;

        TAILQ_INIT(&head);    

        host = gethostbyname (hostname);

        if (host == NULL) {
            perror ("Client: cannot get host description");
            exit (1);
        }


        client_socket = socket (AF_INET, SOCK_STREAM, 0);

        if (client_socket < 0) {
            perror ("Client: cannot open socket");
            exit (1);
        }

        bzero (&server, sizeof (server));
        bcopy (host->h_addr, & (server.sin_addr), host->h_length);
        server.sin_family = host->h_addrtype;
        server.sin_port = htons (port_number);

        if (connect (client_socket, (struct sockaddr*) & server, sizeof (server))) {
            perror ("Client: cannot connect to server");
            exit (1);
        }

        read (client_socket, &buf, 2);
        check_connection(buf);
        uint16_t number_of_connected_users;
        read (client_socket, &number_of_connected_users, 2);
        printf("There are already %d users connected!\n", number_of_connected_users);
        fflush(stdout);
    // Now get the list of usernames 
        int counter = 0;
        for (counter; counter < number_of_connected_users; counter++){
            int username_length;
            read(client_socket, &username_length, 1);

            // printf("1: username_length: %d\n", username_length); fflush(stdout);

            char * current_username = malloc(username_length+1);
            // printf("2\n"); fflush(stdout);
            read(client_socket, current_username, username_length);
            // printf("3:\n"); fflush(stdout);
            current_username[username_length] = '\0';
            // printf("4\n"); fflush(stdout);

            entry * new_entry = (entry *)malloc(sizeof(struct entry));
            // printf("5\n"); fflush(stdout);
            new_entry->username = current_username;
            // printf("6\n"); fflush(stdout);

            TAILQ_INSERT_TAIL(&head, new_entry, entries);
            free(new_entry);
            printf("Adding username: %s\n", current_username); fflush(stdout);
            printf("Adding username: %s\n", current_username); fflush(stdout);
        } 
        printf("Current users: \n"); fflush(stdout);

 		for (np = head.tqh_first; np != NULL; np = np->entries.tqe_next){
                printf("%s\n", np->username); fflush(stdout);
        }
        send_username(&client_socket, username);

        send(client_socket,"0",1,0);//Shouldnt really be 256
        // read (client_socket, &buf, 256);

        pthread_t user_input;
        pthread_create(&user_input, NULL, read_user_input, &client_socket);

        pthread_t received_message;
        pthread_create(&received_message, NULL, received_messages, &client_socket);
        
        while(1);
}