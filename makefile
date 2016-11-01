all: server client
	rm -f *.log

server: 	
	gcc -pthread server.c  accept.c  recieve.c send.c -o server379

client:
	gcc client.c -o chat379
	
clean:
	rm server379
	rm chat379

server379:
	./server379