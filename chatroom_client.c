#include "chatroom_client_lib.h"

int main(int argc, char* argv[]) { 

    printf("Iniciando UI\n");

    gtk_init(&argc, &argv);

    init_app();

    if(chat_data != NULL){
        free(chat_data);
    }

    return 0;
}