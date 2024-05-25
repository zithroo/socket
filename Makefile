CC = gcc
objects = server client
.PHONY : all clean

all : $(objects)

server : server.c
	$(CC) -o server server.c

client : client.c
	$(CC) -o client client.c

clean : 
	rm -f $(objects)