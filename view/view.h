#include <stdint.h>
#include "../net/message.h"
void wrong_msg();
void version();
void cmd_help();
void name(message_t * message);
int cmd_handle(message_t *message);
int check_for_command(message_t * message);