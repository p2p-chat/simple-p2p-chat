#include "net.h"

// static int open_socket(int * fd)
// {
//     int rc;
//     rc = socket(AF_INET, SOCK_DGRAM, 0);
//     if (!rc) return -1;

//     *fd = rc;
//     return 0;
// }

// static int close_socket(int * fd)
// {
//     if (!*fd) return -1;

//     close(*fd);
//     *fd = -1;

//     return 0;
// }

// static inline void fill_sockaddr_inet(struct sockaddr_in * addr, char * host, uint16_t port)
// {
//     addr->sin_family = AF_INET;
//     addr->sin_addr.s_addr = inet_addr(host);
//     addr->sin_port = htons(port);
// }

// int bind_rx_fd(net_t * net)
// {
//     struct sockaddr_in addr;
//     fill_sockaddr(&addr, AF_INET, RX_ADDR, net->rx_port);

//     int rc;
//     rc = bind(net->rx_fd, (const struct sockaddr *) &addr, sizeof(addr));
//     if (rc != 0)
//     {
//         return -1;
//     }

//     return 0;
// }
// rc_t net_add_peer(net_t * net, char * host, uint16_t port);
// rc_t net_del_peer(net_t * net, uint8_t id);

static rc_t find_peer(net_t * net, uint8_t id, int * fd, struct sockaddr ** addr)
{
    if (!net)        return FAILED;
    if (!id)         return FAILED;
    if (!addr)       return FAILED;

    peer_t * peer = net->peer;
    for (; peer && peer->id != id; peer = peer->next);

    if (peer->)
    (peer->fd) ? *fd = peer->fd : return FAILED;
    (peer->addr) ? *addr = peer->addr : return FAILED;

    return SUCCESS;
}

rc_t net_send_message(net_t * net, uint8_t id, message_t * message)
{
    if (!net)     return FAILED;
    if (!id)      return FAILED;
    if (!message) return FAILED;

    int rc;
    int fd;
    struct sockaddr * addr;

    rc = find_peer(net, &fd, id, &addr);
    if (0 != rc) return FAILED;

    header_t * header = &message->header;
    uint16_t addr_len = sizeof(*addr);

    rc = sendto(net->fd, (void *) header, sizeof(*header), 0, addr, (socklen_t) addr_len);
    if (0 > rc) return FAILED;

    if (header->length == 0) return SUCCESS;

    rc = sendto(net->fd, (void *) message->payload, header->length, 0, addr, (socklen_t) addr_len);
    if (!rc) return FAILED;

    return SUCCESS;
}

/* allocates mem */
rc_t net_recv_message(net_t * net,  uint8_t id, message_t ** message)
{
    if (!net)     return FAILED;
    if (!id)      return FAILED;
    if (!message) return FAILED;

    int rc;
    int fd;
    struct sockaddr * addr;

    rc = find_peer(net, id, &fd, &addr);
    if (0 != rc) return FAILED;

    message_t * message_in = (message_t *) malloc(sizeof(header_t));
    if (!message_in) return FAILED;

    rc_t free_and_exit(rc_t rc)
    {
        if (message_in) free(message_in);
        return rc;
    }

    uint16_t addr_len = sizeof(*addr);

    rc = recvfrom(fd, (void *) &message_in->header, sizeof(message_in->header), 0, addr, (socklen_t *) &addr_len);
    if (!rc) return free_and_exit(FAILED);

    uint32_t len = message_in->header.length;
    if (len == 0) return SUCCESS;

    void * tmp = message_in;
    tmp = realloc(tmp, sizeof(*message_in) + len);
    if (!tmp) return free_and_exit(FAILED);

    rc = recvfrom(fd, &message_in->payload, len, 0, addr, (socklen_t *) &addr_len);
    if (!rc) return free_and_exit(FAILED);

    return SUCCESS;
}