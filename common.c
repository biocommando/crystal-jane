#include "common.h"
#include <stdio.h>
const char *get_arg(int argc, char **argv, char flag)
{
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == flag)
			return argv[i] + 1;
	}
	return NULL;
}
