#include "console.h"

static int nfds;
static int listen_fd;
static struct pollfd * fds;
static console_status_t status;
static connection_status_t connection;

static int is_uint8(char * val)
{
    int oct = atoi(val);
    if (oct < 0 || oct > 255) return 0;
    return 1;
}

static int is_addr(char * host)
{
    char * delim = NULL;
    delim = strtok(host, ".");
    if (NULL == delim || !is_uint8(delim)) return 0;

    delim = strtok(NULL, ".");
    if (NULL == delim || !is_uint8(delim)) return 0;

    delim = strtok(NULL, ".");
    if (NULL == delim || !is_uint8(delim)) return 0;

    delim = strtok(NULL, ".");
    if (NULL == delim || !is_uint8(delim)) return 0;

    delim = strtok(NULL, ".");
    if (NULL != delim) return 0;

    if (0 == strcmp(host, "127.0.0.1")) return 0;

    return 1;
}

static inline int console_add_connection(int fd)
{
    void * realloc_protect = (void *) fds;

    fds = (struct pollfd *) realloc((void *) fds, ++nfds * sizeof(struct pollfd));
    if (!fds)
    {
        free(realloc_protect);
        fds = NULL;
        return FAILED;
    }
    fds[nfds-1].fd = fd;
    fds[nfds-1].events = POLLIN;

    if (connection == DISCONNECTED) connection = CONNECTED;

    return SUCCESS;
}

static void connect_cmd(char * cmd)
{
    strtok(cmd, " ");
    char * host = strtok(NULL, " ");
    if (NULL == host)
    {
        printf("Use /connect <host>\n");
        return;
    }
    char * host_verify = strdup(host); 
    if (!is_addr(host_verify))
    {
        printf("Invalid address\n");
        free(host_verify);
        return;
    }
    free(host_verify);

    int new_fd;
    int rc = net_open_tcp_socket(&new_fd);
    if (0 != rc)
    {
        printf("Connect failed 1\n");
        return;
    }
    struct sockaddr addr;
    rc = net_fill_in_addr(&addr, host, LISTEN_PORT);
    if (0 != rc)
    {
        printf("Connect failed 2\n");
        return;
    }

    rc = net_connect_socket(new_fd, &addr);
    if (0 != rc)
    {
        printf("Connect failed 3\n");
        return;
    }
    rc = console_add_connection(new_fd);
    if (SUCCESS != rc)
    {
        close(listen_fd);
        return;
    }

    printf("Connection to %s:%d estabilished\n", host, LISTEN_PORT);
}

static void exit_cmd(char * cmd)
{
    if (status == CONSOLE_STOPPING) return;

    status = CONSOLE_STOPPING;
    close(listen_fd);
    int i;
    for (i = NUM_FDS; i < nfds; i++)
    {
        if (fds[i].fd) close(fds[i].fd);
    }
    if (fds) free(fds);
}

static void version_cmd(char * cmd)
{
    printf("Version: %s\n", VERSION);
}

static void status_cmd(char * cmd)
{
    printf("App:\t\t%s\n", (status == CONSOLE_RUNNING) ? "RUNNING" : "STOPPED");
    printf("Net:\t\t%s\n", (connection == CONNECTED) ? "ESTABILISHED" : "NOT ESTABILISHED");
}

static void unknow_cmd(char * cmd)
{
    printf("Unknow command: \"%s\"\nUse /help\n", &cmd[1]);
}

static void help_cmd(char * cmd)
{
    printf("Avialable commands:\n\tconnect <host>\n\tstatus\n\tversion\n\thelp\n\texit\n");
}

static void console_cmd_handle(char * cmd)
{
    if (NULL == cmd) return;

    if(strstr(cmd, "connect"))
        connect_cmd(cmd);
    else if(strstr(cmd, "status"))
        status_cmd(cmd);
    else if(strstr(cmd, "ver"))
        version_cmd(cmd);
    else if(strstr(cmd, "help"))
        help_cmd(cmd);
    else if(strstr(cmd, "exit"))
        exit_cmd(cmd);
    else
        unknow_cmd(cmd);
}

static int is_cmd(char * cmd)
{
    if (cmd[0] == '/') return 1;
    return 0;
}

