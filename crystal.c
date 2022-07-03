#include "allegro_compat.h"
#include "game_data.h"
#include "synth.h"
#include "common.h"

#include <stdio.h>
#include <math.h>

#include "build_info.h"

int verbose_logging = 0;

extern char keybuffer[ALLEGRO_KEY_MAX];

void draw_box_gradient(int x1, int y1, int x2, int y2, char box_color_top, char box_color_bottom, int symmetric);
void draw_box(int x1, int y1, int x2, int y2, char box_color);

void wait_key_press(int key)
{
	while (!keybuffer[key])
	{
		wait_event();
	}
	while (keybuffer[key])
	{
		wait_event();
	}
}

void clear_screen_to_color(int col)
{
	draw_box(0, 0, 320, 200, col);
}

void clear_screen_for_text()
{
	draw_box_gradient(0, 0, 320, 200, BLUE, BLACK, 0);
	// clear_screen_to_color(0);
	clrscr();
}

#define FLIP al_flip_display()

void sprite_do(int _x, int _y, int area_x, int area_y, char sprite, char zoom_mult);
void sprite_read(char sprite);

char *result, anim[2], on_platform, jump, sound_state = 1;

#define SP_BAT(x) ((x) == 0 ? 'B' : 'b')
#define SP_LEFT_FACING(x) ((x) == 0 ? '<' : '{')
#define SP_RIGHT_FACING(x) ((x) == 0 ? '>' : '}')
#define SP_DIAMOND 'D'
#define SP_STONEMAN 'S'

#define SP_PLAYER_W 7
#define SP_PLAYER_H 16

#define SP_DIAMOND_W 5
#define SP_DIAMOND_H 6

#define SP_BAT_W 11
#define SP_BAT_H 5

#define SP_NUM_PX(sp) (SP_##sp##_W * SP_##sp##_H)

char guy_left1[SP_NUM_PX(PLAYER)], guy_left2[SP_NUM_PX(PLAYER)], guy_right1[SP_NUM_PX(PLAYER)], guy_right2[SP_NUM_PX(PLAYER)], dia_spr[SP_NUM_PX(DIAMOND)], *buf, file_read_buf[20];
char bat1[SP_NUM_PX(BAT)], bat2[SP_NUM_PX(BAT)];
// 7*16 = 112   ;  5*6 = 30

int y, x, lives, diamonds, time_counter, level, count, hiscore = 0;
int score = 0;
int scaling = 3;
int final_level = 15;

// game settings
int enable_sprint = 1, enable_high_jump = 1, enable_weapon = 1,
	jump_height = 6, high_jump_height = 10, max_lives = 8;

#define FROM_GAME_SETTINGS(x)                          \
	do                                                 \
	{                                                  \
		x = read_game_data_setting(#x, x);             \
		verbose_log("Game setting: " #x " = %d\n", x); \
	} while (0)

const char *get_arg(int argc, char **argv, char flag)
{
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == flag)
			return argv[i] + 1;
	}
	return NULL;
}

#define GET_ARG(flag) get_arg(argc, argv, flag)

int main(int argc, char **argv)
{
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
	}

	FROM_GAME_SETTINGS(final_level);
	FROM_GAME_SETTINGS(enable_high_jump);
	FROM_GAME_SETTINGS(enable_sprint);
	FROM_GAME_SETTINGS(enable_weapon);
	FROM_GAME_SETTINGS(jump_height);
	FROM_GAME_SETTINGS(high_jump_height);
	FROM_GAME_SETTINGS(max_lives);

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

	if (!GET_ARG('m'))
	{
		synth_init(SYNTH_SETTINGS);
		extern const char my_synth_sequence[2838];
		set_sequence(my_synth_sequence, 2838);
	}

	clear_screen_for_text();
	screen_printf("HI!\nWelcome to Crystal Jane\n\nStory:\nJane has been ten years in a prison\n");
	screen_printf("of a cruel stoneman.\nOne day stoneman says to free Jane\nif she collects all");
	screen_printf(" the lifecrystals\non that planet...\n\nPRESS ENTER\n");
	FLIP;
	wait_key_press(ALLEGRO_KEY_ENTER);
	set_sfx(20, 30, 40, 50);
	clear_screen_for_text();

	screen_printf(
		"Game controls:\n--------------\n"
		"LEFT / RIGHT: Move left / right\n"
		"RCTRL + LEFT / RIGHT: Sprint left / right\n"
		"UP: Jump\n"
		"RSHIFT: High jump (once in level)\n"
		"DOWN: Use the bat-killing morningstar\n"
		"ENTER: Teleport to beginning of level\n"
		"P: Pause\n\n"
		"Your mission is to collect all\nthe lifecrystals on %d levels.\n\n"
		"PRESS ENTER\n",
		final_level);
	FLIP;
	wait_key_press(ALLEGRO_KEY_ENTER);
	set_sfx(20, 30, 40, 50);
	clear_screen_for_text();
	screen_printf("You can see your lives in the\nupper left corner\nand time running next to it...\nBest of luck for you!\n\nPRESS ENTER\n");
	FLIP;
	wait_key_press(ALLEGRO_KEY_ENTER);
