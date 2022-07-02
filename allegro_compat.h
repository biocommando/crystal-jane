#ifndef allegro_compat_h
#define allegro_compat_h

#include "allegro5/allegro.h"
#include "allegro5/allegro_image.h"
#include "allegro5/allegro_font.h"
#include "allegro5/allegro_primitives.h"
#include "allegro5/allegro_audio.h"

#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define BROWN 6
#define WHITE 7
#define DARK_GRAY 8
#define DARKGRAY DARK_GRAY
#define BRIGHT_BLUE 9
#define BRIGHT_GREEN 10
#define BRIGHT_CYAN 11
#define BRIGHT_RED 12
#define LIGHTRED BRIGHT_RED
#define BRIGHT_MAGENTA 13
#define BRIGHT_YELLOW 14
#define YELLOW BRIGHT_YELLOW
#define BRIGHT_WHITE 15
#define SKIN 100

#define SxENTER ALLEGRO_KEY_ENTER
#define SxESC ALLEGRO_KEY_ESCAPE
#define Sx1 ALLEGRO_KEY_1
#define Sx2 ALLEGRO_KEY_2
#define SxP ALLEGRO_KEY_P
#define SxSPACE ALLEGRO_KEY_SPACE

#define SYNTH_FLAGS(volume, envelope, synth_idx) \
    (((volume) | (((envelope) << 4) & 0xF0)) << (synth_idx * 8))

#define SYNTH_SETTINGS        \
    (SYNTH_FLAGS(5, 7, 3) | \
     SYNTH_FLAGS(9, 1, 2) |   \
     SYNTH_FLAGS(5, 0, 1) |   \
     SYNTH_FLAGS(3, 1, 0))

#define screen_printf(...)                       \
    do                                           \
    {                                            \
        char screen_printf_buf[1024];            \
        sprintf(screen_printf_buf, __VA_ARGS__); \
        stateful_draw_text(screen_printf_buf);   \
    } while (0)

ALLEGRO_COLOR ega_color(char col);

int init_allegro();
int wait_event();
void viive(int v);

void sound(int freq);
void clrscr();
void stateful_draw_text(const char *s);

void set_sfx(int a, int b, int c, int d);
int get_sfx_pos();

#endif