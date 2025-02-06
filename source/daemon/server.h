#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <stdint.h>

struct server_s {
    int socket;
    struct sockaddr_in address;
};
typedef struct server_s Server;


void init_server(Server *server, uint16_t port);
void destroy_server(Server *server);

void server_bind(Server *server);
void server_listen(Server *server, int max_connections);
int server_accept(Server *server);

#endif // SERVER_H