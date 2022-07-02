#include "allegro_compat.h"
#include "synth.h"

#include <stdio.h>
#include <math.h>

#define GAME_DATA_FILE "gdat.dat"

void read_game_data_file_until(FILE *f, const char *title, char id)
{
	printf("finding %s\n", title);
	fseek(f, 0, SEEK_SET);
	while(!feof(f))
	{
		char b[100], rtitle[100] = "", rid = 0;
		fgets(b, 100, f);
		sscanf(b, "@%s %c", rtitle, &rid);
		if (!strcmp(title, rtitle) && (id == 0 || rid == id))
		{
			printf("found %s %d %c\n", title, id, id);
			return;
		}
	}
}

void draw_box(int x1, int y1, int x2, int y2, char box_color);

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

#define SP_NUM_PX(sp) (SP_##sp ## _W * SP_##sp ## _H)

char guy_left1[SP_NUM_PX(PLAYER)], guy_left2[SP_NUM_PX(PLAYER)], guy_right1[SP_NUM_PX(PLAYER)], guy_right2[SP_NUM_PX(PLAYER)], dia_spr[SP_NUM_PX(DIAMOND)], *buf, file_read_buf[20];
char bat1[SP_NUM_PX(BAT)], bat2[SP_NUM_PX(BAT)];
// 7*16 = 112   ;  5*6 = 30

int y, x, lives, diamonds, time_counter, level, count, platform_x[50], platform_y[50], wall_x[50], wall_y[50], diamond_x[50], diamond_y[50], hiscore;
int score = 0;
int bat_x[50], bat_y[50];
int platform_count, wall_count, diamond_count, bat_count;

/*************************************************************'

funktiot:

piirra_boxi = piirtää bufferiin yksivärisen laatikon
parametrit:
x1,y1 = vasemman yläkulman koordinaatit
x2,y2 = oikean alakulman koordinaatit
boxvari = laatikon väri
**
grtila = vaihtaa grafiikka/teksti tilaan/varaa muistia & alustaa bufferin
		 /flippaa puskurin näytälle
tila = mitä yllä olevista vaihtoehdoista tekee
**
sprite_do = piirtää yhden spriten
_x,_y = vasemman yläkulman koordinaatit
aluex = kuvan leveys
aluey = kuvan korkeus
kumpi = mikä sprite (alunperin altogrimissa 2 spriteä...)
lahella = skaalaus (yli 1, kokonaislukuja)
**
sprite_read = lukee spriten tiedostosta
kumpi = mikä sprite
***********
muuttujat:

_tied = yleishyädyllinen tiedosto (kaikki data on samassa tiedostossa,
		paitsi hiscore)
**
tulos = tiedoston osoittaja...
anim = ääh.. [0] mitä animaatiota jane käyttää [1] milloin tulee
	   seuraava animaatio.
lautalla = onko jane lautan päällä (ei tipu lautan läpi).
hyppy = kuinka paljon hyppyvoimaa jäljellä (kun on painettu hyppy näppiä)
aani = onks äänet päällä??? KUULUUKO???!
aijavas1+2,aijaoik1+2,dia_spr = spritejä nuo kaikki (yksi numero indexissä
								edustaa yhtä väriä (/väripistettä))
*buf = näyttäbufferi      HUOM!
tiedluku = lukee levelit ja niitten nimet!
turbo_pc = 1= windows3.1 tai win9X,2X me mutta 2= WIN (badbadbad) XP!
**
y,x = janen sijainti
lives = elämiä
diamonds = monta timanttia keräämättä
nopeus = pc:n nopeus
aika = aikaa jälellä ennen kun menee enkuu!
level = mikä leveli
lasku = yleishyädyllinen (h)intti
lauttax/y = lauttojen koordinaatit (20 lauttaa per level)
diamondx/y = timanttejen/kristallejen koordinaatit (10 timanttii per level)
enkka = ennätys
  -- EIPä MITTääN YLLäTTäVVää.. NYT VAAN SPACEJABIN KIMPUUN
	--- ( : - O ) NEIN!!!!!!!!!!!!!!
'**************************************************************/

extern char keybuffer[ALLEGRO_KEY_MAX];

char *arg = NULL;

