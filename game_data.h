#ifndef game_data_h
#define game_data_h
#include <stdio.h>

#define MAX_OBJ 50

struct level_info
{
	int platform_x[MAX_OBJ], platform_y[MAX_OBJ], wall_x[MAX_OBJ],
		wall_y[MAX_OBJ], diamond_x[MAX_OBJ], diamond_y[MAX_OBJ],
		bat_x[MAX_OBJ], bat_y[MAX_OBJ], bat_status[MAX_OBJ],
		diamond_anim[MAX_OBJ];
	int platform_count, wall_count, diamond_count, bat_count;
    char level_name[32];
};

int read_game_data_file_until(FILE *f, const char *title, char id);

int read_game_data_setting(const char *setting_name, int default_value);

int get_best_time_for_level(int level);

void set_best_time_for_level(int level, int best_time);

int set_game_data_file_name(const char *name);

void set_highscore(int score);

int get_highscore();

FILE *get_game_data_file();

void read_level(int level, struct level_info *info);

void game_data_read_sprite(char sprite, char *sprite_buf);

#endif