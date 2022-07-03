#include "allegro_compat.h"
#include "game_data.h"
#include <stdio.h>
#include <string.h>

EXTERN_GLOBALS;

char game_data_file_name[256] = "gdat.dat";
char hiscore_file_name[256] = "hiscore.jan";
char best_times_file_name[256] = "best_times.jan";

int read_game_data_file_until(FILE *f, const char *title, char id)
{
    verbose_log("Read data %s:%c\n", title, id);
    fseek(f, 0, SEEK_SET);
    while (!feof(f))
    {
        char b[100], rtitle[100] = "", rid = 0;
        fgets(b, 100, f);
        sscanf(b, "@%s %c", rtitle, &rid);
        if (!strcmp(title, rtitle) && (id == 0 || rid == id))
        {
            return 0;
        }
    }
    verbose_log("failed to locate data %s:%c\n", title, id);
    return 1;
}

int read_game_data_setting(const char *setting_name, int default_value)
{
    int value = default_value;
    FILE *f = fopen(game_data_file_name, "r");
    if (!read_game_data_file_until(f, setting_name, 0))
    {
        fscanf(f, "%d", &value);
    }
    fclose(f);
    return value;
}

int get_best_time_for_level(int level)
{
    FILE *f = fopen(best_times_file_name, "rb");
    if (!f)
        return -1;
    fseek(f, sizeof(int) * level, SEEK_SET);
    int tm;
    fread(&tm, sizeof(int), 1, f);
    fclose(f);
    return tm;
}

void set_best_time_for_level(int level, int best_time)
{
    int times[15];
    for (int i = 0; i < 15; i++)
    {
        times[i] = i == level ? best_time : get_best_time_for_level(i);
    }

    FILE *f = fopen(best_times_file_name, "wb");
    fwrite(times, sizeof(int), 15, f);
    fclose(f);
}

int set_game_data_file_name(const char *name)
{
    if (snprintf(game_data_file_name, sizeof(game_data_file_name), "%s", name) < 0 ||
        snprintf(hiscore_file_name, sizeof(hiscore_file_name), "%s-hiscore.jan", name) < 0 ||
        snprintf(best_times_file_name, sizeof(best_times_file_name), "%s-best_times.jan", name) < 0)
    {
        return 1;
    }
    return 0;
}

void set_highscore(int score)
{
    FILE *fhscore = fopen(hiscore_file_name, "w");
    fprintf(fhscore, "%d", score ^ 0xFEFEFEFE);
    fclose(fhscore);
}

int get_highscore()
{
    int hiscore;
    FILE *fhscore = fopen(hiscore_file_name, "r");
    if (fhscore)
    {
        fscanf(fhscore, "%d", &hiscore);
        fclose(fhscore);
    }
    if (hiscore & 0xFE000000)
        hiscore ^= 0xFEFEFEFE;
    else
        hiscore = 0;
    return hiscore;
}

FILE *get_game_data_file()
{
    return fopen(game_data_file_name, "r");
}

void read_level(int level, struct level_info *info)
{
    memset(info, 0, sizeof(struct level_info));
    memset(info->bat_status, 1, sizeof(info->bat_status));
    FILE *game_data = get_game_data_file();
    read_game_data_file_until(game_data, "level", LEVEL_ID(level));
    fgets(info->level_name, 32, game_data);
    while (1)
    {
        char file_read_buf[32];
        fgets(file_read_buf, sizeof(file_read_buf), game_data);
        if (feof(game_data) || file_read_buf[0] == '@')
            break;
        int rx, ry, type = 0;
        sscanf(file_read_buf, "%d %d %c", &rx, &ry, &type);
        if (type == 'H' && info->platform_count < MAX_OBJ)
        {
            info->platform_x[info->platform_count] = rx;
            info->platform_y[info->platform_count] = ry;
            info->platform_count++;
        }
        else if (type == 'V' && info->wall_count < MAX_OBJ)
        {
            info->wall_x[info->wall_count] = rx;
            info->wall_y[info->wall_count] = ry;
            info->wall_count++;
        }
        else if (type == 'D' && info->diamond_count < MAX_OBJ)
        {
            info->diamond_x[info->diamond_count] = rx;
            info->diamond_y[info->diamond_count] = ry;
            info->diamond_count++;
        }
        else if (type == 'B' && info->bat_count < MAX_OBJ)
        {
            info->bat_x[info->bat_count] = rx;
            info->bat_y[info->bat_count] = ry;
            info->bat_count++;
        }
    }
    fclose(game_data);
}

void game_data_read_sprite(char sprite, char *sprite_buf, int sprite_size)
{
	FILE *f = fopen(game_data_file_name, "r");
	read_game_data_file_until(f, "sprite", sprite);

	char read_result = '\n';
	int counter = 0;

	while (read_result != 'E' && counter < sprite_size)
	{
		if (read_result != '\n')
		{
			if (read_result == ' ')
				sprite_buf[counter] = TRANSPARENT;
			// Skin color was done using some weird hack, let's do less weird hack
			else if (read_result == '*')
				sprite_buf[counter] = SKIN;
			else
				sprite_buf[counter] = read_result - 47;

			counter++;
		}
		read_result = fgetc(f);
	}
}