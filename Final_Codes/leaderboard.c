#include <allegro5/allegro.h>
#include <string.h>
#include "menu_scene.h"
#include "leaderboard.h"
#include "game_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"

leaderboard *leaderboards;

Button backButton;

ALLEGRO_BITMAP* trophy_bitmap;

ALLEGRO_BITMAP* leaderboard_bitmap;

int size;

void sort_leaderboard(leaderboard* leaderboards, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = i; j < size; j++) {
            if (leaderboards[j].point > leaderboards[i].point) {
                leaderboard temp = leaderboards[i];
                leaderboards[i] = leaderboards[j];
                leaderboards[j] = temp;
            }
        }
    }
}

void init(void) {
    char* trophy_path = "Assets/trophy.png";

    char* leaderboard_path = "Assets/leaderboard.png";

    trophy_bitmap = al_load_bitmap(trophy_path);

    leaderboard_bitmap = al_load_bitmap(leaderboard_path);

    char* path = "Assets/leaderboard.txt";
    FILE* f = fopen(path, "r");
    if (!f) {
        game_abort("Can't load map file : %s", path);
    }

    fscanf_s(f, "%d", &size);

    leaderboards = malloc(size * sizeof(leaderboard));

    for (int i = 0; i < size; i++) {
        fscanf_s(f, "%s %d", leaderboards[i].name, (unsigned)sizeof(leaderboards[i].name),
            &leaderboards[i].point);

    }

    sort_leaderboard(leaderboards, size);

    fclose(f);

    backButton = button_create(SCREEN_W / 2 - (200 / 2), SCREEN_H - 130, 200, 100,
        "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");

}

void update(void) {
    update_button(&backButton);

    if (backButton.hovered && mouseState.buttons) {
        change_scene(create_menu_scene());
    }
}

void draw(void) {
    al_draw_scaled_bitmap(trophy_bitmap,
        0, 0, 32, 32, 
        180, 50, 50, 50,
        0);

    al_draw_scaled_bitmap(trophy_bitmap,
        0, 0, 32, 32,
        SCREEN_W - 230, 50, 50, 50,
        0);

    al_draw_scaled_bitmap(leaderboard_bitmap,
        0, 0, 170, 25,
        235, 53, (SCREEN_W - 230) - 180 - 53, 50,
        0);

    int startx = 135;
    int starty = 160;

    al_draw_filled_rounded_rectangle(
        startx - 10, starty - 10,
        SCREEN_W - 125, SCREEN_H - 175,
        50, 50,
        al_map_rgb(0, 109, 191));
    al_draw_filled_rounded_rectangle(
        startx, starty,
        SCREEN_W - 135, SCREEN_H - 185,
        50, 50,
        al_map_rgb(46, 146, 255));
    

    draw_button(backButton);
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        backButton.x + (backButton.w / 2),
        (backButton.y + 7) + 20 + backButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "BACK"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        backButton.x + (backButton.w / 2),
        (backButton.y + 7) + 24 + backButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "BACK"
    );
    size = (size > 5) ? 5 : size;
    for (int i = 0; i < size; i++) {
        char number[4];
        snprintf(number, sizeof(number), "%d", (i + 1));
        strcat_s(number, sizeof(number), ".");
        al_draw_text(
            P2_FONT,
            al_map_rgb(225, 225, 225),
            startx + 60,
            starty + (i * 75) + 50,
            ALLEGRO_ALIGN_RIGHT,
            number
        );

        al_draw_text(
            P2_FONT,
            al_map_rgb(225, 225, 225),
            startx + 60 + 40,
            starty + (i * 75) + 50,
            ALLEGRO_ALIGN_LEFT,
            leaderboards[i].name
        );

        char point[6];
        snprintf(point, sizeof(point), "%d", leaderboards[i].point);

        al_draw_text(
            P2_FONT,
            al_map_rgb(225, 225, 225),
            startx + 60 + 40 + 310,
            starty + (i * 75) + 50,
            ALLEGRO_ALIGN_LEFT,
            point
        );

    }
}

void destroy(void) {
    free(leaderboards);
    destroy_button(&backButton);

    al_destroy_bitmap(trophy_bitmap);
    al_destroy_bitmap(leaderboard_bitmap);
}


Scene create_leaderboard_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "leaderboard";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}

