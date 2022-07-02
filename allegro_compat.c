#include "allegro_compat.h"
#include "synth.h"

#include <stdio.h>

ALLEGRO_FONT *font = NULL;
ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_EVENT_QUEUE *queue = NULL;
ALLEGRO_AUDIO_STREAM *audio_stream = NULL;
ALLEGRO_MIXER *mixer = NULL;

ALLEGRO_COLOR ega_color(char col)
{
    if (col == BLACK)
        return al_map_rgb(0x00, 0x00, 0x00);
    if (col == BLUE)
        return al_map_rgb(0x00, 0x00, 0xAA);
    if (col == GREEN)
        return al_map_rgb(0x00, 0xAA, 0x00);
    if (col == CYAN)
        return al_map_rgb(0x00, 0xAA, 0xAA);
    if (col == RED)
        return al_map_rgb(0xAA, 0x00, 0x00);
    if (col == MAGENTA)
        return al_map_rgb(0xAA, 0x00, 0xAA);
    if (col == BROWN)
        return al_map_rgb(0xAA, 0x55, 0x00);
    if (col == WHITE)
        return al_map_rgb(0xAA, 0xAA, 0xAA);
    if (col == DARK_GRAY)
        return al_map_rgb(0x55, 0x55, 0x55);
    if (col == BRIGHT_BLUE)
        return al_map_rgb(0x55, 0x55, 0xFF);
    if (col == BRIGHT_GREEN)
        return al_map_rgb(0x55, 0xFF, 0x55);
    if (col == BRIGHT_CYAN)
        return al_map_rgb(0x55, 0xFF, 0xFF);
    if (col == BRIGHT_RED)
        return al_map_rgb(0xFF, 0x55, 0x55);
    if (col == BRIGHT_MAGENTA)
        return al_map_rgb(0xFF, 0x55, 0xFF);
    if (col == BRIGHT_YELLOW)
        return al_map_rgb(0xFF, 0xFF, 0x55);
    if (col == BRIGHT_WHITE)
        return al_map_rgb(0xFF, 0xFF, 0xFF);
    if (col == SKIN)
        return al_map_rgb(239, 228, 176);
    return al_map_rgb(0, 0, 0);
}

char keybuffer[ALLEGRO_KEY_MAX];

int opt_sound_state = 0;

int audio_buf_size = 1024;
int screen_curs_x_offset = 0, screen_curs_y_offset = 0;

int init_allegro(int scaling, int opt_audio_buf_size)
{
    if (opt_audio_buf_size)
        audio_buf_size = opt_audio_buf_size;
    if (!al_init())
    {
        return 1;
    }
    screen_curs_x_offset = (scaling * 320) / 2 - 160;
    screen_curs_y_offset = (scaling * 200) / 2 - 100;
    memset(keybuffer, 0, sizeof(keybuffer));
    al_install_mouse();
    al_install_keyboard();
    al_init_image_addon();
    al_init_primitives_addon();
    al_init_font_addon();

    al_set_new_window_title("Crysal Jane");
    al_set_new_display_refresh_rate(60);
    al_set_new_display_flags(ALLEGRO_OPENGL);
    font = al_create_builtin_font();
    display = al_create_display(320 * scaling, 200 * scaling);
    if (!display)
    {
        return 1;
    }

    if (opt_sound_state != OPT_ALL_SOUND_OFF)
    {
        al_install_audio();
        al_reserve_samples(0);
        audio_stream = al_create_audio_stream(8, audio_buf_size, 44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);
        mixer = al_get_default_mixer();
        al_attach_audio_stream_to_mixer(audio_stream, mixer);
    }

    timer = al_create_timer(1.0 / 20);
    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    if (opt_sound_state != OPT_ALL_SOUND_OFF)
    {
        al_register_event_source(queue, al_get_audio_stream_event_source(audio_stream));
    }
    al_start_timer(timer);
    synth_init(SYNTH_SETTINGS);
    return 0;
}
ALLEGRO_EVENT event;

void set_sfx_off(int state)
{
    opt_sound_state = state;
}

void sound(int freq)
{
    if (opt_sound_state != OPT_SFX_OFF)
        trigger_sound(0, freq ? 32 + freq : 0);
}

char sfx[4], sfx_pos = -1;
void set_sfx(int a, int b, int c, int d)
{
    sound(a);
    sfx[0] = b;
    sfx[1] = c;
    sfx[2] = d;
    sfx[3] = 0;
    sfx_pos = 0;
}
int get_sfx_pos()
{
    return sfx_pos;
}

int al_frame_counter = 0;
int get_frame_counter()
{
    return al_frame_counter;
}

int wait_event()
{
    ALLEGRO_TIMEOUT tmo;
    al_init_timeout(&tmo, 0.1);
    int res = al_wait_for_event_until(queue, &event, &tmo);
    if (!res)
        return 0;
    if (event.type == ALLEGRO_EVENT_KEY_DOWN)
    {
        keybuffer[event.keyboard.keycode] = 1;
    }
    else if (event.type == ALLEGRO_EVENT_KEY_UP)
    {
        keybuffer[event.keyboard.keycode] = 0;
    }
    else if (event.type == ALLEGRO_EVENT_TIMER)
    {
        al_frame_counter++;
        if (sfx_pos >= 0)
        {
            sound(sfx[sfx_pos]);
            sfx_pos++;
            if (!sfx[sfx_pos])
                sfx_pos = -1;
        }
        else
        {
            sound(0);
        }
        return 1;
    }
    else if (event.type == ALLEGRO_EVENT_AUDIO_STREAM_FRAGMENT && opt_sound_state != OPT_ALL_SOUND_OFF)
    {
        ALLEGRO_AUDIO_STREAM *stream = (ALLEGRO_AUDIO_STREAM *)event.any.source;
        float *buf = (float *)al_get_audio_stream_fragment(stream);
        if (buf)
        {
            synth_process(buf, audio_buf_size * 2);
            al_set_audio_stream_fragment(stream, buf);
        }
    }
    return 2;
}

void wait_delay(int v)
{
    while (v > 0)
    {
        if (wait_event() == 1)
            v--;
    }
}

int screen_curs_x = 0, screen_curs_y = 0;

void clrscr()
{
    screen_curs_x = screen_curs_x_offset;
    screen_curs_y = screen_curs_y_offset;
}

void stateful_draw_text(const char *s)
{
    char chrbuf[3] = {' ', 0};
    for (int i = 0; s[i]; i++)
    {
        if (screen_curs_x == screen_curs_x_offset)
        {
            al_draw_filled_rectangle(screen_curs_x_offset - 15, screen_curs_y - 2, screen_curs_x_offset + 335, screen_curs_y + 18, al_map_rgb(128, 128, 128));
        }
        if (s[i] == '\n')
        {
            screen_curs_y += 12;
            screen_curs_x = screen_curs_x_offset;
            continue;
        }
        chrbuf[0] = s[i];
        al_draw_text(font, al_map_rgb(0, 0, 0), screen_curs_x, screen_curs_y, 0, chrbuf);
        screen_curs_x += 8;
    }
}