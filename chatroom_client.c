
#include <gtk/gtk.h>
#include <pthread.h>
#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#define MAX_BUFF_SIZE 80 
#define PORT 8080 
#define UI_FILE "chatroom_client_UI.glade"
#define CONN_WINDOW_UI_FILE "connection_window.glade"


typedef struct chat_thread_data{

    struct sockaddr_in servaddr;
    int sockfd;

} ChatThreadData;

GtkBuilder *main_builder;

void func(int sockfd) 
{ 
    char buff[MAX_BUFF_SIZE]; 
    int n;

    for (;;) { 
        bzero(buff, sizeof(buff)); 
        printf("Enter the string : "); 
        n = 0; 
        while ((buff[n++] = getchar()) != '\n') 
            ; 
        write(sockfd, buff, sizeof(buff)); 
        
    } 
}

void init_app(){

    GtkWidget *window;
    GError *error = NULL;
    main_builder = gtk_builder_new();
    
    gtk_builder_add_from_file(main_builder, UI_FILE, &error);

    if(error != NULL){

        g_warning("%s", error->message);
        g_free(error);
        gtk_main_quit();        
    }

    gtk_builder_connect_signals(main_builder, NULL);

    window = GTK_WIDGET(gtk_builder_get_object(main_builder, "main_window"));

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    gtk_widget_show(window);

    gtk_main();
}
  
void on_menu_connect_click(GtkWidget *widget, gpointer *data);
void on_connect_btn_clicked(GtkWidget *widget, GtkBuilder *conn_window_builder);

int main(int argc, char* argv[]) { 

    printf("Iniciando UI\n");

    gtk_init(&argc, &argv);

    init_app();

    return 0;
} 


ChatThreadData connect_with_server(char* ip_address, char* port){

    ChatThreadData chat_thread_data;

    // socket create and varification 
    chat_thread_data.sockfd = socket(AF_INET, SOCK_STREAM, 0); 

    if (chat_thread_data.sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 

    bzero(&chat_thread_data.servaddr, sizeof(chat_thread_data.servaddr)); 
  
    // assign IP, PORT 
    chat_thread_data.servaddr.sin_family = AF_INET; 
    chat_thread_data.servaddr.sin_addr.s_addr = inet_addr(ip_address); 
    chat_thread_data.servaddr.sin_port = htons(atoi(port)); 
  
    // connect the client socket to server socket 
    if (connect(chat_thread_data.sockfd, (struct sockaddr*)&chat_thread_data.servaddr, sizeof(chat_thread_data.servaddr)) != 0) { 
        printf("connection with the server failed...\n"); 
        exit(0); 
    } 
    else
        printf("connected to the server..\n"); 
  
    /*// function for chat 
    func(sockfd); 
  
    // close the socket 
    close(sockfd);*/

    return chat_thread_data;
}


void on_menu_connect_click(GtkWidget *widget, gpointer *data){

    printf("Clicked\n");

    GError *error = NULL;
    GtkBuilder *conn_window_builder =  gtk_builder_new();

    gtk_builder_add_from_file(conn_window_builder, CONN_WINDOW_UI_FILE, &error);

    if(error != NULL){

        g_warning("%s", error->message);
        g_free(error);
        exit(0);
        
    }

    gtk_builder_connect_signals(conn_window_builder, conn_window_builder); // Send builder to all callback

    GtkWidget *connection_window = GTK_WIDGET(gtk_builder_get_object(conn_window_builder, "connection_window"));
    printf("obtvop window\n");
    gtk_window_set_title(GTK_WINDOW(connection_window), "Conectar con Servidor");
    printf("title\n");
    gtk_widget_set_size_request(connection_window, 400, 200);
    printf("size req\n");
    gtk_window_set_resizable(GTK_WINDOW(connection_window), FALSE);
    printf("resizable\n");
    gtk_window_set_position(GTK_WINDOW(connection_window), GTK_WIN_POS_CENTER);
    printf("posi\n");

    gtk_widget_show(connection_window);
}

void create_chat_threads(ChatThreadData chat_thread_data){

    pthread_t *chat_threads = (pthread_t*) malloc(sizeof(pthread_t) * 2); // Writer and Reader thread

    pthread_attr_set_

}

void on_connect_btn_clicked(GtkWidget *widget, GtkBuilder *conn_window_builder){

    printf("Clicked\n");

    GtkEntry *server_ip_entry = GTK_ENTRY(gtk_builder_get_object(conn_window_builder, "ip_entry"));
    GtkEntry *server_port_entry = GTK_ENTRY(gtk_builder_get_object(conn_window_builder, "port_entry"));

    gchar* server_ip = gtk_entry_get_text(server_ip_entry);
    gchar* server_port = gtk_entry_get_text(server_port_entry);

    if(gtk_entry_get_text_length(server_ip_entry) == 0 || gtk_entry_get_text_length(server_port_entry) == 0){

        printf("Campos vacios\n");        
        return;
    }

    printf("IP : %s\n", server_ip);
    printf("Port: %s\n", server_port);

    ChatThreadData chat_thread_data= connect_with_server(server_ip, server_port);



    gtk_widget_destroy(GTK_WIDGET(gtk_builder_get_object(conn_window_builder, "connection_window")));

}