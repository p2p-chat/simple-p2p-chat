#include "console.h"

int main(int argc, char const ** argv)
{
	console_init();
	console_loop();
	console_deinit();
	return 0;
}