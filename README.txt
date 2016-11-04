# 379Assignment2
Project 2

Harley Vanselow
Kdan Hillier 1393746

Chat server and client implementation.

Compiling:
	All: make
	client: make client
	server: make server
	clean: make clean
	clean and make all: make anew

Usages: 
	Server: 
		Running: ./server379 port_number
		Quitting: kill pid

	Client: 
		Running: ./chat379 hostname port_number username
		Exit: ctrl + c 
		List users: .users 