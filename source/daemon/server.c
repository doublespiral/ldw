#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include "server.h"


void destroy_server(Server *server)
{
    close( server->socket );
}


void init_server(Server *server, uint16_t port)
{
    server->socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if( server->socket == -1 )
    {
        perror( "Socket failed" );
        exit( 1 );
    }

    server->address.sin_family = AF_INET;
    server->address.sin_addr.s_addr = INADDR_ANY;
    server->address.sin_port = htons( port );
    printf("INFO: Server listening on port: %d", port);
}


void server_bind(Server *server)
{
    int bound = bind(server->socket, (struct sockaddr*)&server->address, sizeof(server->address));
    if( bound == -1 )
    {
        perror( "Bind failed" );
        close( server->socket );
        exit( 1 );
    }
}


void server_listen(Server *server, int max_connections)
{
    if( listen(server->socket, max_connections) == -1 )
    {
        perror( "Listen failed" );
        destroy_server( server );
        exit( 1 );
    }
}


int server_accept(Server *server)
{
    uint64_t address_size = sizeof(server->address);
    int result = accept(
        server->socket,
        (struct sockaddr *) &server->address,
        (socklen_t *) &address_size
    );

    if( result == -1 )
    {
        perror("Failed to accept connection");
        destroy_server( server );
        exit( 1 );
    }

    return result;
}