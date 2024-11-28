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

static Button settingButton;
static Button playButton;

static void init(void) {
    settingButton = button_create(SCREEN_W / 2 - 200, 600, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    playButton = button_create(SCREEN_W / 2 - 200, 500, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");

    change_bgm("Assets/audio/menu_bgm.mp3");
}

static void update(void) {
    update_button(&settingButton);
    update_button(&playButton);
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

}

static void draw(void) {
    // Title Text
    al_draw_text(
        TITLE_FONT,
        al_map_rgb(146, 161, 185),
        SCREEN_W / 2,
        225,
        ALLEGRO_ALIGN_CENTER,
        "NTHU-RPG ADVENTURE"
    );
    al_draw_text(
        TITLE_FONT,
        al_map_rgb(199, 207, 221),
        SCREEN_W / 2,
        220,
        ALLEGRO_ALIGN_CENTER,
        "NTHU-RPG ADVENTURE"
    );

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
        600 + 28 + settingButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "SETTING"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        SCREEN_W / 2,
        600 + 31 + settingButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "SETTING"
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
