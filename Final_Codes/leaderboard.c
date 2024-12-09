#include <allegro5/allegro.h>
#include <string.h>
#include "menu_scene.h"
#include "leaderboard.h"
#include "game_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"
#include "background.h"

leaderboard *leaderboards;

Button backButton;

ALLEGRO_BITMAP* trophy_bitmap;

ALLEGRO_BITMAP* leaderboard_bitmap;

int size;

void insert_to_leaderboard(char name[11], int point) {
    char* path = "Assets/leaderboard.txt";
    FILE* f = fopen(path, "r+");

    if (!f) {
        game_abort("Can't load map file : %s", path);
    }

    int size;
    fscanf_s(f, "%d", &size);
    rewind(f);

    fprintf(f, "%d\n", size + 1);

    fseek(f, 0, SEEK_END);

    char final[20];
    sprintf_s(final, sizeof(final), "%s %d\n", name, point);

    fprintf(f, final);
    fclose(f);
    return;
}

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

ALLEGRO_BITMAP* winning_panda;
Button submitButton;
Form submitForm;
int no_name_animation_tick;
static void init_submit(void) {
    winning_panda = al_load_bitmap("Assets/panda_win.png");
    if (!winning_panda) {
        game_abort("Fail to load panda_win.png");
    }
    submitButton = button_create(SCREEN_W / 2 - 120, SCREEN_H - 150, 240, 120, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    submitForm = form_create(200, submitButton.y - 100,
        SCREEN_W - 200, submitButton.y - 30,
        25, 25,
        al_map_rgb(170, 170, 170),
        al_map_rgb(225, 225, 225));

    no_name_animation_tick = 0;
}

static void draw_submit(void) {
    char points[6];
    snprintf(points, sizeof(points), "%d", points_accumulated);
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        SCREEN_W / 2,
        submitButton.y - 160,
        ALLEGRO_ALIGN_CENTER,
        points
    );

    draw_button(submitButton);
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        submitButton.x + (submitButton.w / 2),
        (submitButton.y + 7) + 26 + submitButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "SUBMIT"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        submitButton.x + (submitButton.w / 2),
        (submitButton.y + 7) + 30 + submitButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "SUBMIT"
    );

    draw_form(submitForm);

    if (submitForm.input[0] == '\0' && !(submitForm.clicked)) {
        al_draw_text(P2_FONT,
            al_map_rgb(145, 145, 145),
            submitForm.x + ((submitForm.x2 - submitForm.x) / 2),
            submitForm.y + ((submitForm.y2 - submitForm.y) / 4) + 3,
            ALLEGRO_ALIGN_CENTER,
            "Enter Your Name");
    }

    al_draw_scaled_bitmap(winning_panda,
        0, 0, 64, 64,
        SCREEN_W / 4, SCREEN_W / 4 - 110, SCREEN_W / 2, SCREEN_H / 2,
        0);
}

static void update_submit(void) {
    update_button(&submitButton);
    update_form(&submitForm);
    if (mouseState.buttons && submitButton.hovered) {
        if (strlen(submitForm.input) == 0) {
            submitForm.default_color = al_map_rgb(225, 0, 0);
            return;
        }
        insert_to_leaderboard(submitForm.input, points_accumulated);
        change_scene(create_leaderboard_scene());

        // Re:zero
        map_number = 0;
        points_accumulated = 0;
        al_rest(0.20);
        return;
    }

}

static void destroy_submit(void) {
    al_destroy_bitmap(winning_panda);
    destroy_button(&submitButton);
    destroy_form(&submitForm);
}


Scene create_submit_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "submit";
    scene.init = &init_submit;
    scene.draw = &draw_submit;
    scene.update = &update_submit;
    scene.destroy = &destroy_submit;

    return scene;
}

Button nextButton;
Button prevButton;

int start;
int perScene = 5;
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

    int mid_rect_x = 135 + ((SCREEN_W - 135 - 135) / 2);
    int mid_rect_y = 160 + ((SCREEN_H - 185 - 160) / 2);
    prevButton = button_create(60 - (TILE_SIZE / 2), mid_rect_y - (TILE_SIZE / 2),
        TILE_SIZE, TILE_SIZE,
        "Assets/button3_flip.png", "Assets/button3_flip.png");

    nextButton = button_create(SCREEN_H - 60 - (TILE_SIZE / 2), mid_rect_y - (TILE_SIZE / 2),
        TILE_SIZE, TILE_SIZE,
        "Assets/button3.png", "Assets/button3.png");

    start = 0;

    init_bg();
}

void update(void) {
    update_button(&backButton);

    if (backButton.hovered && mouseState.buttons) {
        change_scene(create_menu_scene());
    }

    update_button(&nextButton);
    if (nextButton.hovered && mouseState.buttons) {
        start = (start + perScene >= size) ? 0 : start + perScene;
        al_rest(0.3);
    }
    update_button(&prevButton);
    if (prevButton.hovered && mouseState.buttons) {
        if (start - perScene < 0) {
            if (size % 5 == 0) {
                start = ((size / 5) - 1) * 5;
            }
            else {
                start = (size / 5) * 5;
            }
        }
        else {
            start = start - perScene;
        }
        al_rest(0.3);
    }

    update_bg();
}

void draw(void) {
    draw_bg();
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

    draw_button(nextButton);
    draw_button(prevButton);

    
    for (int i = 0; i < perScene; i++) {
        if (start + i >= size) {
            break;
        }
        char number[4];
        snprintf(number, sizeof(number), "%d", (start + i + 1));
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
            leaderboards[start + i].name
        );

        char point[6];
        snprintf(point, sizeof(point), "%d", leaderboards[start + i].point);

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
    destroy_button(&nextButton);
    destroy_button(&prevButton);

    al_destroy_bitmap(trophy_bitmap);
    al_destroy_bitmap(leaderboard_bitmap);

    destroy_bg();
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

