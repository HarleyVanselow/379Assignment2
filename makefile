make:
	gcc server.c -o server379
	gcc client.c -o chat379
clean:
	rm server379
	rm chat379