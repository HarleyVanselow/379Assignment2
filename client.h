#ifndef CLIENT_METHODS
#define CLIENT_METHODS

typedef struct entry {
   TAILQ_ENTRY(entry) entries;
   char * username;        
} entry;

#ifndef FIRST_HANDSHAKE_BYTE
#define FIRST_HANDSHAKE_BYTE 0xCF
#endif

#ifndef SECOND_HANDSHAKE_BYTE
#define SECOND_HANDSHAKE_BYTE 0xA7
#endif

#ifndef UPDATE_MESSAGE
#define UPDATE_MESSAGE 0x00
#endif

#ifndef USER_JOINED
#define USER_JOINED 0x01
#endif

#ifndef USER_QUIT
#define USER_QUIT 0x02
#endif

void send_keep_alive_message(int sig);
void close_client(int sig);

void send_username_to_server(const char * username);
void setupSigIntHandlers();
void check_connection();

void * read_user_input();
void * handle_received_message();

void handle_client_quit();
void handle_client_join();
void handle_message();

#endif