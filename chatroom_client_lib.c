#include "chatroom_client_lib.h"

GtkBuilder* app_builder;
ChatData* chat_data = NULL;

/*
	Initializes application UI	
*/
void init_app(){

    GtkWidget *window;
    
    app_builder = gtk_builder_new_from_file(UI_FILE);

    gtk_builder_connect_signals(app_builder, NULL);

    window = GTK_WIDGET(gtk_builder_get_object(app_builder, "main_window"));

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    gtk_widget_show(window);

    gtk_main();
}

/*
	Tries to connect to server with IP and port recieved by input.

	@param: ip_address --> IP that user has given.
	@param: port --> Port user has given.

	@return: TRUE if connection successful, FALSE if not.
*/
gboolean connect_with_server(char* ip_address, char* port){

    chat_data = malloc(sizeof(ChatData));

    // Socket descriptor creation
    chat_data->sockfd = socket(AF_INET, SOCK_STREAM, 0);    

    if (chat_data->sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    }

    bzero(&chat_data->servaddr, sizeof(chat_data->servaddr)); 
  
    // Assign IP, PORT 
    chat_data->servaddr.sin_family = AF_INET; 
    chat_data->servaddr.sin_addr.s_addr = inet_addr(ip_address); 
    chat_data->servaddr.sin_port = htons(atoi(port)); 
  
    // EStablish conection between client socket and server socket.
    if (connect(chat_data->sockfd, (struct sockaddr*)&chat_data->servaddr, sizeof(chat_data->servaddr)) != 0) { 
        show_connection_failed_dialog();
        return FALSE;
    }

    return TRUE;
}

/*
	Launched as a thread. Listens incoming msgs from server and updates chat view.

	@param: param: Required by pthread.h. NULL in this app.

*/
void* chat_listener_thread(void* param){

    printf("Chat thread initialized\n");

    int status = 1;
    char *msg_buffer = malloc(MAX_MSG_LENGTH);

    while(status != 0){
        
        printf("Waiting msg\n");

        status = read(chat_data->sockfd, msg_buffer, MAX_MSG_LENGTH);

        printf("Recibido\n");

        update_chat_view(msg_buffer);

        printf("RECV: %s\n", msg_buffer);

        bzero(msg_buffer, MAX_MSG_LENGTH);
    }

    printf("Connection closed\n");
        
    free(msg_buffer);
    free(chat_data->chat_listener);

    pthread_exit((void*) 0);
}

/*
	Creates chat listener thread once connection with server established.
*/
void create_chat_listener_thread(){

    chat_data->chat_listener = (pthread_t*) malloc(sizeof(pthread_t));
    pthread_attr_t atributes;
    pthread_attr_init(&atributes);
    pthread_attr_setdetachstate(&atributes, PTHREAD_CREATE_DETACHED); //Creates detached thread that frees resources when finish   
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL); //Sets cancel type to asynchronous --> canceled at any time
    pthread_create(chat_data->chat_listener, &atributes, chat_listener_thread, NULL);

}

/*
	Updates chat view with new msg.
	@param: msg_display --> String containing msg to diplay.
*/
void update_chat_view(char* msg_display){

    GtkTextView *chat_view = GTK_TEXT_VIEW(gtk_builder_get_object(app_builder, "chatroom_view"));
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(chat_view);
    GtkTextMark *mark = gtk_text_buffer_get_insert(buffer);
    GtkTextIter iter;
    gtk_text_buffer_get_iter_at_mark(buffer, &iter, mark); //Initializes iter with the current positio of mark.

    // Insert msg in GTextView(chat view)
    gtk_text_buffer_insert(buffer, &iter, msg_display, -1);
    gtk_text_buffer_insert(buffer, &iter, "\n", 1); //Inserts a new line
}

/*
	Clears chat view.
*/
void clear_chat_view(){

    GtkTextIter* start = malloc(sizeof(GtkTextIter));
    GtkTextIter* end = malloc(sizeof(GtkTextIter));

    GtkTextView *chat_view = GTK_TEXT_VIEW(gtk_builder_get_object(app_builder, "chatroom_view"));
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(chat_view);
    gtk_text_buffer_get_bounds(buffer, start, end);
    gtk_text_buffer_delete(buffer, start, end);

    free(start);
    free(end);
}

/*
	Used as a callback function when 'Conectar' menu item is clicked.
	Creates new window with entries for server IP and port, and a btn 
	to connect.

	@param: widget --> widget thar was clicked. Passed by gtk lib.
	@param: data --> Data passd when function called. NULL in this case.
*/
void on_menu_connect_click(GtkWidget *widget, gpointer *data){

    printf("Clicked menu connect\n");

    ConnectionRequest *conn_request = malloc(sizeof(ConnectionRequest));

    conn_request->connection_window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
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
    
    gtk_widget_set_margin_start(GTK_WIDGET(conn_request->ip_entry), 20);
    gtk_widget_set_margin_end(GTK_WIDGET(conn_request->ip_entry), 20);

    gtk_box_pack_start(ip_box, GTK_WIDGET(ip_label), TRUE, TRUE, 0);
    gtk_box_pack_start(ip_box, GTK_WIDGET(conn_request->ip_entry), TRUE, TRUE, 0);

    //PORT
    gtk_widget_set_size_request(GTK_WIDGET(port_label), 124, 0);
    
    gtk_widget_set_margin_start(GTK_WIDGET(conn_request->port_entry), 20);
    gtk_widget_set_margin_end(GTK_WIDGET(conn_request->port_entry), 20);

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

    gtk_widget_set_size_request(GTK_WIDGET(conn_request->connection_window), 400, 200);

    gtk_window_set_resizable(GTK_WINDOW(conn_request->connection_window), FALSE);

    gtk_window_set_position(GTK_WINDOW(conn_request->connection_window), GTK_WIN_POS_CENTER);

    gtk_widget_show_all(GTK_WIDGET(conn_request->connection_window));
}