game_logic_start:
	screen_printf("LOADING SPRITES...\n");

	hiscore = get_highscore();

	sprite_read(SP_LEFT_FACING(0));
	sprite_read(SP_LEFT_FACING(1));
	sprite_read(SP_RIGHT_FACING(0));
	sprite_read(SP_RIGHT_FACING(1));
	sprite_read(SP_DIAMOND);
	sprite_read(SP_BAT(0));
	sprite_read(SP_BAT(1));

	lives = max_lives;
	level = 0;

	if (GET_ARG('L'))
	{
		sscanf(GET_ARG('L'), "%d", &level);
		level -= 1;
		if (level >= final_level || level < 0)
			level = 0;
		printf("Start level changed to %d\n", level + 1);
	}

	anim[0] = SP_RIGHT_FACING(0);
	anim[1] = 0;
	diamonds = 0;
	score = 0;

	clear_screen_for_text();
	sprite_do(110, 140, SP_PLAYER_W, SP_PLAYER_H, SP_RIGHT_FACING(0), 2);
	sprite_do(150, 154, SP_DIAMOND_W, SP_DIAMOND_H, SP_DIAMOND, 2);
	sprite_do(190, 135, SP_BAT_W, SP_BAT_H, SP_BAT(0), 2);
	screen_printf("Crystal Jane (c) Upr00ted tree software\n        MENU\n       UP: PLAY\n     DOWN: QUIT\n\n\n\n\n\n\n\n        HISCORE: %d\n", hiscore);
	if (GET_ARG('g'))
	{
		extern char game_data_file_name[256];
		screen_printf("\nmod loaded: %s\n", game_data_file_name);
	}
	FLIP;
	set_sfx(20, 30, 40, 50);
	while (1)
	{
		wait_event();
		if (keybuffer[ALLEGRO_KEY_DOWN])
		{
			synth_init(SYNTH_SETTINGS);
			clear_screen_for_text();

			screen_printf("+********************+\n* THANKS FOR PLAYING *\n*    CRYSTAL JANE    *\n+********************+\n");
			screen_printf("Crystal Jane (c) Upr00ted tree software\nAll comments to: joonas1000@gmail.com\n");
			FLIP;
			if (!GET_ARG('S'))
				wait_delay(60);
			exit(1);
		}

		if (keybuffer[ALLEGRO_KEY_UP])
			break;
	}
	set_sfx(70, 10, 50, 30);

	int weapon = 0;
	int initial_frame_counter = 0;
	int bats_killed = 0;
	int sprint, high_jump;
	struct level_info world;
	while (lives > 0 && !keybuffer[ALLEGRO_KEY_ESCAPE])
	{
		if (diamonds == 0)
		{
			srand(1234);
			weapon = 0;
			int beat_time_frames = get_frame_counter() - initial_frame_counter;
			int time_bonus = 30 * 20 - beat_time_frames;
			if (level != 0)
			{
				clear_screen_for_text();
				if (time_bonus < 0)
					time_bonus = 0;
				bats_killed *= 50;
				score += time_bonus;
				score += bats_killed;
				screen_printf(
					"LEVEL %d COMPLETE!\n\n"
					"COMPLETE TIME    : %.1f SEC\n                   ",
					level, beat_time_frames / 20.0f);
				int best_time = get_best_time_for_level(level - 1);
				if (beat_time_frames < best_time || best_time < 0)
				{
					set_best_time_for_level(level - 1, beat_time_frames);
					screen_printf("NEW BEST TIME!\n");
				}
				else
				{
					screen_printf("BEST: %.1f SEC\n", best_time / 20.0f);
				}
				screen_printf(
					"TIME BONUS       : %d\n\n"
					"BATS KILLED BONUS: %d\n\n"
					"LIVES LEFT       : %d\n\n"
					"TOTAL SCORE      : %d\n\n\n"
					"PRESS ENTER",
					time_bonus, bats_killed, lives, score);
				FLIP;
				wait_key_press(ALLEGRO_KEY_ENTER);
			}
			if (level == final_level)
			{
				clear_screen_for_text();
				anim[1] = 0;

				sprite_read(SP_STONEMAN);
				for (x = 0; x < 210; x = x + 3)
				{
					draw_box(0, 0, 320, 200, 0);
					anim[1]++;
					if (anim[1] == 7)
						anim[1] = 0;

					if (anim[1] < 3)
						anim[0] = SP_RIGHT_FACING(0);
					else
						anim[0] = SP_RIGHT_FACING(1);
					sprite_do(x, 80, SP_PLAYER_W, SP_PLAYER_H, anim[0], 2);
					sprite_do(250, 48, SP_PLAYER_W, SP_PLAYER_H, SP_STONEMAN, 4);

					al_flip_display();
					wait_delay(1);
				}
				clear_screen_for_text();
				sprite_do(x, 80, SP_PLAYER_W, SP_PLAYER_H, anim[0], 2);
				sprite_do(250, 48, SP_PLAYER_W, SP_PLAYER_H, SP_STONEMAN, 4);
				screen_printf("JANE: NOW I HAVE COLLECTED\nALL THE CRYSTALS!\n[ENTER]\n");
				FLIP;
				wait_key_press(ALLEGRO_KEY_ENTER);
				set_sfx(20, 30, 40, 50);
				clear_screen_for_text();
				sprite_do(250, 48, SP_PLAYER_W, SP_PLAYER_H, SP_STONEMAN, 4);
				sprite_do(x, 80, SP_PLAYER_W, SP_PLAYER_H, anim[0], 2);
				screen_printf("STONEMAN: YES. I SEE.\nNOW YOU'LL BE FREE TO GO...\n[ENTER]\n");
				FLIP;
				wait_key_press(ALLEGRO_KEY_ENTER);
				set_sfx(20, 30, 40, 50);
				clear_screen_for_text();
				sprite_do(250, 48, SP_PLAYER_W, SP_PLAYER_H, SP_STONEMAN, 4);
				sprite_do(x, 80, SP_PLAYER_W, SP_PLAYER_H, anim[0], 2);
				score += lives * 1000;
				screen_printf("CONGRATULATIONS!!!\nYOU HAVE COMPLETED CRYSTAL JANE!\n\nLIVES BONUS: %d\n"
							  "TOTAL SCORE: %d\n",
							  lives * 1000, score);

				if (score > hiscore && !GET_ARG('L')) // Hiscore logic disabled if level jump is used
				{
					set_highscore(score);
					screen_printf("NEW HISCORE!\n");
				}
				else
					screen_printf("HISCORE: %d\n", hiscore);

				FLIP;
				wait_key_press(ALLEGRO_KEY_ENTER);
				wait_delay(5);
				goto game_logic_start;
			}

			read_level(level, &world);

			clear_screen_for_text();
			screen_printf("         GET READY FOR ACTION!\n\n\n                 LEVEL:\n");

			float best_time = get_best_time_for_level(level) / 20.0f;

			screen_printf("         %d: %s\n\n         BEST TIME: %.1f SEC\n\n         PRESS ENTER\n",
						  level + 1, world.level_name, best_time >= 0 ? best_time : 999);
			FLIP;
			wait_key_press(ALLEGRO_KEY_ENTER);
			set_sfx(20, 30, 40, 50);

			initial_frame_counter = get_frame_counter();
			time_counter = 0;
			if (!GET_ARG('r'))
				level++;
			jump = -1;
			on_platform = 1;
			x = 15;
			y = 160;
			bats_killed = 0;
			sprint = 0;
			high_jump = 1;

			diamonds = world.diamond_count;
		}
		draw_box_gradient(0, 0, 320, 200, DARK_GRAY, BLACK, 0);

		if (jump > 0)
		{
			y = y - 5;
			on_platform = 0;
			if (get_sfx_pos() == -1)
			{
				set_sfx((20 + jump * 5), 0, 0, 0);
			}
		}
		jump--;
		if (jump < 0)
		{
			on_platform = 0;
			for (count = 0; count < world.platform_count; count++)
			{
				if (world.platform_x[count] + 50 > x && world.platform_x[count] - 14 < x &&
					world.platform_y[count] + 1 < y + 32 && world.platform_y[count] + 4 > y + 32)
				{
					on_platform = 1;
					jump = -1;
					y = world.platform_y[count] - 30;
					break;
				}
			}
			if (!on_platform && y < 159)
			{
				y = y + 3;
				jump = -1;
			}
			else
			{
				on_platform = 1;
				jump = -1;
			}
		}
		for (count = 0; count < world.diamond_count; count++)
		{
			if (world.diamond_anim[count])
				continue;
			if (world.diamond_x[count] + 10 > x && world.diamond_x[count] < x + 14 &&
				world.diamond_y[count] < y + 32 && world.diamond_y[count] + 10 > y) // OTA TIMANTTI
			{
				diamonds--;
				world.diamond_anim[count] = 1;
				set_sfx(30, 42, 37, 0);
			}
		}

		if (keybuffer[ALLEGRO_KEY_P])
		{
			int pause_started = get_frame_counter();
			clrscr();
			screen_printf("\n\n\nGAME PAUSED\nPRESS ENTER TO CONTINUE\n");
			FLIP;
			wait_key_press(ALLEGRO_KEY_ENTER);
			initial_frame_counter += get_frame_counter() - pause_started;
		}

		if (keybuffer[ALLEGRO_KEY_ENTER]) // ALOITA ALUSTA
		{
			x = 15;
			y = 160;
			on_platform = 1;
			jump = -1;
			set_sfx(10, 20, 30, 40);
		}
		int moving = 0;

		if (keybuffer[ALLEGRO_KEY_RIGHT] && x < 305 && sprint >= 0) // LIIKU OIKEAAN
		{
			moving = 1;
			if (enable_sprint && keybuffer[ALLEGRO_KEY_RCTRL] && sprint == 0 && on_platform)
				sprint = 10;
		}
		if (sprint > 0)
		{
			if (x < 305)
			{
				moving = sprint > 5 ? 3 : 2;
				sprint--;
			}
			else
				sprint = 0;
		}
		while (moving-- > 0)
		{
			anim[1]++;

			if (anim[0] == SP_LEFT_FACING(0) || anim[0] == SP_LEFT_FACING(1))
				anim[0] = SP_RIGHT_FACING(0);

			if (anim[1] == 3)
			{
				if (anim[0] == SP_RIGHT_FACING(1))
				{
					anim[0] = SP_RIGHT_FACING(0);
				}
				else
				{
					anim[0] = SP_RIGHT_FACING(1);
				}

				if (get_sfx_pos() == -1)
				{
					set_sfx((anim[0] == SP_RIGHT_FACING(1) ? 1 : 8), 0, 0, 0);
				}

				anim[1] = 0;
			}
			x = x + 3;
			for (count = 0; count < world.wall_count; count++) // OSUUKO SEINääN
			{
				if (world.wall_x[count] > x && world.wall_x[count] - 11 < x &&
					world.wall_y[count] < y + 30 && world.wall_y[count] + 30 > y)
					x = x - 3;
			}
		}
		if (keybuffer[ALLEGRO_KEY_LEFT] && x > 5 && sprint <= 0) // LIIKU VASEMPAAN
		{
			moving = 1;
			if (enable_sprint && keybuffer[ALLEGRO_KEY_RCTRL] && sprint == 0 && on_platform)
				sprint = -10;
		}
		if (sprint < 0)
		{
			if (x > 5)
			{
				moving = sprint < -5 ? 3 : 2;
				sprint++;
			}
			else
				sprint = 0;
		}
		while (moving-- > 0)
		{
			anim[1]++;

			if (anim[0] == SP_RIGHT_FACING(0) || anim[0] == SP_RIGHT_FACING(1))
				anim[0] = SP_LEFT_FACING(0);

			if (anim[1] == 3)
			{
				if (anim[0] == SP_LEFT_FACING(1))
				{
					anim[0] = SP_LEFT_FACING(0);
				}
				else
				{
					anim[0] = SP_LEFT_FACING(1);
				}
				if (get_sfx_pos() == -1)
				{
					set_sfx((anim[0] == SP_LEFT_FACING(0) ? 1 : 8), 0, 0, 0);
				}

				anim[1] = 0;
			}
			x = x - 3;
			for (count = 0; count < world.wall_count; count++) // OSUUKO SEINääN
			{
				if (world.wall_x[count] + 5 > x && world.wall_x[count] < x &&
					world.wall_y[count] < y + 30 && world.wall_y[count] + 30 > y)
					x = x + 3;
			}
		}

		if (keybuffer[ALLEGRO_KEY_UP] && on_platform) // HYPPää!
			jump = jump_height;
		if (enable_high_jump && high_jump && keybuffer[ALLEGRO_KEY_RSHIFT] && on_platform)
		{
			high_jump = 0;
			jump = high_jump_height;
		}

		if (enable_weapon && keybuffer[ALLEGRO_KEY_DOWN] && weapon <= -20)
		{
			weapon = 20;
			set_sfx(20, 19, 18, 0);
		}

		//*LAUTAT + MUUT*//

		draw_box_gradient(0, 190, 320, 200, BROWN, BRIGHT_YELLOW, 0);
		for (count = 0; count < world.platform_count; count++)
		{
			draw_box_gradient(world.platform_x[count] - 1, world.platform_y[count],
							  world.platform_x[count] + 50, world.platform_y[count] + 6, BROWN, BRIGHT_YELLOW, 1);
		}
		for (count = 0; count < world.wall_count; count++)
		{
			draw_box_gradient(world.wall_x[count], world.wall_y[count],
							  world.wall_x[count] + 5, world.wall_y[count] + 30, BROWN, BRIGHT_YELLOW, 1);
		}

		for (count = 0; count < world.diamond_count; count++)
		{
			if (world.diamond_anim[count] < 10)
			{
				sprite_do(world.diamond_x[count], world.diamond_y[count], SP_DIAMOND_W, SP_DIAMOND_H, SP_DIAMOND, 2);
				if (world.diamond_anim[count])
				{
					world.diamond_y[count] -= 3;
					world.diamond_anim[count]++;
				}
			}
		}

		int weap_x = 999, weap_y = 999;
		if (weapon > 0)
		{
			float vx, vy;
			vx = cosf(3.14159f / 10.0f * weapon), vy = sinf(3.14159f / 10.0f * weapon);

			for (count = 0; count < 5; count++)
			{
				weap_x = vx * 10 * count + x + 5;
				weap_y = vy * 10 * count + y + 20;
				if (count < 4)
				{
					draw_box(weap_x - 1, weap_y - 1, weap_x + 1, weap_y + 1, BLACK);
				}
				else
				{
					draw_box(weap_x - 4, weap_y - 4, weap_x + 4, weap_y + 4, BLACK);
				}
			}
		}
		weapon--;

		for (count = 0; count < world.bat_count; count++)
		{
			if (!world.bat_status[count])
				continue;
			if (world.bat_status[count] == 2)
			{
				sprite_do(world.bat_x[count], world.bat_y[count], SP_BAT_W, SP_BAT_H, SP_BAT(1), 2);
				world.bat_y[count] += 5;
				if (world.bat_y[count] > 200)
					world.bat_status[count] = 0;
				continue;
			}
			if (world.bat_x[count] + 27 > weap_x && world.bat_x[count] - 5 < weap_x &&
				world.bat_y[count] - 4 < weap_y && world.bat_y[count] + 13 > weap_y)
			{
				bats_killed++;
				time_counter -= 20;
				world.bat_status[count] = 2;
				draw_box(0, 0, 320, 200, BRIGHT_GREEN);
				set_sfx(60, 60, 20, 20);
				continue;
			}

			if (world.bat_x[count] + 10 > x && world.bat_x[count] < x + 14 &&
				world.bat_y[count] < y + 32 && world.bat_y[count] + 10 > y)
			{
				lives--;
				world.bat_status[count] = 2;
				x = 15;
				y = 160;
				on_platform = 1;
				jump = -1;
				draw_box(0, 0, 320, 200, BRIGHT_RED);
				set_sfx(20, 15, 10, 5);
				continue;
			}
			if (rand() % 2)
			{
				if (world.bat_x[count] < x - 5)
					world.bat_x[count] += 1 + rand() % 2;
				else if (world.bat_x[count] > x + 5)
					world.bat_x[count] -= 1 + rand() % 2;
			}
			if (rand() % 2)
			{
				if (world.bat_y[count] < y - 5)
					world.bat_y[count] += 1 + rand() % 2;
				else if (world.bat_y[count] > y + 5)
					world.bat_y[count] -= 1 + rand() % 2;
			}
			if (rand() % 5 == 0)
			{
				world.bat_x[count] += 1 - 2 * (rand() % 2);
				world.bat_y[count] += 1 - 2 * (rand() % 2);
			}
			sprite_do(world.bat_x[count], world.bat_y[count], SP_BAT_W, SP_BAT_H, SP_BAT(world.bat_x[count] % 2), 2);
		}
		sprite_do(x, y - 2, SP_PLAYER_W, SP_PLAYER_H, anim[0], 2);

		for (count = 0; count < lives; count++) // PIIRRä ELäMäT
			sprite_do(count * 10, 1, SP_DIAMOND_W, SP_DIAMOND_H, SP_DIAMOND, 1);

		draw_box_gradient(125, 1, 125 + (175 - time_counter / 2), 6, RED, BRIGHT_RED, 1); // PIIRRä AIKA

		time_counter++;
		if (time_counter > 350)
		{
			lives--;
			time_counter = 0;
		}
		if (time_counter > 300)
		{
			set_sfx(10 * sound_state, 0, 0, 0);
		}
		al_flip_display();
		wait_delay(1);
	}
	clear_screen_for_text();
	screen_printf("         GAME OVER\nYOU REACHED LEVEL: %d / %d\n\n\n\n\n    PRESS ENTER", level, final_level);
	FLIP;
	wait_key_press(ALLEGRO_KEY_ENTER);
	set_sfx(20, 30, 40, 50);
	goto game_logic_start;
}

