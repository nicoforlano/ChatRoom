
#include <gtk/gtk.h>
#include <pthread.h>
#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#define MAX_BUFF_SIZE 512 
#define PORT 8080 
#define UI_FILE "chatroom_client_UI.glade"


typedef struct chat_thread_data{

    struct sockaddr_in servaddr;
    int sockfd;
    pthread_t* chat_listener;

} ChatThreadData;

typedef struct connection_request{

    GtkWindow *connection_window;
    GtkEntry *ip_entry;
    GtkEntry *port_entry;

} ConnectionRequest;

GtkBuilder *main_builder;

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

    gtk_builder_connect_signals(main_builder, main_builder);

    window = GTK_WIDGET(gtk_builder_get_object(main_builder, "main_window"));

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    gtk_widget_show(window);

    gtk_main();
}
  
void on_menu_connect_click(GtkWidget *widget, GtkBuilder *main_builder);
void on_connect_btn_clicked(GtkWidget *widget, ConnectionRequest *conn_request);
void on_send_btn_clicked(GtkWidget *widget, ChatThreadData *chat_thread_data);

int main(int argc, char* argv[]) { 

    printf("Iniciando UI\n");

    gtk_init(&argc, &argv);

    init_app();

    return 0;
} 


ChatThreadData* connect_with_server(char* ip_address, char* port){

    ChatThreadData *chat_thread_data = (ChatThreadData*) malloc(sizeof(ChatThreadData*));

    // socket create and varification 
    chat_thread_data->sockfd = socket(AF_INET, SOCK_STREAM, 0); 

    if (chat_thread_data->sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 

    bzero(&chat_thread_data->servaddr, sizeof(chat_thread_data->servaddr)); 
  
    // assign IP, PORT 
    chat_thread_data->servaddr.sin_family = AF_INET; 
    chat_thread_data->servaddr.sin_addr.s_addr = inet_addr(ip_address); 
    chat_thread_data->servaddr.sin_port = htons(atoi(port)); 
  
    // connect the client socket to server socket 
    if (connect(chat_thread_data->sockfd, (struct sockaddr*)&chat_thread_data->servaddr, sizeof(chat_thread_data->servaddr)) != 0) { 
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


void on_menu_connect_click(GtkWidget *widget, GtkBuilder *main_builder){

    printf("Clicked\n");

    ConnectionRequest *conn_request = (ConnectionRequest*)malloc(sizeof(ConnectionRequest*));

    conn_request->connection_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkBox *vbox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 5));
    GtkBox *ip_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5));
    GtkLabel *ip_label = GTK_LABEL(gtk_label_new("Ip del Servidor"));
    conn_request->ip_entry = GTK_ENTRY(gtk_entry_new());
    GtkBox *port_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5));
    GtkLabel *port_label = GTK_LABEL(gtk_label_new("Puerto del Servidor"));
    conn_request->port_entry = GTK_ENTRY(gtk_entry_new());
    GtkButton *connect_btn = GTK_BUTTON(gtk_button_new_with_label("Conectar"));

    //IP
    gtk_widget_set_size_request(GTK_WIDGET(ip_label), 124, 0);
    
    gtk_widget_set_margin_left(GTK_WIDGET(conn_request->ip_entry), 20);
    gtk_widget_set_margin_right(GTK_WIDGET(conn_request->ip_entry), 20);

    gtk_box_pack_start(ip_box, GTK_WIDGET(ip_label), TRUE, TRUE, 0);
    gtk_box_pack_start(ip_box, GTK_WIDGET(conn_request->ip_entry), TRUE, TRUE, 0);

    //PORT
    gtk_widget_set_size_request(GTK_WIDGET(port_label), 124, 0);
    
    gtk_widget_set_margin_left(GTK_WIDGET(conn_request->port_entry), 20);
    gtk_widget_set_margin_right(GTK_WIDGET(conn_request->port_entry), 20);

    gtk_box_pack_start(port_box, GTK_WIDGET(port_label), TRUE, TRUE, 0);
    gtk_box_pack_start(port_box, GTK_WIDGET(conn_request->port_entry), TRUE, TRUE, 0);

    //BUTTON
    g_signal_connect(connect_btn, "clicked", G_CALLBACK(on_connect_btn_clicked), conn_request);

    //VBOX
    gtk_box_pack_start(vbox, GTK_WIDGET(ip_box), TRUE, TRUE, 0);
    gtk_box_pack_start(vbox, GTK_WIDGET(port_box), TRUE, TRUE, 0);
    gtk_box_pack_start(vbox, GTK_WIDGET(connect_btn), TRUE, TRUE, 0);

    //WINDOW
    gtk_container_add(GTK_CONTAINER(conn_request->connection_window), GTK_WIDGET(vbox));

    gtk_window_set_title(GTK_WINDOW(conn_request->connection_window), "Conectar con Servidor");

    gtk_widget_set_size_request(conn_request->connection_window, 400, 200);

    gtk_window_set_resizable(GTK_WINDOW(conn_request->connection_window), FALSE);

    gtk_window_set_position(GTK_WINDOW(conn_request->connection_window), GTK_WIN_POS_CENTER);

    gtk_widget_show_all(conn_request->connection_window);
}

