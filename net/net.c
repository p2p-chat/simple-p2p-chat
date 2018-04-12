#include "net.h"

rcode_t net_open_tcp_socket(int32_t * fd_out)
{
    if (!fd_out) return FAILED;

    int32_t fd_in;
    fd_in = socket(AF_INET, SOCK_STREAM, 0);
    if (!fd_in) return FAILED;

    *fd_out = fd_in;

    return SUCCESS;
}

rcode_t net_open_udp_socket(int32_t * fd_out)
{
    if (!fd_out) return FAILED;

    int32_t fd_in;
    fd_in = socket(AF_INET, SOCK_DGRAM, 0);
    if (!fd_in) return FAILED;

    *fd_out = fd_in;

    return SUCCESS;
}

rcode_t net_close_socket(int32_t fd)
{
    if (!fd) return FAILED;

    close(fd);
    fd = -1;

    return SUCCESS;
}

rcode_t net_fill_in_addr(struct sockaddr * addr_out, char * host, uint16_t port)
{
    if (!addr_out) return FAILED;

    struct sockaddr_in addr_in;
    memset(&addr_in, 0, sizeof(addr_in));

    addr_in.sin_family = AF_INET;
    addr_in.sin_addr.s_addr = inet_addr(host);
    addr_in.sin_port = htons(port);

    memcpy(addr_out, &addr_in, sizeof(addr_in));

    return SUCCESS;
}

rcode_t net_set_socket_reuse(int32_t fd)
{
    if (!fd) return FAILED;

    int32_t rc;
    rc = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
    rc = setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &(int){ 1 }, sizeof(int));
    if (0 != rc) return FAILED;

    return SUCCESS;
}

rcode_t net_set_socket_nonblock(int32_t fd)
{
    if (!fd) return FAILED;

    int32_t rc;
    rc = fcntl(fd, F_SETFL, O_NONBLOCK);
    if (0 != rc) return FAILED;

    return SUCCESS;
}

rcode_t net_fill_un_addr(struct sockaddr * addr_out, char * path)
{
    if (!addr_out) return FAILED;

    struct sockaddr_un * addr_in = (struct sockaddr_un *) addr_out;
    memset(addr_in, 0, sizeof(*addr_in));

    addr_in->sun_family = AF_UNIX;
    sprintf(addr_in->sun_path, "%s", path);

    memcpy(addr_out, &addr_in, sizeof(addr_in));

    return SUCCESS;
}

rcode_t net_bind_socket(int32_t fd, struct sockaddr * addr)
{
    if (!fd) return FAILED;
    if (!addr) return FAILED;

    int32_t rc;
    rc = bind(fd, addr, sizeof(struct sockaddr));
    if (0 != rc) return FAILED;

    return SUCCESS;
}

rcode_t net_listen_fd(int32_t fd, uint16_t backlog)
{
    if (!fd) return FAILED;

    int32_t rc;
    rc = listen(fd, backlog);
    if (0 != rc) return FAILED;

    return SUCCESS;
}

rcode_t net_accept_connection(int32_t fd, int32_t * fd_out)
{
    if (!fd_out) return FAILED;

    int32_t fd_in;
    fd_in = accept(fd, NULL, NULL);
    if (!fd_in) return FAILED;

    *fd_out = fd_in;
    return SUCCESS;
}

rcode_t net_connect_socket(int32_t fd, struct sockaddr * addr)
{
    if (!fd) return FAILED;
    if (!addr) return FAILED;

    int32_t rc;
    rc = connect(fd, addr, sizeof(*addr));
    if (0 != rc) return FAILED;

    return SUCCESS;
}

rcode_t net_write_data(int32_t fd, void * input, uint32_t input_len)
{
    if (!fd) return FAILED;
    if (!input) return FAILED;
    if (!input_len) return FAILED;

    int32_t rc;
    rc = write(fd, input, input_len);
    if (!rc) return FAILED;

    return SUCCESS;
}

rcode_t net_read_data(int32_t fd, void * output, uint32_t output_len)
{
    if (!fd) return FAILED;
    if (!output) return FAILED;
    if (!output_len) return FAILED;

    int32_t rc;
    rc = read(fd, output, output_len);
    if (!rc) return FAILED;

    return SUCCESS;
}