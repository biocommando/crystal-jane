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

struct game_settings create_game_settings()
{
	struct game_settings g;
	g.final_level = 15;
	g.enable_sprint = 1;
	g.enable_high_jump = 1;
	g.enable_weapon = 1;
	g.jump_height = 6;
	g.high_jump_height = 10;
	g.max_lives = 8;
	g.time_counter_limit = 350;

	g.start_level = 0;
	g.skip_splash_screen = 0;
	g.repeat_level = 0;
	g.is_mod = 0;
	return g;
}