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

void piirra_boxi(int x1, int y1, int x2, int y2, char boxvari);
void grtila(int tila);

#define FLIP grtila('F')

void sprite_do(int _x, int _y, int aluex, int aluey, char kumpi, char lahella);
void sprite_read(char kumpi);

char *tulos, anim[2], lautalla, hyppy, aani = 1;

#define SP_BAT(x) ((x) == 0 ? 'B' : 'b')
#define SP_LEFT_FACING(x) ((x) == 0 ? '<' : '{')
#define SP_RIGHT_FACING(x) ((x) == 0 ? '>' : '}')
#define SP_DIAMOND 'D'

#define SP_PLAYER_W 7
#define SP_PLAYER_H 16

#define SP_DIAMOND_W 5
#define SP_DIAMOND_H 6

#define SP_BAT_W 11
#define SP_BAT_H 5

#define SP_NUM_PX(sp) (SP_##sp ## _W * SP_##sp ## _H)

char aijavas1[SP_NUM_PX(PLAYER)], aijavas2[SP_NUM_PX(PLAYER)], aijaoik1[SP_NUM_PX(PLAYER)], aijaoik2[SP_NUM_PX(PLAYER)], dia_spr[SP_NUM_PX(DIAMOND)], *buf, tiedluku[20];
char bat1[SP_NUM_PX(BAT)], bat2[SP_NUM_PX(BAT)];
// 7*16 = 112   ;  5*6 = 30

int y, x, lives, diamonds, aika, level, lasku, lauttax[50], lauttay[50], seinax[50], seinay[50], diamondx[50], diamondy[50], enkka;
int score = 0;
int bat_x[50], bat_y[50];
int lautta_maara, seina_maara, diamond_maara, bat_maara;

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

	while (!keybuffer[SxSPACE] && GET_ARG('w'))
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
	while (!keybuffer[SxENTER])
	{
		wait_event();
	}
	set_sfx(20, 30, 40, 50);
	viive(5);
	clrscr();
	piirra_boxi(0, 0, 320, 200, 0);
	screen_printf("You'll be able to control Jane with\n<- and -> arrows\n");
	screen_printf("and Jane jumps if you press R-CONTROL.\nJane can also go back to the beginning\nof a level by pressing ENTER.\n"
		"Beware the bats! You can use your whip with\n"
		"R-SHIFT to knock them out.\n"
		"\nPRESS P ANYTIME TO PAUSE GAME.\nYour mission is to get");
	screen_printf(" the lifecrystals\non 15 levels.\n\nPRESS ENTER\n");
	FLIP;
	while (!keybuffer[SxENTER])
	{
		wait_event();
	}
	set_sfx(20, 30, 40, 50);
	viive(5);
	clrscr();
	piirra_boxi(0, 0, 320, 200, 0);
	screen_printf("You can see your lives in the\nupper left corner\nand time running next to it...\nBest of luck for you!\n\nPRESS ENTER\n");
	FLIP;
	while (!keybuffer[SxENTER])
	{
		wait_event();
	}
	viive(5);

