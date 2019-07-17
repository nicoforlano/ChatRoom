cc = gcc

all: chatroom_server chatroom_client

chatroom_server: chatroom_server.c
	cc chatroom_server.c -o chatroom_server -pthread -std=c99 -D_POSIX_C_SOURCE=200809L

chatroom_client: chatroom_client.c
	cc -pthread chatroom_client.c -o chatroom_client -Wall `pkg-config --cflags --libs gtk+-3.0` -export-dynamic -g

clean:
	rm -rf *o chatroom_server chatroom_client *~

clean-o:
	rm *.o