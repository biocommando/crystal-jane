#include "allegro_compat.h"
#include "crystal.h"
#include "game_data.h"
#include "synth.h"
#include "common.h"
#include "graphics.h"

#include <stdio.h>
#include <math.h>

#include "build_info.h"

EXTERN_GLOBALS;

#define FROM_GAME_SETTINGS(x)                                        \
	do                                                               \
	{                                                                \
		gmsettings.x = read_game_data_setting(#x, gmsettings.x); \
		verbose_log("Game setting: " #x " = %d\n", gmsettings.x);  \
	} while (0)

int main(int argc, char **argv)
{
	struct game_settings gmsettings = create_game_settings();
	printf(
		"CRYSTAL JANE\n\nversion " VERSION " - Built on " BUILD_DATE
		"\nCopyrights 2004-2022 by Joonas Salonpaa\nMIT licensed\n");
	if (GET_ARG('v'))
	{
		verbose_logging = 1;
		verbose_log("Verbose logging enabled\n");
	}
	if (GET_ARG('h'))
	{
		printf(
			"\n"
			"Available command line arguments:\n\n"
			"  command   | description\n"
			"--------------------------------------------------------------------------------\n"
			"    h       | Show this help\n"
			"    m       | Disable music\n"
			"    s       | Disable sound\n"
			"            | Note: if both m and s are given the sound driver won't be loaded\n"
			"            | at all.\n"
			"    w       | Wait for ENTER press before anything.\n"
			"            | Makes it easier for screen recording to capture also the beginning\n"
			"            | of the game.\n"
			"    L[NUM]  | Skip to [NUM] level (1...15).\n"
			"            | When this option is selected highscore is not saved.\n"
			"    r       | Enable replay more that prevents level progression\n"
			"    S       | Disable ending splash screen\n"
			"    v       | Enable verbose logging\n"
			"    g[FILE] | Change game data file to [FILE] (default 'gdat.dat')\n"
			"    a[SIZE] | Audio buffer size (2^n), default 1024\n");
		return 0;
	}

	printf("Run with '%s h' to see help on command line options\n", argv[0]);

	if (GET_ARG('g'))
	{
		printf("Change game data file to %s\n", GET_ARG('g'));
		if (set_game_data_file_name(GET_ARG('g')))
		{
			printf("Error changing game data file\n");
			return 0;
		}
		gmsettings.is_mod = 1;
	}
	
	if (GET_ARG('L'))
	{
		sscanf(GET_ARG('L'), "%d", &gmsettings.start_level);
		gmsettings.start_level -= 1;
		if (gmsettings.start_level >= gmsettings.final_level || gmsettings.start_level < 0)
			gmsettings.start_level = 0;
		printf("Start level changed to %d\n", gmsettings.start_level + 1);
	}

	gmsettings.skip_splash_screen = !!GET_ARG('S');
	gmsettings.repeat_level = !!GET_ARG('r');

	FROM_GAME_SETTINGS(final_level);
	FROM_GAME_SETTINGS(enable_high_jump);
	FROM_GAME_SETTINGS(enable_sprint);
	FROM_GAME_SETTINGS(enable_weapon);
	FROM_GAME_SETTINGS(jump_height);
	FROM_GAME_SETTINGS(high_jump_height);
	FROM_GAME_SETTINGS(max_lives);
	FROM_GAME_SETTINGS(time_counter_limit);

	if (GET_ARG('s'))
	{
		set_sfx_off(GET_ARG('m') ? OPT_ALL_SOUND_OFF : OPT_SFX_OFF);
	}

	if (GET_ARG('x'))
	{
		sscanf(GET_ARG('x'), "%d", &scaling);
		if (!(scaling > 0 && scaling <= 8))
			scaling = 1;
		printf("Changed scaling to %d\n", scaling);
	}

	int audio_buf_size_override = 0;

	if (GET_ARG('a'))
	{
		sscanf(GET_ARG('a'), "%d", &audio_buf_size_override);
		printf("Using audio buffer size %d\n", audio_buf_size_override);
	}

	if (init_allegro(scaling, audio_buf_size_override))
	{
		printf("Init allegro failed");
		return 0;
	}

	if (GET_ARG('w'))
	{
		wait_key_press(ALLEGRO_KEY_ENTER);
	}

	if (GET_ARG('m'))
	{
		set_music(0);
	}
	else
	{
		synth_init(SYNTH_SETTINGS);
		set_music(2);
	}

	clear_screen_for_text();
	char story[1000];
	get_story(story, sizeof(story), '0');
	screen_printf("%s\n\nPRESS ENTER\n", story);
	FLIP;
	wait_key_press(ALLEGRO_KEY_ENTER);
	set_sfx(20, 30, 40, 50);
	clear_screen_for_text();

	get_story(story, sizeof(story), '1');
	screen_printf("%s\n\nPRESS ENTER\n", story);
	FLIP;
	wait_key_press(ALLEGRO_KEY_ENTER);
	set_sfx(20, 30, 40, 50);
	clear_screen_for_text();
	get_story(story, sizeof(story), '2');
	screen_printf("%s\n\nPRESS ENTER\n", story);
	FLIP;
	wait_key_press(ALLEGRO_KEY_ENTER);

    read_sprites();

	while (!game_logic(gmsettings)) {}
}
