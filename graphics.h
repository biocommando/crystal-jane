#ifndef crystal_sprite_h
#define crystal_sprite_h
#include "common.h"

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

void sprite_read(char sprite, char *sprite_buf, int sprite_size);

void sprite_do(int _x, int _y, int area_x, int area_y, char sprite, char zoom_mult);

void draw_box_gradient(int x1, int y1, int x2, int y2, char box_color_top, char box_color_bottom, int symmetric);
void draw_box(int x1, int y1, int x2, int y2, char box_color);
void clear_screen_to_color(int col);
void clear_screen_for_text();
#define FLIP al_flip_display();

void draw_world(struct level_info *world);

#endif