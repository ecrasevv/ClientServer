CC = gcc
CFLAGS = -Wall -o

all: server client

server: src/server.c
	$(CC) $(CFLAGS) server src/server.c

client: src/client.c
	$(CC) $(CFLAGS) client src/client.c

.PHONY: clean

clean: 
	@rm -f client server
