#include "net.h"

static int nfds = 0;
static int running = 0;
static struct pollfd * connections;
static int message_counter = 0;
static char messages[128][1024];

rcode_t net_get_connection_status(char ** status)
{
    if (sizeof(*connections) <= sizeof(struct pollfd))
    {
        asprintf(status, "Net:\t\tLISTENING\n");
        return SUCCESS;
    }
    int i, cnt = 0;
    for (i = 1; i < nfds; i++)
    {
        if (!connections[i].fd) continue;
        cnt++;
    }
    asprintf(status, "Net:\t\tHAVE %d CONNECTIONS\n", cnt);
    return SUCCESS;
}

rcode_t net_message_send(char * msg)
{
    message_t * message = (message_t *) malloc(sizeof(header_t) + strlen(msg)+1);
    if (message == NULL)
    {
        printf("Cannot send message\n");
        return FAILED;
    }

    memset(message, 0, sizeof(*message));
    message->header.length = strlen(msg)+1;
    memcpy(message->payload, msg, strlen(msg)+1);
    int rc;
    int i;
    for (i = 1; i < nfds; i++)
    {
        if (!connections[i].fd) continue;
        rc = net_send_message(connections[i].fd, message);
        if (rc != SUCCESS)
        {
            printf("Error sending message\n");
        }
    }
    free(message);
    return SUCCESS;
}

rcode_t net_get_message(char * message)
{
    if (message_counter == 0) return FAILED;
    memcpy(message, messages[message_counter], 1024);
    memset(messages[message_counter], 0, 1024);
    message_counter--;
    return SUCCESS;
}

static void net_put_message(uint8_t * message, uint32_t length)
{
    if (message_counter == 1024) return;
    memcpy(messages[message_counter], message, length);
    message_counter++;
}

rcode_t net_thread_stop()
{
    if (running) running--;
    return SUCCESS;
}

rcode_t net_connection_add(char * host)
{
    int new_fd;
    int rc = net_open_tcp_socket(&new_fd);
    if (0 != rc)
    {
        return FAILED;
    }
    struct sockaddr addr;
    rc = net_fill_in_addr(&addr, host, LISTEN_PORT);
    if (0 != rc)
    {
        return FAILED;
    }

    rc = net_connect_socket(new_fd, &addr);
    if (0 != rc)
    {
        return FAILED;
    }

    void * realloc_protect = (void *) connections;
    connections = (struct pollfd *) realloc((void *) connections, ++nfds * sizeof(struct pollfd));
    if (!connections)
    {
        free(realloc_protect);
        connections = NULL;
        return CRITICAL;
    }

    connections[nfds-1].fd = new_fd;
    connections[nfds-1].events = POLLIN;

    return SUCCESS;
}

static int net_init_listening()
{
    connections = (struct pollfd *) calloc(1, sizeof(struct pollfd));
    if (!connections) return CRITICAL;

    rcode_t rc = net_open_tcp_socket(&connections[0].fd);
    if (rc != SUCCESS)
    {
        free(connections);
        return CRITICAL;
    }

    rc = net_set_socket_nonblock(connections[0].fd);
    if (rc != SUCCESS)
    {
        close(connections[0].fd);
        free(connections);
        return CRITICAL;
    }

    rc = net_set_socket_reuse(connections[0].fd);
    if (rc != SUCCESS)
    {
        close(connections[0].fd);
        free(connections);
        return CRITICAL;
    }

    struct sockaddr addr;
    rc = net_fill_in_addr(&addr, LISTEN_ADDR, LISTEN_PORT);
    if (rc != SUCCESS)
    {
        close(connections[0].fd);
        free(connections);
        return CRITICAL;
    }

    rc = net_bind_socket(connections[0].fd, &addr);
    if (rc != SUCCESS)
    {
        close(connections[0].fd);
        free(connections);
        return CRITICAL;
    }

    rc = net_listen_fd(connections[0].fd, 5);
    if (rc != SUCCESS)
    {
        close(connections[0].fd);
        free(connections);
        return CRITICAL;
    }

    return SUCCESS;
}

void * net_thread_routine(void * cookie)
{
    rcode_t rc = net_init_listening();
    if (SUCCESS != rc)
    {
        if (connections[0].fd) close(connections[0].fd);
        free(connections);
        return NULL;
    }

    int i;
    running++;
    while (running)
    {
        rc = poll(connections, nfds, 1000);
        if (0 == rc) continue;
        if (rc < 0) return NULL; //todo free resourses

        if (connections[0].revents & POLLIN)
        {
            int new_fd;
            rc = net_accept_connection(connections[0].fd, &new_fd);
            if (rc == CRITICAL) return NULL;
            if (rc == FAILED)   continue;

            connections = (struct pollfd *) realloc((void *) connections, ++nfds * sizeof(struct pollfd));
            if (!connections) return NULL; //todo free resourses

            connections[nfds-1].fd = new_fd;
            connections[nfds-1].events = POLLIN;
        }

        for (i = 1; i < nfds; i++)
        {
            if (connections[i].revents & POLLIN)
            {
                int rc;
                message_t * message = NULL;
                rc = net_recv_message(connections[i].fd, &message);
                if (rc != SUCCESS)
                {
                    if (message) free(message);
                    continue;
                }
                if (message->header.length != 0) net_put_message(message->payload, message->header.length);
                if (message) free(message);
            }
        }
    }

    return NULL;
}