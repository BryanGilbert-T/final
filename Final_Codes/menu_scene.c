#include <allegro5/allegro.h>
#include <string.h>
#include "menu_scene.h"
#include "game_scene.h"
#include "setting_scene.h"
#include "loading_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"
#include "costumize_scene.h"
#include "leaderboard.h"
#include "background.h"

static Button settingButton;
static Button playButton;
static Button leaderboardButton;

static ALLEGRO_BITMAP* title;

static void init(void) {
    ALLEGRO_COLOR color = jurassic ? al_map_rgb(210, 134, 104) : al_map_rgb(255, 255, 255);

    settingButton = button_create(SCREEN_W / 2 - 200, 600, 400, 100,
        color,
        "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    playButton = button_create(SCREEN_W / 2 - 200, 400, 400, 100,
        color,
        "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    leaderboardButton = button_create(SCREEN_W / 2 - 200, 500, 400, 100,
        color,
        "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");

    char* title_path;
    if (jurassic) {
        title_path = "Assets/dune_title.png";
    }
    else {
        title_path = "Assets/star_title.png";
    }
    title = al_load_bitmap(title_path);
    if (!title) {
        game_abort("cant find title bitmap with path %s", title_path);
    }
    
    init_bg();

    if (jurassic) {
        change_bgm("");
    }
    else {
        change_bgm("Assets/audio/star_wars_theme.mp3");
    }

}

static void update(void) {
    update_button(&settingButton);
    update_button(&playButton);
    update_button(&leaderboardButton);
    if (keyState[ALLEGRO_KEY_ENTER]) {
        change_scene(create_loading_scene());
        return;
    }

    /*
        [TODO HACKATHON 4-3] 
        
        Change scene to setting scene when the button is pressed
    */
    Point mouse = { mouseState.x , mouseState.y };
    if (settingButton.hovered && mouseState.buttons) {
        change_scene(create_setting_scene());
        al_rest(0.4);
        return;
    }
    if (playButton.hovered && mouseState.buttons) {
        change_scene(create_costumize_scene());
        return;
    }
    if (leaderboardButton.hovered && mouseState.buttons) {
        change_scene(create_leaderboard_scene());
        return;
    }

    update_bg();
}

static void draw(void) {
    draw_bg();
    
    al_draw_scaled_bitmap(title,
        0, 0, 222, 108,
        SCREEN_W / 2 - (513 / 2), 120,
        513, 250,
        0);

    /* Changed to button
    al_draw_text(
        P2_FONT,
        al_map_rgb(255, 255, 255),
        SCREEN_W / 2,
        500,
        ALLEGRO_ALIGN_CENTER,
        "PRESS [ENTER] TO PLAY"
    );
    */

    // button
    draw_button(settingButton);
    // button text
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        SCREEN_W / 2,
        settingButton.y + 28 + settingButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "SETTING"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        SCREEN_W / 2,
        settingButton.y + 31 + settingButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "SETTING"
    );

    draw_button(leaderboardButton);
    // button text
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        SCREEN_W / 2,
        leaderboardButton.y + 28 + leaderboardButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "LEADERBOARD"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        SCREEN_W / 2,
        leaderboardButton.y + 31 + leaderboardButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "LEADERBOARD"
    );

    char playOrContinue[15];
    if (map_number == 0) {
        strcpy_s(playOrContinue, sizeof(playOrContinue), "PLAY");
    }
    else {
        strcpy_s(playOrContinue, sizeof(playOrContinue), "CONTINUE");
    }
    //Play Button
    draw_button(playButton);
    // button text
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        SCREEN_W / 2,
        playButton.y + 28 + playButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        playOrContinue
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        SCREEN_W / 2,
        playButton.y + 31 + playButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        playOrContinue
    );

   
}

static void destroy(void) {
    destroy_button(&settingButton);
    destroy_button(&playButton);
    destroy_button(&leaderboardButton);
    al_destroy_bitmap(title);
    destroy_bg();
}


Scene create_menu_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "menu";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}
