#ifndef game_data_h
#define game_data_h
#include <stdio.h>
#include "common.h"

int read_game_data_file_until(FILE *f, const char *title, char id);

int read_game_data_setting(const char *setting_name, int default_value);

int get_best_time_for_level(int level);

void set_best_time_for_level(int level, int best_time);

int set_game_data_file_name(const char *name);

void set_highscore(int score);

int get_highscore();

FILE *get_game_data_file();

void read_level(int level, struct level_info *info);

void game_data_read_sprite(char sprite, char *sprite_buf, int sprite_size);

#endif