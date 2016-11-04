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

#define UPDATE_MESSAGE 0x00
#define USER_JOINED 0x01
#define USER_QUIT 0x02

int number_of_connected_users;
int client_socket;

pthread_t user_input;
pthread_t received_message;

typedef struct entry {
   TAILQ_ENTRY(entry) entries;
   char * username;        
} entry;

TAILQ_HEAD(tailhead, entry) head;
struct tailhead *headp;                    
entry *np;

void close_client(int sig);

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

void send_keep_alive_message(int sig){
    alarm(25);
    uint16_t keep_alive_message = htons(0);
    send(client_socket, &keep_alive_message,2,0);
}

void * read_user_input(void * client_socket){

    char message[255];

    while(1){
        fgets(message, 255, stdin);
        
        if (strcmp(message, ".users\n") == 0){
            np = head.tqh_first;
            if (np != NULL){
                printf("Current users: \n");
                for (np = head.tqh_first; np != NULL; np = np->entries.tqe_next){
                    printf("%s\n", np->username);fflush(stdout);
                }
            } else {
                printf("There is nobody else here!\n");fflush(stdout);
            }

        } else {
            char message_to_send[256];
            uint16_t message_length= strlen(message);
            snprintf(message_to_send, sizeof message_to_send, "%c%c%s", message_length & 0xFF, (message_length >> 8) & 0xFF, message);

            alarm(25);
            send(*((int *)client_socket),message_to_send,message_length+1,0);//Shouldnt really be 256

            memset(message_to_send,0,256);
        }
    }
}


void * received_messages(void * client_socket){
    unsigned char message_type;
    uint8_t username_length;
    uint16_t message_length;
    while (1){

        //wait until we get something
       int message_received = read (*((int *)client_socket), &message_type, 1);
       if (message_received <= 0){
        close_client(0);    
       }

        if (message_type == UPDATE_MESSAGE) {
            read(*((int *)client_socket), &username_length, 1);
            char sender_username[username_length+1];
            read(*((int *)client_socket), &sender_username, username_length);
            sender_username[username_length] = '\0';

            read(*((int *)client_socket), &message_length, 2);
            message_length = htons(message_length);
            char received_message[message_length+1];
            printf("message_length: %d\n", message_length); fflush(stdout);
            read(*((int *)client_socket), &received_message, message_length);
            received_message[message_length] = '\0';

            printf("%s: %s\n", sender_username, received_message);
            fflush(stdout);
        } else if (message_type == USER_JOINED) {
            read(*((int *)client_socket), &username_length, 1);
            char * updated_username = malloc (username_length+1);
            read(*((int *)client_socket), updated_username, username_length);
            updated_username[username_length] = '\0';
            printf("%s has joined the chat\n", updated_username);
            fflush(stdout);

            entry * new_entry = (entry *)malloc(sizeof(struct entry));
            new_entry->username = updated_username;
            TAILQ_INSERT_TAIL(&head, new_entry, entries);
        } else if (message_type == USER_QUIT) {
            read(*((int *)client_socket), &username_length, 1);
            char updated_username[username_length + 1];// = malloc(username_length+1);
            read(*((int *)client_socket), updated_username, username_length);
            updated_username[username_length] = '\0';
            printf("%s has left the chat\n", updated_username);
            fflush(stdout);
            entry * current_node;
            for (np = head.tqh_first; np != NULL; np = np->entries.tqe_next){
                if (strcmp(np->username, updated_username) == 0){
                    TAILQ_REMOVE(&head, np, entries);
                    break;
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
    send(*((int *)client_socket),message_to_send,username_length+1,0);//Shouldnt really be 256
}

void close_client(int sig){
    printf("Closing client\n");fflush(stdout);
    close(client_socket);
    shutdown(client_socket, 2);
    // pthread_exit(user_input);
    // pthread_exit(received_message);
    exit(1);
}

void setupSigIntHandlers(){
    struct sigaction onSigInt;
    onSigInt.sa_handler = close_client;
    sigemptyset(&onSigInt.sa_mask);
    onSigInt.sa_flags = 1;

    sigaction(SIGINT, &onSigInt, NULL);

    struct sigaction onSigAlarm;
    onSigAlarm.sa_handler = send_keep_alive_message;
    sigemptyset(&onSigAlarm.sa_mask);
    onSigAlarm.sa_flags = 1;

    sigaction(SIGALRM, &onSigAlarm, NULL);
}


int main(int argc, char const *argv[]){
    if (argc != 4){
        printf("Invalid parameters!\n");
        return -1;
    }

    const char * hostname = argv[1];
    int port_number = atoi(argv[2]);
    const char * username = argv[3];

    setupSigIntHandlers();
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
    uint8_t number_of_connected_users;
    read (client_socket, &number_of_connected_users, 2);
    if (number_of_connected_users != 0){
        printf("There are already %d users connected!\n", number_of_connected_users);
    }
    fflush(stdout);
    
        // Now get the list of usernames 
    int counter = 0;
    uint8_t username_length;
    for (counter; counter < number_of_connected_users; counter++){
        
        read(client_socket, &username_length, 1);
        printf("username_length: %c\n", username_length);
        char * updated_username = malloc (username_length+1);
        read(client_socket, updated_username, username_length);
        updated_username[username_length] = '\0';
        printf("%s has joined the chat\n", updated_username);
        fflush(stdout);

        entry * new_entry = (entry *)malloc(sizeof(struct entry));
        new_entry->username = updated_username;
        TAILQ_INSERT_TAIL(&head, new_entry, entries);
    } 

    if (counter != 0){
        printf("Current users: \n"); fflush(stdout);

        for (np = head.tqh_first; np != NULL; np = np->entries.tqe_next){
            printf("%s\n", np->username); fflush(stdout);
        }
    } else {
       printf("There is nobody here yet!\n"); fflush(stdout);
   }
   send_username(&client_socket, username);

   pthread_create(&user_input, NULL, read_user_input, &client_socket);

   pthread_create(&received_message, NULL, received_messages, &client_socket);
   alarm(25);
   while(1);
}
