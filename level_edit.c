
#include "allegro_compat.h"
#include "game_data.h"
#include "graphics.h"
#include "crystal.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

EXTERN_GLOBALS;

#include "build_info.h"

void save_level_data(FILE *f, int *x, int *y, int count, char type)
{
    for (int i = 0; i < count; i++)
    {
        if (x[i] < 999)
            fprintf(f, "%d %d %c\n", x[i], y[i], type);
    }
}

void save_level(int level, struct level_info *info, const char *filename)
{
    FILE *f = fopen(filename, "w");
    fprintf(f, "@level %c\n", LEVEL_ID(level));
    fprintf(f, "%s", info->level_name);
    save_level_data(f, info->platform_x, info->platform_y, info->platform_count, 'H');
    save_level_data(f, info->wall_x, info->wall_y, info->wall_count, 'V');
    save_level_data(f, info->diamond_x, info->diamond_y, info->diamond_count, 'D');
    save_level_data(f, info->bat_x, info->bat_y, info->bat_count, 'B');
    fclose(f);
}

void create_obj(int *xbuf, int *ybuf, int *bmax, int x, int y)
{
    int idx = -1;
    for (int i = 0; i < *bmax; i++)
    {
        if (xbuf[i] == 999)
        {
            idx = i;
            break;
        }
    }
    if (*bmax < MAX_OBJ)
    {
        idx = *bmax;
        *bmax = idx + 1;
    }
    if (idx == -1)
        return;
    xbuf[idx] = x;
    ybuf[idx] = y;
}

