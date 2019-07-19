#include "chatroom_server_lib.h"

/*
	Initializes server.

	@param: server --> Server struct to be filled with server data.

*/
void init_server(ServerData* server){

	server->sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if(server->sockfd == -1){
		printf("> Socket no pudo ser creado");
		exit(0);
	}

	server->server_addr.sin_family = AF_INET;
	server->server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server->server_addr.sin_port = htons(PORT);

	if(bind(server->sockfd, (struct sockaddr*) &server->server_addr, sizeof(server->server_addr)) != 0){
		printf("> Falló el socket bind\n");
		exit(0);
	}

	if(listen(server->sockfd, MAX_CLIENTS) != 0){
		printf("> Falló listen() del socket.\n");
		exit(0);
	}
}

/*
	Broadcasts msgs to all active clients except the one that sent msg.

	@param: msg --> Msg recieved.
	@param: client --> Sender data.
	@param: server --> Sever data.
*/
void broadcast_message(char* msg, ClientData* client, ServerData* server){

	char* msg_to_send = malloc(MAX_BUFFER_LENGTH);
	//Prepare msg -> <hostname>: <msg>
	strcpy(msg_to_send, client->host);
	strcat(msg_to_send, ": ");
	strcat(msg_to_send, msg);

	for(int i=0; i<server->active_connections; i++){
		if(client->conn_id != i){
			write(server->connections[i], msg_to_send, MAX_BUFFER_LENGTH);
		}
	}

	free(msg_to_send);
}

/*
	Executed as a thread. Manages connection with socket listening to 
	incoming msgs and sending it to other clients.
	
	@param: param: Required by pthread.h. Contains all data realted to client.

*/
void* client_thread(void* param){

	ClientData* client = (ClientData*) param;
	ServerData* server = client->server;

	char* buffer = malloc(MAX_BUFFER_LENGTH);
	int status;

	write(server->connections[client->conn_id], HELLO_MSG, sizeof(HELLO_MSG));

	while(1){

		bzero(buffer, MAX_BUFFER_LENGTH);

		status = read(server->connections[client->conn_id], buffer, MAX_BUFFER_LENGTH);

		if(status == 0){
			sprintf(buffer, CLI_DESC_MSG, client->host); 
			broadcast_message(buffer, client, server); //Notifies connections that client has disconected
			server->active_connections--;
			free(buffer);
			return 0;
		}

		broadcast_message(buffer, client, server);
		printf("> Mensaje de %s enviado: %s\n", client->host, buffer);

	}
}