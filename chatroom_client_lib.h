#ifndef CHATROOM_CLI_LIB

#define CHATROOM_CLI_LIB

#include <gtk/gtk.h>
#include <pthread.h>
#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <arpa/inet.h>
#define PORT 8080 
#define UI_FILE "chatroom_client_UI.glade"
#define MAX_MSG_LENGTH 256

typedef struct chat_data{

    struct sockaddr_in servaddr;
    int sockfd;
    pthread_t* chat_listener;

} ChatData;

typedef struct connection_request{

    GtkWindow *connection_window;
    GtkEntry *ip_entry;
    GtkEntry *port_entry;

} ConnectionRequest;

extern GtkBuilder* app_builder;
extern ChatData* chat_data;

void init_app();
gboolean connect_with_server(char* ip_address, char* port);
void* chat_listener_thread(void* param);
void create_chat_listener_thread();
void update_chat_view(char* msg_display);
void clear_chat_view();
void on_menu_connect_click(GtkWidget *widget, gpointer *data);
void on_connect_btn_clicked(GtkWidget *widget, ConnectionRequest *conn_request);
void on_disconnect_btn_clicked(GtkWidget *widget, gpointer *data);
void on_send_btn_clicked(GtkWidget *widget, gpointer *data);
void close_connection_failed_dialog(GtkWidget* widget, gint response_id, GtkDialog *dialog);
void show_connection_failed_dialog();

#endif