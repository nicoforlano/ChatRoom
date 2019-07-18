#ifndef CHATROOM_SERVER

#define CHATROOM_SERVER

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#define HELLO_MSG "SERVER: Bienvenido al chatroom!"
#define CLI_DESC_MSG "%s se ha desconectado"
#define MAX_CLIENTS 10
#define PORT 8080
#define MAX_BUFFER_LENGTH 256

typedef struct ServerData{

	int connections[MAX_CLIENTS];
	int active_connections;
	int sockfd;
	struct sockaddr_in server_addr;

} ServerData;

typedef struct ClientData{

	ServerData *server;
	int conn_id;
	struct sockaddr client_addr;
	char host[100];
	char service[20];

} ClientData;

void init_server(ServerData* server);
void* client_thread(void* param);

#endif