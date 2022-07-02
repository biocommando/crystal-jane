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
#define BRIGHT_BLUE 9
#define BRIGHT_GREEN 10
#define BRIGHT_CYAN 11
#define BRIGHT_RED 12
#define BRIGHT_MAGENTA 13
#define BRIGHT_YELLOW 14
#define BRIGHT_WHITE 15
#define SKIN 100
#define TRANSPARENT 0x7F

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

int get_frame_counter();

#define OPT_SFX_OFF 1
#define OPT_ALL_SOUND_OFF 2

void set_sfx_off(int state);

int init_allegro(int scale, int audio_buf_size);
int wait_event();
void wait_delay(int v);

void sound(int freq);
void clrscr();
void stateful_draw_text(const char *s);

void set_sfx(int a, int b, int c, int d);
int get_sfx_pos();

#endif