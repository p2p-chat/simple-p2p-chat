#include <stdio.h>
#include <string.h>

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

int cmd_handle(char *cmd)
{
	if (!strcmp("/help", cmd))
	{
		cmd_help();
		return 0;
	}
	if (!strcmp("/version", cmd))
	{
		version();
		return 0;
	}
	wrong_msg();
	return 0;
}

int check_for_command(char *cmd)
{
	switch(cmd[0])
	{
		case '/':
			if (strlen(cmd) < 3) wrong_msg();
			else cmd_handle(cmd);
			return 1;
		default :
			return 0;
	}
}

// для тестирования
void main()
{
	while(1)
	{
		char cmd[120];
		scanf("%s", cmd);
		if (check_for_command(cmd))
			continue;
	}
}