alku:
	screen_printf("LOADING SPRITES...\n");

	{
		FILE *fenkka = fopen("hiscore.jan", "r");
		fscanf(fenkka, "%d", &enkka);
		fclose(fenkka);
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

	piirra_boxi(0, 0, 320, 200, 0);
	sprite_do(100, 80, SP_PLAYER_W, SP_PLAYER_H, SP_RIGHT_FACING(0), 2);
	sprite_do(140, 94, SP_DIAMOND_W, SP_DIAMOND_H, SP_DIAMOND, 2);
	sprite_do(180, 75, SP_BAT_W, SP_BAT_H, SP_BAT(0), 2);
	clrscr();
	screen_printf("Crystal Jane (c) Upr00ted tree software\n        MENU\n        1: PLAY\n        2: QUIT\n\n\n\n\n\n\n\n        HISCORE: %d\n", enkka);
	FLIP;
	set_sfx(20, 30, 40, 50);
	while (1)
	{
		wait_event();
		if (keybuffer[Sx2])
		{
			synth_init(SYNTH_SETTINGS);
			clrscr();
			piirra_boxi(0, 0, 320, 200, 0);

			screen_printf("+********************+\n* THANKS FOR PLAYING *\n*    CRYSTAL JANE    *\n+********************+\n");
			screen_printf("Crystal Jane (c) Upr00ted tree software\nAll comments to: joonas1000@gmail.com\n");
			FLIP;
			viive(60);
			exit(1);
		}

		if (keybuffer[Sx1])
			break;
	}
	set_sfx(70, 10, 50, 30);

	int ase = 0;
	while (lives > 0 && !keybuffer[SxESC])
	{
		if (diamonds == 0)
		{
			srand(1234);
			ase = 0;
			piirra_boxi(0, 0, 320, 200, 0);
			clrscr();
			if (level == 15)
			{
				anim[1] = 0;

				sprite_read('S');
				for (x = 0; x < 200; x = x + 3)
				{
					piirra_boxi(0, 0, 320, 200, 0);
					anim[1]++;
					if (anim[1] == 7)
						anim[1] = 0;

					if (anim[1] < 3)
						anim[0] = SP_RIGHT_FACING(0);
					else
						anim[0] = SP_RIGHT_FACING(1);
					sprite_do(x, 80, SP_PLAYER_W, SP_PLAYER_H, anim[0], 2);
					sprite_do(250, 48, SP_PLAYER_W, SP_PLAYER_H, 'S', 4);

					grtila('F');
					viive(1);
				}
				clrscr();
				piirra_boxi(0, 0, 320, 200, 0);
				sprite_do(x, 80, SP_PLAYER_W, SP_PLAYER_H, anim[0], 2);
				sprite_do(250, 48, SP_PLAYER_W, SP_PLAYER_H, 'S', 4);
				screen_printf("JANE: NOW I HAVE COLLECTED\nALL THE CRYSTALS!\n[ENTER]\n");
				FLIP;
				while (!keybuffer[SxENTER])
				{
					wait_event();
				}
				set_sfx(20, 30, 40, 50);
				viive(5);
				clrscr();
				piirra_boxi(0, 0, 320, 200, 0);
				sprite_do(250, 48, SP_PLAYER_W, SP_PLAYER_H, 'S', 4);
				sprite_do(x, 80, SP_PLAYER_W, SP_PLAYER_H, anim[0], 2);
				screen_printf("STONEMAN: YES. I SEE.\nNOW YOU'LL BE FREE TO GO...\n[ENTER]\n");
				FLIP;
				while (!keybuffer[SxENTER])
				{
					wait_event();
				}
				set_sfx(20, 30, 40, 50);
				viive(5);
				clrscr();
				piirra_boxi(0, 0, 320, 200, 0);
				sprite_do(250, 48, SP_PLAYER_W, SP_PLAYER_H, 'S', 4);
				sprite_do(x, 80, SP_PLAYER_W, SP_PLAYER_H, anim[0], 2);
				score += lives * 1000;
				screen_printf("CONGRATULATIONS!!!\nYOU HAVE COMPLETED CRYSTAL JANE!\n\nLIVES BONUS: %d\n"
					"TOTAL SCORE: %d\n", lives * 1000, score);

				if (score > enkka)
				{
					FILE *fenkka = fopen("hiscore.jan", "w");
					fprintf(fenkka, "%d", score);
					fclose(fenkka);
					screen_printf("NEW HISCORE!\n");
				}
				else
					screen_printf("HISCORE: %d\n", enkka);

				FLIP;
				while (!keybuffer[SxENTER])
				{
					wait_event();
				}
				set_sfx(20, 30, 40, 50);
				viive(5);
				goto alku;
			}

			FILE *game_data = fopen("gdat.dat", "r");
			read_game_data_file_until(game_data, "level", level < 9 ? '1' + level : 'A' + level - 9);

			if (level != 15)
			{
				if (level != 0)
				{
					int bonus = 350 - aika;
					score += bonus * 2;
				}
				screen_printf("\n\n\n\n         GET READY FOR ACTION!\n\n\n                 LEVEL:\n");

				char level_name[32];
				fgets(level_name, 32, game_data);

				screen_printf("         %d: %s\n\n\n\nPress enter...\n", level + 1, level_name);
				FLIP;
				while (!keybuffer[SxENTER])
				{
					wait_event();
				}
				set_sfx(20, 30, 40, 50);
			}

			aika = 0;
			level++;
			hyppy = -1;
			lautalla = 'K';
			x = 15;
			y = 160;

			lautta_maara = seina_maara = diamond_maara = bat_maara = 0;
			while (1)
			{
				fgets(tiedluku, 20, game_data);
				if (feof(game_data) || tiedluku[0] == '@')
					break;
				int rx, ry, type = 0;
				sscanf(tiedluku, "%d %d %c", &rx, &ry, &type);
				if (type == 'H')
				{
					lauttax[lautta_maara] = rx;
					lauttay[lautta_maara] = ry;
					lautta_maara++;
				}
				else if (type == 'V')
				{
					seinax[seina_maara] = rx;
					seinay[seina_maara] = ry;
					seina_maara++;
				}
				else if (type == 'D')
				{
					diamondx[diamond_maara] = rx;
					diamondy[diamond_maara] = ry;
					diamond_maara++;
				}
				else if (type == 'B')
				{
					bat_x[bat_maara] = rx;
					bat_y[bat_maara] = ry;
					bat_maara++;
				}
			}
			diamonds = diamond_maara;
			fclose(game_data);
		}
		piirra_boxi(0, 0, 320, 200, DARKGRAY);

		if (hyppy > 0)
		{
			y = y - 5;
			lautalla = 'E';
			if (get_sfx_pos() == -1)
			{
				set_sfx((20 + hyppy * 5), 0, 0, 0);
			}
		}
		hyppy--;

		if (hyppy < 0)
		{
			lautalla = 'E';
			for (lasku = 0; lasku < lautta_maara; lasku++)
			{
				if (lauttax[lasku] + 50 > x && lauttax[lasku] - 14 < x && lauttay[lasku] + 1 < y + 32 && lauttay[lasku] + 4 > y + 32)
				{
					lautalla = 'K';
					hyppy = -1;
				}
			}
			if (lautalla == 'E' && y < 159)
			{
				y = y + 3;
				hyppy = -1;
			}
			else
			{
				lautalla = 'K';
				hyppy = -1;
			}
		}
		for (lasku = 0; lasku < diamond_maara; lasku++)
		{
			if (diamondx[lasku] + 10 > x && diamondx[lasku] < x + 14 && diamondy[lasku] < y + 32 && diamondy[lasku] + 10 > y) // OTA TIMANTTI
			{
				diamonds--;
				diamondx[lasku] = 500;
				diamondy[lasku] = 500;
				set_sfx(30, 42, 37, 0);
			}
		}

		if (keybuffer[SxP])
		{
			clrscr();
			screen_printf("\n\n\nGAME PAUSED\nPRESS SPACE TO CONTINUE\n");
			while (!keybuffer[SxSPACE])
			{
				wait_event();
			}
		}

		if (keybuffer[SxENTER]) // ALOITA ALUSTA
		{
			x = 15;
			y = 160;
			lautalla = 'K';
			hyppy = -1;
		}

		if (keybuffer[SxRIGHT] && x < 305) // LIIKU OIKEAAN
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
			for (lasku = 0; lasku < seina_maara; lasku++) // OSUUKO SEINääN
			{
				if (seinax[lasku] > x && seinax[lasku] - 11 < x && seinay[lasku] < y + 30 && seinay[lasku] + 30 > y)
					x = x - 3;
			}
		}

		if (keybuffer[SxLEFT] && x > 5) // LIIKU VASEMPAAN
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
			for (lasku = 0; lasku < 10; lasku++) // OSUUKO SEINääN
			{
				if (seinax[lasku] + 5 > x && seinax[lasku] < x && seinay[lasku] < y + 30 && seinay[lasku] + 30 > y)
					x = x + 3;
			}
		}

		if (keybuffer[SxCTRL] && lautalla == 'K') // HYPPää!
			hyppy = 6;
		
		if (keybuffer[ALLEGRO_KEY_RSHIFT] && ase <= -20)
		{
			ase = 20;
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

		piirra_boxi(0, 190, 320, 200, YELLOW);
		for (lasku = 0; lasku < 20; lasku++)
		{
			piirra_boxi(lauttax[lasku] - 1, lauttay[lasku], lauttax[lasku] + 50, lauttay[lasku] + 5, YELLOW);
			if (lasku < 10)
				piirra_boxi(seinax[lasku], seinay[lasku], seinax[lasku] + 5, seinay[lasku] + 30, YELLOW);
		}

		for (lasku = 0; lasku < 10; lasku++)
		{
			if (diamondx[lasku] < 500)
				sprite_do(diamondx[lasku], diamondy[lasku], SP_DIAMOND_W, SP_DIAMOND_H, SP_DIAMOND, 2);
		}

		int ase_x = 999, ase_y = 999;
		if (ase > 0)
		{
			float vx = cosf(3.14159f / 10.0f * ase), vy = sinf(3.14159f / 10.0f * ase);

			for (lasku = 0; lasku < 5; lasku++)
			{
				ase_x = vx * 10 * lasku + x + 5;
				ase_y = vy * 10 * lasku + y + 20;
				if (lasku < 4)
					piirra_boxi(ase_x - 1, ase_y - 1, ase_x + 1, ase_y + 1, 0);
				else
					piirra_boxi(ase_x - 4, ase_y - 4, ase_x + 4, ase_y + 4, 0);
			}
		}
		ase--;

		for (lasku = 0; lasku < bat_maara; lasku++)
		{
			if (bat_x[lasku] >= 500)
				continue;
			if (bat_x[lasku] + 27 > ase_x && bat_x[lasku] - 5 < ase_x && bat_y[lasku] - 4 < ase_y && bat_y[lasku] + 13 > ase_y)
			{
				aika -= 20;
				bat_x[lasku] = 500;
				piirra_boxi(0, 0, 320, 200, BRIGHT_GREEN);
				set_sfx(60, 60, 20, 20);
				continue;
			}

			if (bat_x[lasku] + 10 > x && bat_x[lasku] < x + 14 && bat_y[lasku] < y + 32 && bat_y[lasku] + 10 > y)
			{
				lives--;
				bat_x[lasku] = 500;
				x = 15;
				y = 160;
				lautalla = 'K';
				hyppy = -1;
				piirra_boxi(0, 0, 320, 200, LIGHTRED);
				set_sfx(20, 15, 10, 5);
				continue;
			}
			if (rand() % 2)
			{
				if (bat_x[lasku] < x - 5)
					bat_x[lasku] += 1 + rand() % 2;
				else if (bat_x[lasku] > x + 5)
					bat_x[lasku] -= 1 + rand() % 2;
			}
			if (rand() % 2)
			{
				if (bat_y[lasku] < y - 5)
					bat_y[lasku] += 1 + rand() % 2;
				else if (bat_y[lasku] > y + 5)
					bat_y[lasku] -= 1 + rand() % 2;
			}
			if (rand() % 5 == 0)
			{
				bat_x[lasku] += 1 - 2 * (rand() % 2);
				bat_y[lasku] += 1 - 2 * (rand() % 2);
			}
			sprite_do(bat_x[lasku], bat_y[lasku], SP_BAT_W, SP_BAT_H, SP_BAT(bat_x[lasku] % 2), 2);
		}
		sprite_do(x, y - 2, SP_PLAYER_W, SP_PLAYER_H, anim[0], 2);

		for (lasku = 0; lasku < lives; lasku++) // PIIRRä ELäMäT
			sprite_do(lasku * 10, 1, SP_DIAMOND_W, SP_DIAMOND_H, SP_DIAMOND, 1);

		piirra_boxi(125, 1, 125 + (175 - aika / 2), 6, LIGHTRED); // PIIRRä AIKA

		aika++;
		if (aika > 350)
		{
			lives--;
			aika = 0;
		}
		if (aika > 300)
		{
			set_sfx(10 * aani, 0, 0, 0);
		}
		grtila('F');
		viive(1);
	}
	piirra_boxi(0, 0, 320, 200, 0);
	clrscr();
	screen_printf("\n\n         GAME OVER\nYOU REACHED LEVEL: %d / 15\n\n\n\n\n    PRESS SPACE", level);
	FLIP;
	while (!keybuffer[SxSPACE])
	{
		wait_event();
	}
	set_sfx(20, 30, 40, 50);
	goto alku;
}

