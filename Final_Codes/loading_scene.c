#include <allegro5/allegro.h>
#include <string.h>
#include "menu_scene.h"
#include "loading_scene.h"
#include "game_scene.h"
#include "setting_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"
#include "timetravel.h"

static int timer_countdown;
static ALLEGRO_BITMAP* player_bitmap;
static ALLEGRO_BITMAP* player_plane;

static void init(void) {
    timer_countdown = 60;
    if (timetravel) {
        timer_countdown = 160;
    }

    player_bitmap = al_load_bitmap("Assets/panda2.png");
    if (!player_bitmap) {
        game_abort("Failed to load player bitmap");
    }

    player_plane = al_load_bitmap("Assets/timetravel/player_ship.png");
    if (!player_plane) {
        game_abort("Failed to load player plane");
    }

    change_bgm("None");
    gameDone = false;
}

static void update(void) {
    timer_countdown--;
    if (timer_countdown == 0) {
        if (timetravel) {
            change_scene(create_timetravel_scene());
        }
        else if (!timetravel) {
            change_scene(create_game_scene());
        }
        
    }
}

static void draw(void) {
    if (timetravel) {
        char time_travel_text[20] = "TIME TRAVELLING";

        al_draw_text(
            TITLE_FONT,
            al_map_rgb(146, 161, 185),
            SCREEN_W / 2,
            SCREEN_H / 2 - 10,
            ALLEGRO_ALIGN_CENTER,
            time_travel_text
        );

        if (timer_countdown < (160 / 8) * 6) {
            al_draw_scaled_bitmap(player_plane,
                0, 0, 16, 16,
                SCREEN_W / 2 - (64 / 2) - 64 - 40, SCREEN_H / 2 + 100, 64, 64,
                0);
        }
        if (timer_countdown < (160 / 8) * 4) {
            al_draw_scaled_bitmap(player_plane,
                16, 0, 16, 16,
                SCREEN_W / 2 - (64 / 2), SCREEN_H / 2 + 100, 64, 64,
                0);
        }
        if (timer_countdown < (160 / 8) * 2) {
            al_draw_scaled_bitmap(player_plane,
                32, 0, 16, 16,
                SCREEN_W / 2 - (64 / 2) + 64 + 40, SCREEN_H / 2 + 100, 64, 64,
                0);
        }
 
    }

    else if (!timetravel){
        char map_num[3];
        snprintf(map_num, sizeof(map_num), "%d", map_number + 1);
        char level_num[20] = "LEVEL ";
        strcat_s(level_num, sizeof(level_num), map_num);

        if (map_number == 3) {
            strcpy_s(level_num, sizeof(level_num), "Final Boss");
            al_draw_text(
                TITLE_FONT,
                al_map_rgb(146, 161, 185),
                SCREEN_W / 2,
                SCREEN_H / 2,
                ALLEGRO_ALIGN_CENTER,
                level_num
            );
            return;
        }
        al_draw_text(
            TITLE_FONT,
            al_map_rgb(146, 161, 185),
            SCREEN_W / 2 + 40,
            SCREEN_H / 2,
            ALLEGRO_ALIGN_CENTER,
            level_num
        );


        al_draw_scaled_bitmap(player_bitmap,
            0, 0, 32, 32,
            SCREEN_W / 4, SCREEN_H / 2, 64, 64, 0);
    }

}

static void destroy(void) {
    al_destroy_bitmap(player_bitmap);
}


Scene create_loading_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "loading";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}
