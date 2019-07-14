#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#define HELLO_MSG "Bienvenido al chatroom!"
#define MAX_CLIENTS 10
#define PORT 8080
#define MAX_BUFFER_LENGTH 256

typedef struct server_data{

	int connections[MAX_CLIENTS];
	int active_connections;
	int sockfd;
	struct sockaddr_in server_addr;

} server_data;

typedef struct client_data{

	server_data *server;
	int conn_id;
	struct sockaddr client_addr;

} client_data;

void init_server(server_data* server);
void* client_thread(void* param);

int main(int argc, char const *argv[])
{
		
	server_data server;
	int connfd;
	socklen_t cli_addr_size;
	struct sockaddr client_addr;
	pthread_t* client_threads = (pthread_t*) malloc(sizeof(pthread_t*) * MAX_CLIENTS);
	client_data *clients = malloc(sizeof(client_data) * MAX_CLIENTS);

	server.active_connections = 0;

	init_server(&server);

	cli_addr_size = sizeof(client_addr);

	while(1){

		printf("Accepting client connections\n");
		
		printf("%d\n", server.sockfd);

		connfd = accept(server.sockfd, (struct sockaddr*) &client_addr, &cli_addr_size);

		if(connfd < 0){
			printf("Server accept failed\n");
			exit(0);
		}

		printf("Recieved client conn %d\n", connfd);

		clients[server.active_connections].conn_id = server.active_connections;
		clients[server.active_connections].server = &server;
		clients[server.active_connections].client_addr = client_addr;

		pthread_create(&client_threads[server.active_connections], NULL, client_thread, &clients[server.active_connections]);
		server.connections[server.active_connections] = connfd;

		server.active_connections++;
	}

	close(server.sockfd);

	return 0;
}

void init_server(server_data* server){

	server->sockfd = socket(AF_INET, SOCK_STREAM, 0);
	printf("%d\n", server->sockfd);
	if(server->sockfd == -1){
		printf("no se pud crear el socket");
		exit(0);
	}

	server->server_addr.sin_family = AF_INET;
	server->server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server->server_addr.sin_port = htons(PORT);

	if(bind(server->sockfd, (struct sockaddr*) &server->server_addr, sizeof(server->server_addr)) != 0){
		printf("SOcket bind failed\n");
		exit(0);
	}

	printf("SOcket binded!\n");


	if(listen(server->sockfd, MAX_CLIENTS) != 0){
		printf("Listen failed\n");
		exit(0);
	}

	printf("Server ready to listen\n");

}


void* client_thread(void* param){

	client_data* client = (client_data*) param;
	server_data* server = client->server;

	char* buffer[MAX_BUFFER_LENGTH];
	int status;

	printf("Client sock fd %d\n", server->connections[client->conn_id]);
	printf("Sending hello\n");

	write(server->connections[client->conn_id], HELLO_MSG, sizeof(HELLO_MSG));

	while(1){
		
		printf("Wating for msg\n");

		status = read(server->connections[client->conn_id], buffer, MAX_BUFFER_LENGTH);
		
		printf("Satus: %d\n", status);

		if(status == 0){
			printf("Client %d closed connection\n", client->conn_id);
			server->active_connections--;
			return 0;
		}

		printf("CONN %d %s: %s\n", client->conn_id, "", buffer);

	}
}