void grtila(int tila)
{
	/*if (tila == 1)
		textmode(0x13);
	if (tila == 0)
		textmode(0x3);
	if (tila == 'A')
	{

		buf = (char *)malloc(64000); // Alustetaan puskuri
		if (buf == NULL)
		{
			screen_printf("Not enought conventional memory!\n");
			resetkeyhandler();
			exit(1);
		}
	}*/
	if (tila == 'F')
		al_flip_display();
}

void piirra_boxi(int x1, int y1, int x2, int y2, char boxvari)
{
	x1 *= 3;
	x2 *= 3;
	y1 *= 3;
	y2 *= 3;
	al_draw_filled_rectangle(x1, y1, x2, y2, ega_color(boxvari));
}

char *get_sprite(char kumpi)
{
	if (kumpi == SP_LEFT_FACING(0) || kumpi == 'S')
		return aijavas1;
	if (kumpi == SP_LEFT_FACING(1))
		return aijavas2;
	if (kumpi == SP_RIGHT_FACING(0))
		return aijaoik1;
	if (kumpi == SP_RIGHT_FACING(1))
		return aijaoik2;
	if (kumpi == SP_DIAMOND)
		return dia_spr;
	if (kumpi == SP_BAT(0))
		return bat1;
	if (kumpi == SP_BAT(1))
		return bat2;
	return NULL;
}

