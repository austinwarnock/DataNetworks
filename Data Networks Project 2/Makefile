all: server sender reciever

server: Server.c
	gcc -o server Server.c

sender: Sender.c
	gcc -o sender Sender.c	 

reciever: Reciever.c
	gcc -o reciever Reciever.c

clean:
	rm -f *.o reciever server sender