#define CALC_GRADIENT(c0, c1, out)                   \
	temp = c0 + (y - y1) * (c1 - c0) / (y2 - y1);    \
	temp = temp > 255 ? 255 : (temp < 0 ? 0 : temp); \
	unsigned char out = temp

void draw_box_gradient(int x1, int y1, int x2, int y2, char box_color_top, char box_color_bottom, int symmetric)
{
	if (symmetric)
	{
		draw_box_gradient(x1, y1, x2, y2 - (y2 - y1) / 2, box_color_top, box_color_bottom, 0);
		draw_box_gradient(x1, y1 + (y2 - y1) / 2, x2, y2, box_color_bottom, box_color_top, 0);
		return;
	}
	x1 *= scaling;
	x2 *= scaling;
	y1 *= scaling;
	y2 *= scaling;
	unsigned char c0r, c0g, c0b;
	unsigned char c1r, c1g, c1b;
	al_unmap_rgb(ega_color(box_color_top), &c0r, &c0g, &c0b);
	al_unmap_rgb(ega_color(box_color_bottom), &c1r, &c1g, &c1b);

	int temp;
	for (int y = y1; y < y2; y++)
	{
		CALC_GRADIENT(c0r, c1r, r);
		CALC_GRADIENT(c0g, c1g, g);
		CALC_GRADIENT(c0b, c1b, b);
		al_draw_filled_rectangle(x1, y, x2, y + 1, al_map_rgb(r, g, b));
	}
}

