#ifndef NET_H
#define NET_H

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>

#include "message.h"

typedef enum
{
    SUCCESS,
    FAILED
} rcode_t;

rcode_t net_open_tcp_socket(int32_t * fd_out);
rcode_t net_open_udp_socket(int32_t * fd_out);
rcode_t net_close_socket(int32_t fd);
rcode_t net_set_socket_reuse(int32_t fd);
rcode_t net_set_socket_nonblock(int32_t fd);
rcode_t net_fill_in_addr(struct sockaddr * addr_out, char * host, uint16_t port);
rcode_t net_bind_socket(int32_t fd, struct sockaddr * addr);
rcode_t net_listen_fd(int32_t fd, uint16_t backlog);
rcode_t net_accept_connection(int32_t fd, int32_t * fd_out);
rcode_t net_connect_socket(int32_t fd, struct sockaddr * addr);
rcode_t net_send_message(int32_t fd, message_t * message);
rcode_t net_recv_message(int32_t fd, message_t ** message);

#endif /* NET_H */