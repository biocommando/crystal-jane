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
    char level_name[32];
};

#define verbose_log(...) \
    if (verbose_logging) \
    printf("DEBUG: " __VA_ARGS__)

#define EXTERN_GLOBALS                   \
    extern int verbose_logging, scaling; \
    extern char keybuffer[ALLEGRO_KEY_MAX]

#endif