void draw_box(int x1, int y1, int x2, int y2, char box_color)
{
	x1 *= scaling;
	x2 *= scaling;
	y1 *= scaling;
	y2 *= scaling;
	al_draw_filled_rectangle(x1, y1, x2, y2, ega_color(box_color));
}

char *get_sprite(char sprite)
{
	if (sprite == SP_LEFT_FACING(0) || sprite == SP_STONEMAN)
		return guy_left1;
	if (sprite == SP_LEFT_FACING(1))
		return guy_left2;
	if (sprite == SP_RIGHT_FACING(0))
		return guy_right1;
	if (sprite == SP_RIGHT_FACING(1))
		return guy_right2;
	if (sprite == SP_DIAMOND)
		return dia_spr;
	if (sprite == SP_BAT(0))
		return bat1;
	if (sprite == SP_BAT(1))
		return bat2;
	return NULL;
}

void sprite_do(int _x, int _y, int area_x, int area_y, char sprite, char zoom_mult)
{
	char _123x, _123y = 0;
	int counter = 0;
	char *sprite_buf = get_sprite(sprite);
	if (!sprite_buf)
		return;

	while (area_y * zoom_mult > _123y)
	{
		for (_123x = 0; _123x < area_x * zoom_mult; _123x = _123x + zoom_mult)
		{
			if (sprite_buf[counter] != TRANSPARENT)
				draw_box(_x + _123x, _y + _123y, _x + _123x + zoom_mult, _y + _123y + zoom_mult, sprite_buf[counter]);
			counter++;
		}
		_123y = _123y + zoom_mult;
	}
}
void sprite_read(char sprite)
{
	char *sprite_buf = get_sprite(sprite);
	if (!sprite_buf)
		return;
	game_data_read_sprite(sprite, sprite_buf);
}
