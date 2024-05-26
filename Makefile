CC = gcc
objects = server client
.PHONY : all clean

all : $(objects)

server : server.c
	$(CC) -o server server.c

client : client.c
	$(CC) -o client client.c

debug : server.c client.c
	$(CC) -g server.c -o sever
	$(CC) -g client.c -o client
	
clean : 
	rm -f $(objects)