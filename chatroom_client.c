
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

    gtk_builder_connect_signals(main_builder, main_builder);

    window = GTK_WIDGET(gtk_builder_get_object(main_builder, "main_window"));

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    gtk_widget_show(window);

    gtk_main();
}
  
void on_menu_connect_click(GtkWidget *widget, GtkBuilder *main_builder);
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


void on_menu_connect_click(GtkWidget *widget, GtkBuilder *main_builder){

    printf("Clicked\n");

    GtkWidget *connection_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkBox *vbox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 5));
    GtkBox *ip_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5));
    GtkLabel *ip_label = GTK_LABEL(gtk_label_new("Ip del Servidor"));
    GtkEntry *ip_entry = GTK_ENTRY(gtk_entry_new());
    GtkBox *port_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5));
    GtkLabel *port_label = GTK_LABEL(gtk_label_new("Puerto del Servidor"));
    GtkEntry *port_entry = GTK_ENTRY(gtk_entry_new());
    GtkButton *connect_btn = GTK_BUTTON(gtk_button_new_with_label("Conectar"));

    //IP
    gtk_widget_set_size_request(GTK_WIDGET(ip_label), 124, 0);
    
    gtk_widget_set_margin_left(GTK_WIDGET(ip_entry), 20);
    gtk_widget_set_margin_right(GTK_WIDGET(ip_entry), 20);

    gtk_box_pack_start(ip_box, GTK_WIDGET(ip_label), TRUE, TRUE, 0);
    gtk_box_pack_start(ip_box, GTK_WIDGET(ip_entry), TRUE, TRUE, 0);

    //PORT
    gtk_widget_set_size_request(GTK_WIDGET(port_label), 124, 0);
    
    gtk_widget_set_margin_left(GTK_WIDGET(port_entry), 20);
    gtk_widget_set_margin_right(GTK_WIDGET(port_entry), 20);

    gtk_box_pack_start(port_box, GTK_WIDGET(port_label), TRUE, TRUE, 0);
    gtk_box_pack_start(port_box, GTK_WIDGET(port_entry), TRUE, TRUE, 0);

    //VBOX
    gtk_box_pack_start(vbox, GTK_WIDGET(ip_box), TRUE, TRUE, 0);
    gtk_box_pack_start(vbox, GTK_WIDGET(port_box), TRUE, TRUE, 0);
    gtk_box_pack_start(vbox, GTK_WIDGET(connect_btn), TRUE, TRUE, 0);

    //WINDOW
    gtk_container_add(GTK_CONTAINER(connection_window), GTK_WIDGET(vbox));

    gtk_window_set_title(GTK_WINDOW(connection_window), "Conectar con Servidor");

    gtk_widget_set_size_request(connection_window, 400, 200);

    gtk_window_set_resizable(GTK_WINDOW(connection_window), FALSE);

    gtk_window_set_position(GTK_WINDOW(connection_window), GTK_WIN_POS_CENTER);

    gtk_widget_show_all(connection_window);
}

void create_chat_threads(ChatThreadData chat_thread_data){  

    pthread_t *chat_threads = (pthread_t*) malloc(sizeof(pthread_t) * 2); // Writer and Reader thread

    

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