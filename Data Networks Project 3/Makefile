all: server client

server: Server.c
	gcc -pthread -o server Server.c -Wall -pedantic

client: Client.c
	gcc -pthread -o client Client.c -Wall -pedantic

clean:
	rm -f *.o client server