static void console_msg_handle(char * msg)
{
    if (connection == DISCONNECTED)
    {
        printf("Connection not estabilished\n");
        return;
    }

    message_t * message = (message_t *) malloc(sizeof(header_t) + strlen(msg)+1);
    if (message == NULL)
    {
        printf("Cannot send message\n");
        return;
    }

    memset(message, 0, sizeof(*message));
    message->header.length = strlen(msg)+1;
    memcpy(message->payload, msg, strlen(msg)+1);
    int rc;
    int i;
    for (i = NUM_FDS; i < nfds; i++)
    {
        if (!fds[i].fd) continue;
        rc = net_send_message(fds[i].fd, message);
        if (rc != SUCCESS)
        {
            printf("Error sending message\n");
        }
    }
    free(message);
}

static void incoming_message_handle(message_t * message)
{
    printf("%s\n", message->payload);
}

/* askii only */
static void console_welcome(char * target)
{
    if (!target) return;
    srand(getpid());
    char * range = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPRSTUVWXYZ !@#$%^&*()_+-=<>,.";
    char str[strlen(target)];
    memset(str, 0 , sizeof(str));

    putchar('\t');
    fflush(stdout);
    int i;
    for (i = 0; i < strlen(target); i++)
    {
        do
        {
            str[i] = range[rand()%strlen(range)];
            putchar(str[i]);
            fflush(stdout);
            usleep(1000);
            if (target[i] != str[i]) putchar('\b');
            else break;
        } while (1);
    }
    printf("\n\n> ");
    fflush(stdout);
}

void console_init()
{
    console_welcome(NAME);
    fds = (struct pollfd *) calloc(NUM_FDS, sizeof(struct pollfd));
    if (!fds) return;

    rcode_t rc = net_open_tcp_socket(&listen_fd);
    if (rc != SUCCESS)
    {
        free(fds);
        return;
    }

    rc = net_set_socket_nonblock(listen_fd);
    if (rc != SUCCESS)
    {
        close(listen_fd);
        free(fds);
        return;
    }

    rc = net_set_socket_reuse(listen_fd);
    if (rc != SUCCESS)
    {
        close(listen_fd);
        free(fds);
        return;
    }

    struct sockaddr addr;
    rc = net_fill_in_addr(&addr, LISTEN_ADDR, LISTEN_PORT);
    if (rc != SUCCESS)
    {
        close(listen_fd);
        free(fds);
        return;
    }

    rc = net_bind_socket(listen_fd, &addr);
    if (rc != SUCCESS)
    {
        close(listen_fd);
        free(fds);
        return;
    }

    rc = net_listen_fd(listen_fd, 5);
    if (rc != SUCCESS)
    {
        close(listen_fd);
        free(fds);
        return;
    }

    fds[STDIN_FD].fd = 0;
    fds[STDIN_FD].events = POLLIN;

    fds[LISTEN_FD].fd = listen_fd;
    fds[LISTEN_FD].events = POLLIN;

    nfds = NUM_FDS;

    status = CONSOLE_RUNNING;
    connection = DISCONNECTED;
}

void console_loop(void * cookie)
{
    int i;
    int rc = 0;
    char buf[INPUT_MAX_LEN];

    while (CONSOLE_RUNNING == status)
    {
        rc = poll(fds, nfds, POLL_TIMEOUT);
        if (rc == 0) continue;
        if (fds[STDIN_FD].revents & POLLIN)
        {
            memset(buf, 0, sizeof(buf));
            for (i = 0; i < sizeof(buf) && (10 != (buf[i] = getchar()));i++);
            buf[i] = '\0';
            if (is_cmd(buf))
                console_cmd_handle(buf);
            else
                console_msg_handle(buf);
            printf("> ");
            fflush(stdout);
        }
        if (fds[LISTEN_FD].revents & POLLIN)
        {
            int new_fd;
            rc = net_accept_connection(listen_fd, &new_fd);
            if (rc != SUCCESS)
            {
                close(listen_fd);
                free(fds);
                return;
            }
            rc = console_add_connection(new_fd);
            if (rc != SUCCESS)
            {
                close(listen_fd);
                return;
            }
        }
        for (i = NUM_FDS; i < nfds; i++)
        {
            if (fds[i].revents & POLLIN)
            {
                int rc;
                message_t * message = NULL;
                rc = net_recv_message(fds[i].fd, &message);
                if (rc != SUCCESS)
                {
                    if (message) free(message);
                    continue;
                }
                incoming_message_handle(message);
                printf("> ");
                fflush(stdout);
                if (message) free(message);
            }
        }
        if (rc < 0) return;
    }
    return;
}

void console_deinit()
{
    exit_cmd(NULL);
}