#include <stdio.h>
#include <string.h>
#include "view.h"

void wrong_msg()
{
	printf("wrong command, for more information try /help\n");
}

void version()
{
	printf(" p2p-chat v_0.0.0.1 beta\n");
}

void cmd_help()
{
	printf(" /help - show this info\n /version - show version\n");
}

void name(message_t *message)
{
	message->header.type = MESSAGE_TYPE_HELLO;
	memmove(message->payload, message->payload + 5, strlen((char *)message->payload) - 5);
}

int cmd_handle(message_t *message)
{
	if (!strcmp("/help", (char *)message->payload))
	{
		cmd_help();
		return 0;
	}
	if (!strcmp("/version", (char *)message->payload))
	{
		version();
		return 0;
	}
	if (!strcmp("/name", (char *)message->payload))
	{
		name(message);
		return 0;
	}
	wrong_msg();
	return 0;
}

int check_for_command(message_t * message)
{
	switch((char)message->payload[0])
	{
		case '/':
			if (strlen((char *)message->payload) < 3) wrong_msg();
			else cmd_handle(message);
			return 1;
		default :
			message->header.type = MESSAGE_TYPE_TEXT;
			return 0;
	}
}

// для тестирования
// void main()
// {
// 	while(1)
// 	{
// 		char cmd[120];
// 		scanf("%s", cmd);
// 		if (check_for_command(cmd))
// 			continue;
// 	}
// }
