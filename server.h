#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <pthread.h>
#include <string.h>
#ifndef CLIENT
#define CLIENT
struct client
{
	int socket_id;
	unsigned char name[256];
	unsigned char buf[65535]; //Can hold largest possible message	
}clients[50];
#endif

extern FILE *f;
extern int MY_PORT;
extern fd_set master;
extern maxFD;
extern short client_count;

void* Accept(void* input);
void* Recieve(void* input);
void* Send(void* input);
void send_client_change_notice(char* name, int joined_or_left);