const char *get_arg(int argc, char **argv, char flag)
{
	for (int i = 0; i < argc; i++)
	{
		if (argv[i][0] == flag)
			return arg = argv[i] + 1;
	}
	return arg = NULL;
}

#define GET_ARG(flag) get_arg(argc, argv, flag)

int main(int argc, char **argv)
{
	
	if (init_allegro())
	{
		printf("Init allegro failed");
		return 0;
	}

	while (!keybuffer[ALLEGRO_KEY_SPACE] && GET_ARG('w'))
	{
		wait_event();
	}
	synth_init(SYNTH_SETTINGS);
	extern const char my_synth_sequence[2838];
	set_sequence(my_synth_sequence, 2838);
	
	clrscr();
	screen_printf("HI!\nWelcome to Crystal Jane\n\nStory:\nJane has been ten years in a prison\n");
	screen_printf("of a cruel stoneman.\nOne day stoneman says to free Jane\nif she collects all");
	screen_printf(" the lifecrystals\non that planet...\n\nPRESS ENTER\n");
	FLIP;
	while (!keybuffer[ALLEGRO_KEY_ENTER])
	{
		wait_event();
	}
	set_sfx(20, 30, 40, 50);
	wait_delay(5);
	clrscr();
	draw_box(0, 0, 320, 200, 0);
	screen_printf("You'll be able to control Jane with\nLEFT and RIGHT arrows\n");
	screen_printf("and Jane jumps if you press UP.\nJane can also go back to the beginning\nof a level by pressing ENTER.\n"
		"Beware the bats! You can use your whip with\n"
		"DOWN arrow to knock them out.\n"
		"\nPRESS P ANYTIME TO PAUSE GAME.\nYour mission is to get");
	screen_printf(" the lifecrystals\non 15 levels.\n\nPRESS ENTER\n");
	FLIP;
	while (!keybuffer[ALLEGRO_KEY_ENTER])
	{
		wait_event();
	}
	set_sfx(20, 30, 40, 50);
	wait_delay(5);
	clrscr();
	draw_box(0, 0, 320, 200, 0);
	screen_printf("You can see your lives in the\nupper left corner\nand time running next to it...\nBest of luck for you!\n\nPRESS ENTER\n");
	FLIP;
	while (!keybuffer[ALLEGRO_KEY_ENTER])
	{
		wait_event();
	}
	wait_delay(5);

alku:
	screen_printf("LOADING SPRITES...\n");

	{
		FILE *fhscore = fopen("hiscore.jan", "r");
		fscanf(fhscore, "%d", &hiscore);
		fclose(fhscore);
	}

	sprite_read(SP_LEFT_FACING(0));
	sprite_read(SP_LEFT_FACING(1));
	sprite_read(SP_RIGHT_FACING(0));
	sprite_read(SP_RIGHT_FACING(1));
	sprite_read(SP_DIAMOND);
	sprite_read(SP_BAT(0));
	sprite_read(SP_BAT(1));

	lives = 8;
	level = 0;
	
	if (GET_ARG('L') && arg[0])
	{
		sscanf(arg, "%d", &level);
		level -= 1;
		if (level >= 15 || level < 0)
			level = 0;
		printf("Start level changed to %d\n", level + 1);
	}

	anim[0] = SP_RIGHT_FACING(0);
	anim[1] = 0;
	diamonds = 0;
	score = 0;

	draw_box(0, 0, 320, 200, 0);
	sprite_do(100, 80, SP_PLAYER_W, SP_PLAYER_H, SP_RIGHT_FACING(0), 2);
	sprite_do(140, 94, SP_DIAMOND_W, SP_DIAMOND_H, SP_DIAMOND, 2);
	sprite_do(180, 75, SP_BAT_W, SP_BAT_H, SP_BAT(0), 2);
	clrscr();
	screen_printf("Crystal Jane (c) Upr00ted tree software\n        MENU\n       UP: PLAY\n     DOWN: QUIT\n\n\n\n\n\n\n\n        HISCORE: %d\n", hiscore);
	FLIP;
	set_sfx(20, 30, 40, 50);
	while (1)
	{
		wait_event();
		if (keybuffer[ALLEGRO_KEY_DOWN])
		{
			synth_init(SYNTH_SETTINGS);
			clrscr();
			draw_box(0, 0, 320, 200, 0);

			screen_printf("+********************+\n* THANKS FOR PLAYING *\n*    CRYSTAL JANE    *\n+********************+\n");
			screen_printf("Crystal Jane (c) Upr00ted tree software\nAll comments to: joonas1000@gmail.com\n");
			FLIP;
			wait_delay(60);
			exit(1);
		}

		if (keybuffer[ALLEGRO_KEY_UP])
			break;
	}
	set_sfx(70, 10, 50, 30);

	int weapon = 0;
	while (lives > 0 && !keybuffer[ALLEGRO_KEY_ESCAPE])
	{
		if (diamonds == 0)
		{
			srand(1234);
			weapon = 0;
			draw_box(0, 0, 320, 200, 0);
			clrscr();
			if (level == 15)
			{
				anim[1] = 0;

				sprite_read(SP_STONEMAN);
				for (x = 0; x < 200; x = x + 3)
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
				clrscr();
				draw_box(0, 0, 320, 200, 0);
				sprite_do(x, 80, SP_PLAYER_W, SP_PLAYER_H, anim[0], 2);
				sprite_do(250, 48, SP_PLAYER_W, SP_PLAYER_H, SP_STONEMAN, 4);
				screen_printf("JANE: NOW I HAVE COLLECTED\nALL THE CRYSTALS!\n[ENTER]\n");
				FLIP;
				while (!keybuffer[ALLEGRO_KEY_ENTER])
				{
					wait_event();
				}
				set_sfx(20, 30, 40, 50);
				wait_delay(5);
				clrscr();
				draw_box(0, 0, 320, 200, 0);
				sprite_do(250, 48, SP_PLAYER_W, SP_PLAYER_H, SP_STONEMAN, 4);
				sprite_do(x, 80, SP_PLAYER_W, SP_PLAYER_H, anim[0], 2);
				screen_printf("STONEMAN: YES. I SEE.\nNOW YOU'LL BE FREE TO GO...\n[ENTER]\n");
				FLIP;
				while (!keybuffer[ALLEGRO_KEY_ENTER])
				{
					wait_event();
				}
				set_sfx(20, 30, 40, 50);
				wait_delay(5);
				clrscr();
				draw_box(0, 0, 320, 200, 0);
				sprite_do(250, 48, SP_PLAYER_W, SP_PLAYER_H, SP_STONEMAN, 4);
				sprite_do(x, 80, SP_PLAYER_W, SP_PLAYER_H, anim[0], 2);
				score += lives * 1000;
				screen_printf("CONGRATULATIONS!!!\nYOU HAVE COMPLETED CRYSTAL JANE!\n\nLIVES BONUS: %d\n"
					"TOTAL SCORE: %d\n", lives * 1000, score);

				if (score > hiscore)
				{
					FILE *fhscore = fopen("hiscore.jan", "w");
					fprintf(fhscore, "%d", score);
					fclose(fhscore);
					screen_printf("NEW HISCORE!\n");
				}
				else
					screen_printf("HISCORE: %d\n", hiscore);

				FLIP;
				while (!keybuffer[ALLEGRO_KEY_ENTER])
				{
					wait_event();
				}
				set_sfx(20, 30, 40, 50);
				wait_delay(5);
				goto alku;
			}

			FILE *game_data = fopen("gdat.dat", "r");
			read_game_data_file_until(game_data, "level", level < 9 ? '1' + level : 'A' + level - 9);

			if (level != 15)
			{
				if (level != 0)
				{
					int bonus = 350 - time_counter;
					score += bonus * 2;
				}
				screen_printf("\n\n\n\n         GET READY FOR ACTION!\n\n\n                 LEVEL:\n");

				char level_name[32];
				fgets(level_name, 32, game_data);

				screen_printf("         %d: %s\n\n\n\nPress enter...\n", level + 1, level_name);
				FLIP;
				while (!keybuffer[ALLEGRO_KEY_ENTER])
				{
					wait_event();
				}
				set_sfx(20, 30, 40, 50);
			}

			time_counter = 0;
			level++;
			jump = -1;
			on_platform = 1;
			x = 15;
			y = 160;

			platform_count = wall_count = diamond_count = bat_count = 0;
			while (1)
			{
				fgets(file_read_buf, 20, game_data);
				if (feof(game_data) || file_read_buf[0] == '@')
					break;
				int rx, ry, type = 0;
				sscanf(file_read_buf, "%d %d %c", &rx, &ry, &type);
				if (type == 'H')
				{
					platform_x[platform_count] = rx;
					platform_y[platform_count] = ry;
					platform_count++;
				}
				else if (type == 'V')
				{
					wall_x[wall_count] = rx;
					wall_y[wall_count] = ry;
					wall_count++;
				}
				else if (type == 'D')
				{
					diamond_x[diamond_count] = rx;
					diamond_y[diamond_count] = ry;
					diamond_count++;
				}
				else if (type == 'B')
				{
					bat_x[bat_count] = rx;
					bat_y[bat_count] = ry;
					bat_count++;
				}
			}
			diamonds = diamond_count;
			fclose(game_data);
		}
		draw_box(0, 0, 320, 200, DARK_GRAY);

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
			for (count = 0; count < platform_count; count++)
			{
				if (platform_x[count] + 50 > x && platform_x[count] - 14 < x && platform_y[count] + 1 < y + 32 && platform_y[count] + 4 > y + 32)
				{
					on_platform = 1;
					jump = -1;
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
		for (count = 0; count < diamond_count; count++)
		{
			if (diamond_x[count] + 10 > x && diamond_x[count] < x + 14 && diamond_y[count] < y + 32 && diamond_y[count] + 10 > y) // OTA TIMANTTI
			{
				diamonds--;
				diamond_x[count] = 500;
				diamond_y[count] = 500;
				set_sfx(30, 42, 37, 0);
			}
		}

		if (keybuffer[ALLEGRO_KEY_P])
		{
			clrscr();
			screen_printf("\n\n\nGAME PAUSED\nPRESS SPACE TO CONTINUE\n");
			while (!keybuffer[ALLEGRO_KEY_SPACE])
			{
				wait_event();
			}
		}

		if (keybuffer[ALLEGRO_KEY_ENTER]) // ALOITA ALUSTA
		{
			x = 15;
			y = 160;
			on_platform = 1;
			jump = -1;
		}

		if (keybuffer[ALLEGRO_KEY_RIGHT] && x < 305) // LIIKU OIKEAAN
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
			for (count = 0; count < wall_count; count++) // OSUUKO SEINääN
			{
				if (wall_x[count] > x && wall_x[count] - 11 < x && wall_y[count] < y + 30 && wall_y[count] + 30 > y)
					x = x - 3;
			}
		}

		if (keybuffer[ALLEGRO_KEY_LEFT] && x > 5) // LIIKU VASEMPAAN
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
			for (count = 0; count < 10; count++) // OSUUKO SEINääN
			{
				if (wall_x[count] + 5 > x && wall_x[count] < x && wall_y[count] < y + 30 && wall_y[count] + 30 > y)
					x = x + 3;
			}
		}

		if (keybuffer[ALLEGRO_KEY_UP] && on_platform) // HYPPää!
			jump = 6;
		
		if (keybuffer[ALLEGRO_KEY_DOWN] && weapon <= -20)
		{
			weapon = 20;
			set_sfx(20, 19, 18, 0);
		}

		// TODO
		/*if (keybuffer[SxF5] && keybuffer[SxJ] && lautalla == 'K') // KOODIT
			hyppy = 12;
		if (keybuffer[SxF5] && keybuffer[SxD] && y < 160) // KOODIT
			lautalla = 'E';
		if (keybuffer[SxF5] && keybuffer[SxF] && hyppy < 1) // KOODIT
			hyppy = 6;*/

		//*LAUTAT + MUUT*//

		draw_box(0, 190, 320, 200, YELLOW);
		for (count = 0; count < 20; count++)
		{
			draw_box(platform_x[count] - 1, platform_y[count], platform_x[count] + 50, platform_y[count] + 5, YELLOW);
			if (count < 10)
				draw_box(wall_x[count], wall_y[count], wall_x[count] + 5, wall_y[count] + 30, YELLOW);
		}

		for (count = 0; count < 10; count++)
		{
			if (diamond_x[count] < 500)
				sprite_do(diamond_x[count], diamond_y[count], SP_DIAMOND_W, SP_DIAMOND_H, SP_DIAMOND, 2);
		}

		int ase_x = 999, ase_y = 999;
		if (weapon > 0)
		{
			float vx = cosf(3.14159f / 10.0f * weapon), vy = sinf(3.14159f / 10.0f * weapon);

			for (count = 0; count < 5; count++)
			{
				ase_x = vx * 10 * count + x + 5;
				ase_y = vy * 10 * count + y + 20;
				if (count < 4)
					draw_box(ase_x - 1, ase_y - 1, ase_x + 1, ase_y + 1, 0);
				else
					draw_box(ase_x - 4, ase_y - 4, ase_x + 4, ase_y + 4, 0);
			}
		}
		weapon--;

		for (count = 0; count < bat_count; count++)
		{
			if (bat_x[count] >= 500)
				continue;
			if (bat_x[count] + 27 > ase_x && bat_x[count] - 5 < ase_x && bat_y[count] - 4 < ase_y && bat_y[count] + 13 > ase_y)
			{
				time_counter -= 20;
				bat_x[count] = 500;
				draw_box(0, 0, 320, 200, BRIGHT_GREEN);
				set_sfx(60, 60, 20, 20);
				continue;
			}

			if (bat_x[count] + 10 > x && bat_x[count] < x + 14 && bat_y[count] < y + 32 && bat_y[count] + 10 > y)
			{
				lives--;
				bat_x[count] = 500;
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
				if (bat_x[count] < x - 5)
					bat_x[count] += 1 + rand() % 2;
				else if (bat_x[count] > x + 5)
					bat_x[count] -= 1 + rand() % 2;
			}
			if (rand() % 2)
			{
				if (bat_y[count] < y - 5)
					bat_y[count] += 1 + rand() % 2;
				else if (bat_y[count] > y + 5)
					bat_y[count] -= 1 + rand() % 2;
			}
			if (rand() % 5 == 0)
			{
				bat_x[count] += 1 - 2 * (rand() % 2);
				bat_y[count] += 1 - 2 * (rand() % 2);
			}
			sprite_do(bat_x[count], bat_y[count], SP_BAT_W, SP_BAT_H, SP_BAT(bat_x[count] % 2), 2);
		}
		sprite_do(x, y - 2, SP_PLAYER_W, SP_PLAYER_H, anim[0], 2);

		for (count = 0; count < lives; count++) // PIIRRä ELäMäT
			sprite_do(count * 10, 1, SP_DIAMOND_W, SP_DIAMOND_H, SP_DIAMOND, 1);

		draw_box(125, 1, 125 + (175 - time_counter / 2), 6, BRIGHT_RED); // PIIRRä AIKA

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
	draw_box(0, 0, 320, 200, 0);
	clrscr();
	screen_printf("\n\n         GAME OVER\nYOU REACHED LEVEL: %d / 15\n\n\n\n\n    PRESS SPACE", level);
	FLIP;
	while (!keybuffer[ALLEGRO_KEY_SPACE])
	{
		wait_event();
	}
	set_sfx(20, 30, 40, 50);
	goto alku;
}

void draw_box(int x1, int y1, int x2, int y2, char box_color)
{
	x1 *= 3;
	x2 *= 3;
	y1 *= 3;
	y2 *= 3;
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
			if (sprite_buf[counter] != 'n')
				draw_box(_x + _123x, _y + _123y, _x + _123x + zoom_mult, _y + _123y + zoom_mult, sprite_buf[counter]);
			counter++;
		}
		_123y = _123y + zoom_mult;
	}
}
void sprite_read(char sprite)
{
	char read_result = 10;
	int counter = 0;
	char *sprite_buf = get_sprite(sprite);
	if (!sprite_buf)
		return;

	FILE *f = fopen(GAME_DATA_FILE, "r");
	read_game_data_file_until(f, "sprite", sprite);
	
	while (read_result != 'E')
	{
		if (read_result != 10)
		{
			if (read_result == ' ')
				read_result = 'n' + 47;
			// Skin color was done using some weird hack, let's do less weird hack
			if (read_result == '*')
				read_result = SKIN + 47;
			sprite_buf[counter] = read_result - 47;

			counter++;
		}
		read_result = fgetc(f);
	}
	while (read_result != 'E');
}

