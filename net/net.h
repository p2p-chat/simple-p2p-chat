#ifndef NET_H
#define NET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <poll.h>

#define MAX_PAYLOAD_LEN 1024 /* max possible udp packet length */

/* move in global config */
typedef enum
{
    SUCCESS,
    FAILED
} rc_t;

typedef struct __attribute__((packed))
{
    uint32_t length;
    uint8_t  type;  // reserved
    uint8_t  flags; // reserved
    uint8_t  rcode; // reserved
} header_t;

typedef struct __attribute__((packed))
{
    header_t header;
    uint8_t  payload[0];
} message_t;

typedef struct peer_s
{
    uint8_t           id;   // reserved
    int32_t           fd;   // listened fd
    struct sockaddr * addr; // addr this peer (for tx)
    struct peer_s   * next; // reserved
} peer_t;

typedef struct
{
    int fd; // fot tx
    peer_t * peer;
} net_t;

// int  open_socket(int * fd);
// int  close_socket(int * fd);
// void fill_inet_sockaddr_rx(net_t * net, char * host, uint16_t port);
// void fill_inet_sockaddr_br(net_t * net, char * host, uint16_t port);
// int  bind_rx_fd(net_t * net);
// int  bind_br_fd(net_t * net);
// int  send_message(net_t * net, uint8_t flags, void * data, uint16_t data_len);
// int  recv_message(net_t * net, void ** data);
rc_t net_add_peer(net_t * net, char * host, uint16_t port);
rc_t net_del_peer(net_t * net, uint8_t id);

rc_t net_send_message(net_t * net, uint8_t id, message_t * message);
rc_t net_recv_message(net_t * net, uint8_t id, message_t ** message);

#endif /* NET_H */  