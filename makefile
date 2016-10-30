make:
	rm -f *.log
	gcc -pthread -o server379 accept.c server.c 
	gcc client.c -o chat379
clean:
	rm server379
	rm chat379
server379:
	./server379