#include "graphics.h"

#include "allegro_compat.h"
#include "game_data.h"

#define MAX_SPRITES 16

struct
{
    char *buf;
    char id;
} sprite_cache[MAX_SPRITES];

int num_sprites = 0;

char *get_sprite(char sprite)
{
    for (int i = 0; i < num_sprites; i++)
    {
        if (sprite == sprite_cache[i].id)
            return sprite_cache[i].buf;
    }
    return NULL;
}

void sprite_read(char sprite, char *sprite_buf)
{
    if (num_sprites < MAX_SPRITES)
    {
        sprite_cache[num_sprites].buf = sprite_buf;
        sprite_cache[num_sprites].id = sprite;
        num_sprites++;
        game_data_read_sprite(sprite, sprite_buf);
    }
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
    extern int scaling;
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
    extern int scaling;
	x1 *= scaling;
	x2 *= scaling;
	y1 *= scaling;
	y2 *= scaling;
	al_draw_filled_rectangle(x1, y1, x2, y2, ega_color(box_color));
}