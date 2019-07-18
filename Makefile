cc = gcc

all: chatroom_server chatroom_client clean-o

chatroom_server: chatroom_server.c chatroom_server_lib.h chatroom_server_lib.o
	cc chatroom_server.c chatroom_server_lib.o  -o chatroom_server -pthread -std=c99 -D_POSIX_C_SOURCE=200809L

chatroom_client: chatroom_client.c
	cc -pthread chatroom_client.c -o chatroom_client -Wall `pkg-config --cflags --libs gtk+-3.0` -export-dynamic -g

chatroom_server_lib.o:  chatroom_server_lib.h chatroom_server_lib.c
	cc -c chatroom_server_lib.c -std=c99

clean:
	rm -rf *o chatroom_server chatroom_client *~

clean-o:
	rm *.o