/*
	Used as a callback function when 'Conectar' btn clicked on connection window.
	Connects the client to server, create chat listener thread and closes
	connection window.

	@param: widget --> Button clicked.
	@param: conn_request --> struct contaning info related to the users connection
							request.
*/
void on_connect_btn_clicked(GtkWidget *widget, ConnectionRequest *conn_request){

    printf("Clicked\n");

    if(gtk_entry_get_text_length(conn_request->ip_entry) == 0 || gtk_entry_get_text_length(conn_request->port_entry) == 0){

        printf("Campos vacios\n");        
        return; 
    }

    const gchar* server_ip = gtk_entry_get_text(conn_request->ip_entry);
    const gchar* server_port = gtk_entry_get_text(conn_request->port_entry);

    printf("IP : %s\n", server_ip);
    printf("Port: %s\n", server_port);

    // Connects to server storing useful data in chat_data
    if(!connect_with_server((char*)server_ip, (char*)server_port)){
        gtk_window_close(conn_request->connection_window);
        return;
    }
    
    printf("Sock fd: %d\n", chat_data->sockfd);

    GtkButton *send_btn = GTK_BUTTON(gtk_builder_get_object(app_builder, "send_button"));

    g_signal_connect(send_btn, "clicked", G_CALLBACK(on_send_btn_clicked), NULL); // Connect clicked event on send button to callback

    create_chat_listener_thread();

    gtk_window_close(conn_request->connection_window);

    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(app_builder, "server_ip_label")), server_ip); // Show server IP on label
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(app_builder, "server_port_label")), server_port); //Show server port on label
    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(app_builder, "connect_menu_item")), FALSE); //Disables Connect btn
    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(app_builder, "disconnect_menu_item")), TRUE); //Enables Disconnect btn
    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(app_builder, "send_button")), TRUE); //Enables Send btn

    free(conn_request);
}

/*
	Used as a callback function when 'Desonectar' menu item clicked.
	Disconnects client from server and clears the char view.

	@param: widget --> Menu item clicked.
	@param: data --> Data passed. NULL in this case.
*/
void on_disconnect_btn_clicked(GtkWidget *widget, gpointer *data){

    pthread_cancel(*chat_data->chat_listener); // Cancels thread listening incoming msgs.

    close(chat_data->sockfd); // Closes connection

    clear_chat_view();

    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(app_builder, "server_ip_label")), "No Conectado");
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(app_builder, "server_port_label")), "No Conectado");
    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(app_builder, "connect_menu_item")), TRUE); //Enabled Connect btn
    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(app_builder, "disconnect_menu_item")), FALSE); //Disables Disconnect btn
    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(app_builder, "send_button")), FALSE); //Disables Send btn

    printf("Connection closed\n");
}

/*
	Used as a callback function when 'Enviar' btn clicked.
	Sends the msg written by user to the server and updates chat view.
	
	@þaram: widget --> Btn clicked.
	@param: data --> Data passed. NULL  in this case.
*/
void on_send_btn_clicked(GtkWidget *widget, gpointer *data){

    printf("Send click\n");

    GtkEntry *msg_entry = GTK_ENTRY(gtk_builder_get_object(app_builder, "msg_entry"));
    char *msg_display = malloc(MAX_MSG_LENGTH);

    if(gtk_entry_get_text_length(msg_entry) == 0){
        printf("Send vacío\n");
        return;
    }

    const gchar *msg = gtk_entry_get_text(msg_entry); //Get msg to send
    
    write(chat_data->sockfd, (char*)msg, MAX_MSG_LENGTH); // Sends the msg.

    //Prepare display_msg
    strcpy(msg_display, "YO: ");

    strcat(msg_display, msg);

    update_chat_view(msg_display);

    gtk_entry_set_text(msg_entry, ""); //Clears the msg entry..

    free(msg_display);
}

/*
	Used as a callback function when 'Aceptar' btn clicked on popup.

	@param: widget --> Btn clicked.
	@param: response_id --> Id from the response generated by btn.
	@param: dialog --> Dialog that needs to be closed.
*/
void close_connection_failed_dialog(GtkWidget* widget, gint response_id, GtkDialog *dialog){
    
    gtk_widget_destroy(GTK_WIDGET(dialog));

}

/*
	Creates and show a dialog telling user that connection with server failed.
*/
void show_connection_failed_dialog(){

    GtkLabel* label;
    GtkWidget* content_area;
    GtkDialog* dialog;
    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    dialog = GTK_DIALOG(gtk_dialog_new_with_buttons("Falló Conexión con el Servidor", GTK_WINDOW(gtk_builder_get_object(app_builder, "main_window")),
                                                        flags, "_OK", GTK_RESPONSE_OK, NULL));

    content_area = gtk_dialog_get_content_area(dialog);

    label = GTK_LABEL(gtk_label_new("Ha ocurrido un fallo al conectar con el servidor.\nCorrobore que la IP y el puerto sean correctos."));   

    gtk_container_add(GTK_CONTAINER(content_area), GTK_WIDGET(label));

    g_signal_connect(dialog, "response", G_CALLBACK(close_connection_failed_dialog), dialog);

    gtk_widget_set_size_request(GTK_WIDGET(dialog), 400, 200);

    gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);

    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);

    gtk_widget_show_all(GTK_WIDGET(dialog));

}