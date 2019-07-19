#include "chatroom_server_lib.h"

int main(int argc, char const *argv[])
{
		
	ServerData server;
	int connfd;
	socklen_t cli_addr_size;
	struct sockaddr client_addr;
	pthread_t* client_threads = (pthread_t*) malloc(sizeof(pthread_t*) * MAX_CLIENTS);
	ClientData *clients = malloc(sizeof(ClientData) * MAX_CLIENTS);

	server.active_connections = 0;

	init_server(&server);

	cli_addr_size = sizeof(client_addr);

	printf("> Servidor listo para recibir conexiones.\n");

	while(1){

		connfd = accept(server.sockfd, (struct sockaddr*) &client_addr, &cli_addr_size);

		if(connfd < 0){
			printf("Server accept failed\n");
			exit(0);
		}

		printf("> Recibida conexión de cliente. SockFD: %d\n", connfd);

		clients[server.active_connections].conn_id = server.active_connections;
		clients[server.active_connections].server = &server;
		clients[server.active_connections].client_addr = client_addr;
		getnameinfo(&client_addr, sizeof client_addr, (char*)clients[server.active_connections].host, 
						sizeof clients[server.active_connections].host, (char*)clients[server.active_connections].service,
						sizeof clients[server.active_connections].service, 0); // Gets hostname and service info

		pthread_create(&client_threads[server.active_connections], NULL, client_thread, &clients[server.active_connections]);
		server.connections[server.active_connections] = connfd;

		server.active_connections++;
	}

	close(server.sockfd);

	return 0;
}

