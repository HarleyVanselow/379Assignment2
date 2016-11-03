all: server client
	
server:     
	gcc -pthread server.c  accept.c  receive.c send.c timeout_check.c -o server379

client:
	gcc -pthread client.c -o chat379
	
clean:
	rm server379
	rm chat379
	rm -f *.log

anew: clean all