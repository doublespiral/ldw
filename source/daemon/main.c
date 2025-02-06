#if 0
//#include <stdio.h>
//#include <stdlib.h>
//#include <stdint.h>
//#include <unistd.h>
//#include <string.h>
//#include <stdbool.h>
//#include <pthread.h>
//
//#include <sys/socket.h>
//#include <netinet/in.h>

#include "server.h"
//#include "xwindow.h"


#define MAX_IN_SIZE 1024


//bool daemon_running;


void *render_deez_plugins_loop()
{
    while( daemon_running == true )
    {
        (void)0;
        puts("hi");
    }
}

void handle_clients_loop(Server *server)
{
    char in_buffer[MAX_IN_SIZE] = {0};
    while( daemon_running )
    {
        int client_socket = server_accept( server );

        printf("Connection accepted...\n");

        read(client_socket, in_buffer, MAX_IN_SIZE);
        in_buffer[MAX_IN_SIZE-1] = '\0';
        printf("receieved message from client:\n");
        printf("%s\n", in_buffer);

        close( client_socket );
    }
}



// wow, all things considered this is pretty concise
int main(void)
{
    server server = {0};
    init_server(&server, LDW_PORT);
    server_bind( &server );
    server_listen(&server, 5);

    GLXFBConfig framebuffer_config = {0};
    XWindow *xwindow = create_x_window(NULL, &framebuffer_config);
    xwindow_create_opengl_context(xwindow, &framebuffer_config );
    xwindow_show( xwindow );

    pthread_t render_thread;
    pthread_create(&render_thread, NULL, render_deez_plugins_loop, NULL);

    handle_clients_loop( &server );

    destroy_x_window( xwindow );
    destroy_server( &server );

    return 0;
}
#else

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>

#define LDW_USE_LDW_MACROS
#include "ldw.h"

#include "server.h"
#include "xwindow.h"

bool daemon_running = true;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

#define DO_SAFELY(body) \
    do{ \
        pthread_mutex_lock(&mutex); \
        body; \
        pthread_mutex_unlock(&mutex); \
    }while(0) 


void *render_loop(void *arg)
{
     XWindow *xwindow = (XWindow *) arg;

    while( daemon_running )
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glXSwapBuffers(xwindow->display, xwindow->window);
    }

    return NULL;
}


void server_loop(Server *server)
{
    char receieved_buffer[LDW_MAX_MESSAGE_SIZE] = {0};

    while( daemon_running )
    {
        printf("Checking for connection\n");
        int client_socket = server_accept( server );

        printf("Connection accepted...\n");

        read(client_socket, receieved_buffer, LDW_MAX_MESSAGE_SIZE);
        receieved_buffer[LDW_MAX_MESSAGE_SIZE-1] = '\0';
        printf("receieved message from client:\n%s\n", receieved_buffer);

        if( LDW_STRS_EQUAL(receieved_buffer, "LDW_DM_SHUTDOWN") )
            DO_SAFELY( daemon_running = false );

        close( client_socket );
    }
}


// All things considered this is actually pretty concise
int main(void)
{
    Server server = {0};
    init_server(&server, LDW_PORT);
    server_bind( &server );
    server_listen(&server, LDW_MAX_CLIENTS);

    GLXFBConfig framebuffer_config = {0};
    XWindow *xwindow = create_x_window(NULL, &framebuffer_config);
    xwindow_create_opengl_context(xwindow, &framebuffer_config);
    xwindow_show( xwindow );

    pthread_t rendering_thread;
    pthread_create(&rendering_thread, NULL, render_loop, xwindow);

    server_loop( &server );

    pthread_join(rendering_thread, NULL);
    LDW_LOG("All thread loops sucessfully exited\n");

    destroy_x_window( xwindow );
    destroy_server( &server );
}


#endif