int main(int argc, char **argv)
{
    verbose_logging = 1;
    printf(
        "CRYSTAL JANE -- LEVEL EDITOR\n\nversion " VERSION " - Built on " BUILD_DATE
        "\nCopyrights 2004-2022 by Joonas Salonpaa\nMIT licensed\n");

    if (GET_ARG('h'))
    {
        printf(
            "Command line arguments:\n"
            "h = help\n"
            "L[NUM] = level number\n"
            "s[FILE] = sprite file\n"
            "i[FILE] = input file (if none given, create new level)\n"
            "o[FILE] = output file\n");
    }

    set_sfx_off(OPT_ALL_SOUND_OFF);

    init_allegro(4, 0);
    scaling = 4;

    if (GET_ARG('s'))
    {
        set_game_data_file_name(GET_ARG('s'));
    }

    struct level_info world;

    int level = 0;
    if (GET_ARG('L'))
        level = atoi(GET_ARG('L')) - 1;
    if (GET_ARG('i'))
    {
        set_game_data_file_name(GET_ARG('i'));
        read_level(level, &world);
    }
    else
    {
        memset(&world, 0, sizeof(struct level_info));
        memset(world.bat_status, 1, sizeof(world.bat_status));
    }

    int x = 160, y = 100;
    int selected_type = 'H';
    int current_object_idx = 0;

    read_sprites();

    while (!keybuffer[ALLEGRO_KEY_ESCAPE])
    {
        draw_world(&world);
        sprite_do(15, 160, SP_PLAYER_W, SP_PLAYER_H, SP_RIGHT_FACING(0), 2);

        if (keybuffer[ALLEGRO_KEY_LEFT])
            x -= keybuffer[ALLEGRO_KEY_ALT] ? 1 : 5;
        if (keybuffer[ALLEGRO_KEY_RIGHT])
            x += keybuffer[ALLEGRO_KEY_ALT] ? 1 : 5;
        if (keybuffer[ALLEGRO_KEY_UP])
            y -= keybuffer[ALLEGRO_KEY_ALT] ? 1 : 5;
        if (keybuffer[ALLEGRO_KEY_DOWN])
            y += keybuffer[ALLEGRO_KEY_ALT] ? 1 : 5;
        if (keybuffer[ALLEGRO_KEY_H])
            selected_type = 'H';
        if (keybuffer[ALLEGRO_KEY_V])
            selected_type = 'V';
        if (keybuffer[ALLEGRO_KEY_D])
            selected_type = 'D';
        if (keybuffer[ALLEGRO_KEY_B])
            selected_type = 'B';

        if (keybuffer[ALLEGRO_KEY_SPACE])
        {
            for (int i = 0; i < world.platform_count; i++)
            {
                if (abs(x - world.platform_x[i]) < 10 && abs(y - world.platform_y[i]) < 10)
                {
                    selected_type = 'H';
                    current_object_idx = i;
                    al_draw_circle(x * scaling, y * scaling, 20, al_map_rgb(255, 0, 255), 4);
                    break;
                }
            }
            for (int i = 0; i < world.wall_count; i++)
            {
                if (abs(x - world.wall_x[i]) < 10 && abs(y - world.wall_y[i]) < 10)
                {
                    selected_type = 'V';
                    current_object_idx = i;
                    al_draw_circle(x * scaling, y * scaling, 20, al_map_rgb(255, 0, 255), 4);
                    break;
                }
            }
            for (int i = 0; i < world.diamond_count; i++)
            {
                if (abs(x - world.diamond_x[i]) < 10 && abs(y - world.diamond_y[i]) < 10)
                {
                    selected_type = 'D';
                    current_object_idx = i;
                    al_draw_circle(x * scaling, y * scaling, 20, al_map_rgb(255, 0, 255), 4);
                    break;
                }
            }
            for (int i = 0; i < world.bat_count; i++)
            {
                if (abs(x - world.bat_x[i]) < 10 && abs(y - world.bat_y[i]) < 10)
                {
                    selected_type = 'B';
                    current_object_idx = i;
                    al_draw_circle(x * scaling, y * scaling, 20, al_map_rgb(255, 0, 255), 4);
                    break;
                }
            }
        }
        if (keybuffer[ALLEGRO_KEY_M])
        {
            if (selected_type == 'H' && world.platform_count > current_object_idx)
            {
                world.platform_x[current_object_idx] = x;
                world.platform_y[current_object_idx] = y;
            }
            if (selected_type == 'V' && world.wall_count > current_object_idx)
            {
                world.wall_x[current_object_idx] = x;
                world.wall_y[current_object_idx] = y;
            }
            if (selected_type == 'D' && world.diamond_count > current_object_idx)
            {
                world.diamond_x[current_object_idx] = x;
                world.diamond_y[current_object_idx] = y;
            }
            if (selected_type == 'B' && world.bat_count > current_object_idx)
            {
                world.bat_x[current_object_idx] = x;
                world.bat_y[current_object_idx] = y;
            }
        }
        
        if (keybuffer[ALLEGRO_KEY_DELETE])
        {
            if (selected_type == 'H' && world.platform_count > current_object_idx)
            {
                world.platform_x[current_object_idx] = 999;
            }
            if (selected_type == 'V' && world.wall_count > current_object_idx)
            {
                world.wall_x[current_object_idx] = 999;
            }
            if (selected_type == 'D' && world.diamond_count > current_object_idx)
            {
                world.diamond_x[current_object_idx] = 999;
            }
            if (selected_type == 'B' && world.bat_count > current_object_idx)
            {
                world.bat_x[current_object_idx] = 999;
            }
        }
        
        if (keybuffer[ALLEGRO_KEY_INSERT])
        {
            if (selected_type == 'H')
            {
                create_obj(world.platform_x, world.platform_y, &world.platform_count, x, y);
            }
            if (selected_type == 'V')
            {
                create_obj(world.wall_x, world.wall_y, &world.wall_count, x, y);
            }
            if (selected_type == 'D')
            {
                create_obj(world.diamond_x, world.diamond_y, &world.diamond_count, x, y);
            }
            if (selected_type == 'B')
            {
                create_obj(world.bat_x, world.bat_y, &world.bat_count, x, y);
            }
            wait_delay(3);
        }

        if (keybuffer[ALLEGRO_KEY_F10])
        {
            save_level(level, &world, "playtest.dat");
            extern char game_data_file_name[256];
            sprintf(game_data_file_name, "playtest.dat");
            struct game_settings gs = create_game_settings();
            gs.repeat_level = 1;
            gs.start_level = level;
            game_logic(gs);
        }

        if (selected_type == 'H')
            al_draw_circle(x * scaling, y * scaling, 10, al_map_rgb(255, 255, 255), 4);
        if (selected_type == 'V')
            al_draw_circle(x * scaling, y * scaling, 10, al_map_rgb(0, 255, 255), 4);
        if (selected_type == 'D')
            al_draw_circle(x * scaling, y * scaling, 10, al_map_rgb(255, 128, 128), 4);
        if (selected_type == 'B')
            al_draw_circle(x * scaling, y * scaling, 10, al_map_rgb(64, 64, 255), 4);

        FLIP;
        wait_delay(1);
    }
    save_level(level, &world, GET_ARG('o') ? GET_ARG('o') : "lev-edit-output.dat");
}