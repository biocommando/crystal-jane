#ifndef crystal_common_h
#define crystal_common_h

#define MAX_OBJ 50

struct level_info
{
    int platform_x[MAX_OBJ], platform_y[MAX_OBJ], wall_x[MAX_OBJ],
        wall_y[MAX_OBJ], diamond_x[MAX_OBJ], diamond_y[MAX_OBJ],
        bat_x[MAX_OBJ], bat_y[MAX_OBJ], bat_status[MAX_OBJ],
        diamond_anim[MAX_OBJ];
    int platform_count, wall_count, diamond_count, bat_count;
    int init_x, init_y;
    char level_name[32];
};

struct game_settings
{
    int final_level, enable_sprint, enable_high_jump, enable_weapon,
		jump_height, high_jump_height, max_lives, time_counter_limit;
    int start_level, skip_splash_screen, repeat_level, is_mod;
};

struct game_settings create_game_settings();

#define verbose_log(...) \
    if (verbose_logging) \
    printf("DEBUG: " __VA_ARGS__)

#define EXTERN_GLOBALS                   \
    extern int verbose_logging, scaling; \
    extern char keybuffer[ALLEGRO_KEY_MAX]

const char *get_arg(int argc, char **argv, char flag);

#define GET_ARG(flag) get_arg(argc, argv, flag)

#define LEVEL_ID(level) level < 9 ? '1' + level : 'A' + level - 9

#endif
