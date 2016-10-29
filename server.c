#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
	int MY_PORT = strtol(argv[1],NULL,10);
	printf("%s\n",argv[1]);
	/* code */
	return 0;
}