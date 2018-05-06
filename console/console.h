#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#include "../net/net.h"

#define NAME           "Crypto P2P Chat"
#define VERSION        "0.0.0.3"
#define POLL_TIMEOUT    1000
#define INPUT_MAX_LEN   1024
#define LISTEN_ADDR     "0.0.0.0"
#define LISTEN_PORT     33000

typedef enum
{
    CONSOLE_RUNNING,
    CONSOLE_STOPPING
} console_status_t;

typedef enum
{
    CONNECTED,
    DISCONNECTED
} connection_status_t;

enum pollfds
{
	STDIN_FD,
	LISTEN_FD,
	NUM_FDS
};

void console_init();
void console_loop();
void console_deinit();

#endif /* CONSOLE_H */