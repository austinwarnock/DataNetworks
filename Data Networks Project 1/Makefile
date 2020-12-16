all: server client

server: Server.c
	gcc -o server Server.c -Wall -pedantic

client: Client.c
	gcc -o client Client.c -Wall -pedantic

clean:
	rm -f *.o client server
