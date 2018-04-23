#include "net.h"
#include <poll.h>
#include "../view/view.h"

int main(int argc, char *argv[])
{
    int32_t fd;
    int32_t ln_fd;
    struct sockaddr addr;

    time_t ticks; 

    int32_t rc;
    rc = net_open_tcp_socket(&ln_fd);
    if (rc)
    {
        printf("net_open_tcp_socket failed\n");
        return FAILED;
    }

    rc = net_set_socket_reuse(ln_fd);
    if (rc)
    {
        printf("net_set_socket_reuse failed\n");
        return FAILED;
    }

    rc = net_fill_in_addr(&addr, argv[1], 33000);
    if (rc)
    {
        printf("net_fill_in_addr failed\n");
        return FAILED;
    }

    rc = net_bind_socket(ln_fd, &addr);
    if (rc)
    {
        printf("net_bind_socket failed\n");
        return FAILED;
    }

    rc = net_listen_fd(ln_fd, 5);
    if (rc)
    {
        printf("net_listen_fd failed\n");
        return FAILED;
    }

    rc = net_accept_connection(ln_fd, &fd);
    if (rc)
    {
        printf("net_accept_connection failed\n");
        return FAILED;
    }

    struct pollfd fds2[2];
    fds2[0].fd = fd;
    fds2[0].events = POLLIN;
    fds2[1].fd = STDIN_FILENO;
    fds2[1].events = POLLIN;
    char name[128]="хз кто";
    while (1)
    {
        rc = poll(fds2, 2, 1);
        if (rc > 0)
        {
            if (fds2[0].revents & POLLIN)
            {
                message_t * message;
                rc = net_recv_message(fd, &message);
                if (SUCCESS != rc)
                {
                    printf("net_recv_message failed\n");
                    continue;
                }
                if (message->header.type == MESSAGE_TYPE_TEXT)
                    printf("[%s]:%s\n", message->payload);
                if (message->header.type == MESSAGE_TYPE_HELLO)
                {
                    printf("you talk with %s\n", message->payload);
                    sprintf(name, "%s", message->payload);
                }
                free(message);
            }
            if (fds2[1].revents & POLLIN)
            {
                char buf[1024] = "";
                message_t * message = malloc(sizeof(header_t)+strlen(buf));
                memcpy(message->payload, buf, strlen(buf));
                message->header.length = strlen(buf);

                if (check_for_command(message))
                {
                    free(message);
                    continue;
                }
                rc = net_send_message(fd, message);
                if (SUCCESS != rc)
                {
                    printf("net_send_message failed\n");
                    continue;
                }
                free(message);
            }
        }
    }

    rc = net_close_socket(fd);
    if (rc)
    {
        printf("net_write_data failed\n");
        return FAILED;
    }

    rc = net_close_socket(ln_fd);
    if (rc)
    {
        printf("net_write_data failed\n");
        return FAILED;
    }

    return SUCCESS;
}