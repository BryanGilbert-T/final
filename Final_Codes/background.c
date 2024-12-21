#include <allegro5/allegro.h>
#include <string.h>
#include "menu_scene.h"
#include "loading_scene.h"
#include "game_scene.h"
#include "setting_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"

ALLEGRO_BITMAP* background;
int bg_animation_tick = 0;
int bg_offset = 0;

void init_bg(void) {
    char* path;
    if (dune) {
        path = "Assets/dune_desert.png";
    }
    else {
        path = "Assets/clear_night.png";
    }
	
	background = al_load_bitmap(path);
    if (!background) {
        game_abort("Failed to laod %s", path);
    }
}

void update_bg(void) {
    bg_animation_tick = (bg_animation_tick + 1) % 64;
    if (dune) {
        if (bg_animation_tick % 2 == 0) {
            bg_offset += 1;
            if (bg_offset == 1200) {
                bg_offset = 0;
            }
        }
    }
    else {
        if (bg_animation_tick % 4 == 0) {
            bg_offset += 1;
            if (bg_offset == 1024) {
                bg_offset = 0;
            }
        }
    }
  
}

void draw_bg(void) {
    if (dune) {
        al_draw_tinted_scaled_bitmap(
            background,
            al_map_rgb(255, 255, 255),
            bg_offset, 0, SCREEN_W, 300,
            0, 0, SCREEN_W, SCREEN_H,
            0
        );
    }
    else {
        al_draw_tinted_scaled_bitmap(
            background,
            al_map_rgb(255, 255, 255),
            bg_offset, 0, SCREEN_W, 538,
            0, 0, SCREEN_W, SCREEN_H,
            0
        );
    }
}

void destroy_bg(void) {
    al_destroy_bitmap(background);
}

ALLEGRO_BITMAP* timetravel_bg;
int timetravel_bg_offset;

void init_timetravel_bg(void) {
    char* path = "Assets/timetravel/clear_night.png";
    timetravel_bg = al_load_bitmap(path);
    if (!timetravel_bg) {
        game_abort("Failed to load %s", path);
    }
    timetravel_bg_offset = al_get_bitmap_height(timetravel_bg) - SCREEN_H;
}

void update_timetravel_bg(int speed) {
    timetravel_bg_offset -= speed;
    if (timetravel_bg_offset <= 0) {
        timetravel_bg_offset = al_get_bitmap_height(timetravel_bg) - SCREEN_H;
    }
}

void draw_timetravel_bg(void) {
    al_draw_tinted_scaled_bitmap(
        timetravel_bg,
        al_map_rgb(255, 255, 255),
        0, timetravel_bg_offset, 528, SCREEN_H,
        0, 0, SCREEN_W, SCREEN_H,
        0
    );
}

void destroy_timetravel_bg(void) {
    al_destroy_bitmap(timetravel_bg);
}