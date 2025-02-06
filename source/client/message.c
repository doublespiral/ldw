#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ldw.h"



struct s_client {
    int socket;
};
typedef struct s_client Client;


void destroy_client(Client *client)
{
    close( client->socket );
}


void init_client(Client *client)
{
    client->socket = socket(AF_INET, SOCK_STREAM, 0);
    if( client->socket == -1 )
    {
        perror( "Client socket failed" );
        exit( 1 );
    }
}


void client_connect(Client *client, struct sockaddr_in server)
{
    int connected = connect(
        client->socket, 
        (struct sockaddr *) &server,
        sizeof( server )
    );
    if( connected == -1 )
    {
        perror( "Failed to connect" );
        destroy_client( client );
        exit( 1 );
    }
}


void client_send_message(Client *client, const char *message)
{
    send(client->socket, message, strlen(message), 0);
    shutdown(client->socket, SHUT_WR);

    // FIXME: you shouldnt busy loop here, do that sys call that tells you
    //        when you can continue executing
    for(;;) {
        bool successfully_sent = (read(client->socket, NULL, 0) == 0);

        if( successfully_sent )
            break;

        else {
            perror("Reading");
            exit(1);
        }
    }
}



void send_message(enum ldw_e_daemon_message message_value, unsigned short port)
{
    Client client = {0};
    init_client( &client );

    struct sockaddr_in server_address = {0};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons( port );

    if( inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) != 1)
    {
        perror( "inet_pton() failed" );
        destroy_client( &client );
        exit( 1 );
    }

    client_connect(&client, server_address);

    const char *message = ldw_dm_lookup( message_value );
    client_send_message(&client, message);

    destroy_client( &client );
}