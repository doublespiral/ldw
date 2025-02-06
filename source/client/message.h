#ifndef CLIENT_H
#define CLIENT_H

#include "ldw.h"


void send_message(enum ldw_e_daemon_message message_value, unsigned short port);


#endif // CLIENT_H