#include "console.h"

static pthread_t net_tid;
static struct pollfd stdin_pollfd;
static console_status_t status;

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

    rcode_t rc = net_connection_add(host);
    if (SUCCESS != rc)
    {
        printf("net_connection_add failed\n");
        return;
    }

    printf("Connection to %s:%d estabilished\n", host, LISTEN_PORT);
}

static void exit_cmd(char * cmd)
{
    if (status == CONSOLE_STOPPING) return;

    status = CONSOLE_STOPPING;
    net_thread_stop();
}

static void version_cmd(char * cmd)
{
    printf("Version: %s\n", VERSION);
}

static void status_cmd(char * cmd)
{
    printf("App:\t\t%s\n", (status == CONSOLE_RUNNING) ? "RUNNING" : "STOPPED");
    char * result = NULL;
    net_get_connection_status(&result);
    if (NULL != result)
    {
        printf("%s\n", result);
        free(result);
    }
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
    rcode_t rc = net_message_send(msg);
    if (SUCCESS != rc)
    {
        printf("Cannot send message\n");
    }
}

static void incoming_message_handle(char * message)
{
    printf("%s\n", message);
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

static void signal_handler(int signo)
{
    if (signo == SIGTERM)
    {
        printf("/exit\n");
        exit_cmd(NULL);
    }
}

void console_init()
{
    console_welcome(NAME);

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler =  signal_handler;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGTERM);
    sigaction(SIGTERM, &sa, NULL );

    sigset_t sset;
    sigemptyset(&sset);
    sigaddset(&sset, SIGINT);
    sigprocmask(SIG_BLOCK, &sset, 0);

    stdin_pollfd.fd = 0;
    stdin_pollfd.events = POLLIN;

    status = CONSOLE_RUNNING;

    pthread_create(&net_tid, NULL, net_thread_routine, NULL);
}

void console_loop(void * cookie)
{
    int i;
    int rc = 0;
    char buf[INPUT_MAX_LEN];

    while (CONSOLE_RUNNING == status)
    {
        rc = poll(&stdin_pollfd, 1, POLL_TIMEOUT);
        if (rc == 0) continue;
        if (rc < 0) return;
        if (stdin_pollfd.revents & POLLIN)
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
        rc = net_get_message(buf);
        if (rc != SUCCESS) continue;
        incoming_message_handle(buf);
    }
    return;
}

void console_deinit()
{
    exit_cmd(NULL);
}