void create_chat_thread(ChatThreadData *chat_thread_data){  

    pthread_t *chat_threads = (pthread_t*) malloc(sizeof(pthread_t*));

    
    
}

void on_connect_btn_clicked(GtkWidget *widget, ConnectionRequest *conn_request){

    printf("Clicked\n");

    ChatThreadData *chat_thread_data = (ChatThreadData*) malloc(sizeof(ChatThreadData*));

    if(gtk_entry_get_text_length(conn_request->ip_entry) == 0 || gtk_entry_get_text_length(conn_request->port_entry) == 0){

        printf("Campos vacios\n");        
        return;
    }

    gchar* server_ip = gtk_entry_get_text(conn_request->ip_entry);
    gchar* server_port = gtk_entry_get_text(conn_request->port_entry);

    printf("IP : %s\n", server_ip);
    printf("Port: %s\n", server_port);

    chat_thread_data = connect_with_server(server_ip, server_port);
    
    printf("Sock fd: %d\n", chat_thread_data->sockfd);

    GtkButton *send_btn = GTK_BUTTON(gtk_builder_get_object(main_builder, "send_button"));

    g_signal_connect(send_btn, "clicked", G_CALLBACK(on_send_btn_clicked), chat_thread_data); //Sends chat data to callback



    gtk_window_close(conn_request->connection_window);

}

void on_send_btn_clicked(GtkWidget *widget, ChatThreadData *chat_thread_data){

    printf("Send click\n");

    GtkEntry *msg_entry = GTK_ENTRY(gtk_builder_get_object(main_builder, "msg_entry"));
    char* msg_display[1024];

    if(gtk_entry_get_text_length(msg_entry) == 0){
        printf("Send vacío\n"); 
    }

    GtkTextView *chat_view = GTK_TEXT_VIEW(gtk_builder_get_object(main_builder, "chatroom_view"));
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(chat_view);
    GtkTextMark *mark = gtk_text_buffer_get_insert(buffer);
    GtkTextIter iter;
    gtk_text_buffer_get_iter_at_mark(buffer, &iter, mark); //Initializes iter with the current positio of mark.

    gchar *msg = gtk_entry_get_text(msg_entry); //Get msg to send

    write(chat_thread_data->sockfd, (char*)msg, sizeof(msg)); // Sends the msg.

    //Prepare display_msg
    strcpy(msg_display, "YO: ");

    strcat(msg_display, msg);

    // Insert msg in GTextView(chat view)
    gtk_text_buffer_insert(buffer, &iter, msg_display, -1);
    gtk_text_buffer_insert(buffer, &iter, "\n", 1); //Inserts a new line

    gtk_entry_set_text(msg_entry, ""); //Clears the msg entry..

}
