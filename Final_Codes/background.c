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
	char* path = "Assets/clear_night.png";
	background = al_load_bitmap(path);
}

void update_bg(void) {
    bg_animation_tick = (bg_animation_tick + 1) % 64;
    if (bg_animation_tick % 4 == 0) {
        bg_offset += 1;
        if (bg_offset == 1024) {
            bg_offset = 0;
        }
    }
}

void draw_bg(void) {
    al_draw_tinted_scaled_bitmap(
        background,
        al_map_rgb(255, 255, 255),
        bg_offset, 0, SCREEN_W, 538,
        0, 0, SCREEN_W, SCREEN_H,
        0
    );
}

void destroy_bg(void) {
    al_destroy_bitmap(background);
}