void sprite_do(int _x, int _y, int aluex, int aluey, char kumpi, char lahella)
{
	char _123x, _123y = 0;
	int laskuri = 0;
	char *sprite = get_sprite(kumpi);
	if (!sprite)
		return;

	while (aluey * lahella > _123y)
	{
		for (_123x = 0; _123x < aluex * lahella; _123x = _123x + lahella)
		{
			if (sprite[laskuri] != 'n')
				piirra_boxi(_x + _123x, _y + _123y, _x + _123x + lahella, _y + _123y + lahella, sprite[laskuri]);
			laskuri++;
		}
		_123y = _123y + lahella;
	}
}
void sprite_read(char kumpi)
{
	char luetkos = 10;
	int laskuri = 0;
	char *sprite = get_sprite(kumpi);
	if (!sprite)
		return;

	FILE *f = fopen(GAME_DATA_FILE, "r");
	read_game_data_file_until(f, "sprite", kumpi);
	
	while (luetkos != 'E')
	{
		if (luetkos != 10)
		{
			if (luetkos == ' ')
				luetkos = 'n' + 47;
			// Skin color was done using some weird hack, let's do less weird hack
			if (luetkos == '*')
				luetkos = SKIN + 47;
			sprite[laskuri] = luetkos - 47;

			laskuri++;
		}
		luetkos = fgetc(f);
	}
	while (luetkos != 'E');
}

