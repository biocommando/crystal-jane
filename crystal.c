#include "allegro_compat.h"
#include "game_data.h"
#include "synth.h"
#include "common.h"
#include "graphics.h"

#include <stdio.h>
#include <math.h>

#include "build_info.h"

EXTERN_GLOBALS;

void read_sprites()
{
	static char guy_left1[SP_NUM_PX(PLAYER)],
		guy_left2[SP_NUM_PX(PLAYER)],
		guy_right1[SP_NUM_PX(PLAYER)],
		guy_right2[SP_NUM_PX(PLAYER)],
		dia_spr[SP_NUM_PX(DIAMOND)],
		bat1[SP_NUM_PX(BAT)],
		bat2[SP_NUM_PX(BAT)],
		stoneman[SP_NUM_PX(PLAYER)];
		
	sprite_read(SP_LEFT_FACING(0), guy_left1, sizeof(guy_left1));
	sprite_read(SP_LEFT_FACING(1), guy_left2, sizeof(guy_left2));
	sprite_read(SP_RIGHT_FACING(0), guy_right1, sizeof(guy_right1));
	sprite_read(SP_RIGHT_FACING(1), guy_right2, sizeof(guy_right2));
	sprite_read(SP_DIAMOND, dia_spr, sizeof(dia_spr));
	sprite_read(SP_BAT(0), bat1, sizeof(bat1));
	sprite_read(SP_BAT(1), bat2, sizeof(bat2));
	sprite_read(SP_STONEMAN, stoneman, sizeof(stoneman));
}

int game_logic(struct game_settings gmsettings)
{
	char *result, anim[2], on_platform, jump, sound_state = 1;

	char *buf, file_read_buf[20];

	int y, x, lives, diamonds, time_counter, level, count, hiscore = 0;
	int score = 0;
	screen_printf("LOADING SPRITES...\n");

	hiscore = get_highscore();


	lives = gmsettings.max_lives;
	level = 0;

	if (gmsettings.start_level)
	{
		level = gmsettings.start_level;
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
	if (gmsettings.is_mod)
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
			if (!gmsettings.skip_splash_screen)
				wait_delay(60);
			return -1;
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
			if (level == gmsettings.final_level)
			{
				set_music(3);
				clear_screen_for_text();
				anim[1] = 0;
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

				if (score > hiscore && !gmsettings.start_level) // Hiscore logic disabled if level jump is used
				{
					set_highscore(score);
					screen_printf("NEW HISCORE!\n");
				}
				else
					screen_printf("HISCORE: %d\n", hiscore);

				FLIP;
				wait_key_press(ALLEGRO_KEY_ENTER);
				wait_delay(5);
				return 0;
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
			if (!gmsettings.repeat_level)
				level++;
			jump = -1;
			on_platform = 1;
			x = world.init_x;
			y = world.init_y;
			bats_killed = 0;
			sprint = 0;
			high_jump = 1;

			diamonds = world.diamond_count;
			set_music(1);
		}

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
			x = world.init_x;
			y = world.init_y;
			on_platform = 1;
			jump = -1;
			set_sfx(10, 20, 30, 40);
		}
		int moving = 0;

		if (keybuffer[ALLEGRO_KEY_RIGHT] && x < 305 && sprint >= 0) // LIIKU OIKEAAN
		{
			moving = 1;
			if (gmsettings.enable_sprint && keybuffer[ALLEGRO_KEY_RCTRL] && sprint == 0 && on_platform)
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
			if (gmsettings.enable_sprint && keybuffer[ALLEGRO_KEY_RCTRL] && sprint == 0 && on_platform)
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
			jump = gmsettings.jump_height;
		if (gmsettings.enable_high_jump && high_jump && keybuffer[ALLEGRO_KEY_RSHIFT] && on_platform)
		{
			high_jump = 0;
			jump = gmsettings.high_jump_height;
		}

		if (gmsettings.enable_weapon && keybuffer[ALLEGRO_KEY_DOWN] && weapon <= -20)
		{
			weapon = 20;
			set_sfx(20, 19, 18, 0);
		}

		draw_world(&world);

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
				x = world.init_x;
				y = world.init_y;
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
		}
		sprite_do(x, y - 2, SP_PLAYER_W, SP_PLAYER_H, anim[0], 2);

		for (count = 0; count < lives; count++) // PIIRRä ELäMäT
			sprite_do(count * 10, 1, SP_DIAMOND_W, SP_DIAMOND_H, SP_DIAMOND, 1);

		draw_box_gradient(125, 1, 125 + (175 - time_counter / 2), 6, RED, BRIGHT_RED, 1); // PIIRRä AIKA

		time_counter++;
		if (time_counter > gmsettings.time_counter_limit)
		{
			lives--;
			time_counter = 0;
		}
		if (time_counter > gmsettings.time_counter_limit - 50)
		{
			set_sfx(10 * sound_state, 0, 0, 0);
		}
		al_flip_display();
		wait_delay(1);
	}
	clear_screen_for_text();
	screen_printf("         GAME OVER\nYOU REACHED LEVEL: %d / %d\n\n\n\n\n    PRESS ENTER", level, gmsettings.final_level);
	FLIP;
	wait_key_press(ALLEGRO_KEY_ENTER);
	set_sfx(20, 30, 40, 50);
	return 0;
}
