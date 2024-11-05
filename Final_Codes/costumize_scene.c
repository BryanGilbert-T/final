#include <allegro5/allegro.h>
#include <string.h>
#include "menu_scene.h"
#include "costumize_scene.h"
#include "game_scene.h"
#include "setting_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"

ALLEGRO_BITMAP* sniper;
ALLEGRO_BITMAP* smg;
ALLEGRO_BITMAP* panda;
ALLEGRO_BITMAP* fiona;

ALLEGRO_BITMAP* coin;

Button backButton;
Button sniperButton;
Button smgButton;
Button pandaButton;
Button fionaButton;

int item_size;

int item1x;
int item2x;

int item1y;
int item2y;

static void init(void) {
    item_size = 140;

    item1x = SCREEN_W / 2 - 140 - 60;
    item2x = SCREEN_W / 2 + 60;

    item1y = 140;
    item2y = item1y + item_size + 100;

    sniper = al_load_bitmap("Assets/sniper.png");
    if (!sniper) {
        game_abort("Failed to load sniper.png");
    }

    smg = al_load_bitmap("Assets/guns.png");
    if (!smg) {
        game_abort("Failed to load guns.png");
    }

    panda = al_load_bitmap("Assets/panda2.png");
    if (!panda) {
        game_abort("Failed to load panda2.png");
    }

    fiona = al_load_bitmap("Assets/player.png");
    if (!fiona) {
        game_abort("Failed to load fiona.png");
    }

    coin = al_load_bitmap("Assets/coin_icon.png");
    if (!coin) {
        game_abort("Failed to load coin_icon.png");
    }


    backButton = button_create(
        SCREEN_W / 2 - 120,
        SCREEN_H - 120 - 40,
        240, 120,
        "Assets/UI_Button.png",
        "Assets/UI_Button_hovered.png");

    int button_w = 140;
    int button_h = 70;

    int button_x = (item_size - button_w) / 2;
    int button_y = item_size + 30;


    pandaButton = button_create(
        item1x + button_x,
        item1y + button_y,
        button_w, button_h,
        "Assets/UI_Button.png",
        "Assets/UI_Button_hovered.png");

    fionaButton = button_create(
        item2x + button_x,
        item1y + button_y,
        button_w, button_h,
        "Assets/UI_Button.png",
        "Assets/UI_Button_hovered.png");

    smgButton = button_create(
        item1x + button_x,
        item2y + button_y - 10,
        button_w, button_h,
        "Assets/UI_Button.png",
        "Assets/UI_Button_hovered.png");

    sniperButton = button_create(
        item2x + button_x,
        item2y + button_y - 10,
        button_w, button_h,
        "Assets/UI_Button.png",
        "Assets/UI_Button_hovered.png");

}

static void update(void) {
    update_button(&backButton);
    if (mouseState.buttons && backButton.hovered) {
        change_scene(create_menu_scene());
        al_rest(0.4);
    }

    update_button(&pandaButton);
    if (mouseState.buttons && pandaButton.hovered) {
        player_type = PANDA;
    }
    update_button(&fionaButton);
    if (mouseState.buttons && fionaButton.hovered) {
        player_type = FIONA;
    }
    update_button(&smgButton);
    if (mouseState.buttons && smgButton.hovered) {
        player_weapon = SMG;
    }
    update_button(&sniperButton);
    if (mouseState.buttons && sniperButton.hovered) {
        player_weapon = SNIPER;
    }
}

static void draw(void) {
    char coinstr[5];
    int coin_x = 20;
    int coin_y = 30;
    snprintf(coinstr, sizeof(coinstr), "%02d", total_coins);
    al_draw_text(P2_FONT, al_map_rgb(255, 255, 255),
        coin_x + 67, coin_y + 17, ALLEGRO_ALIGN_LEFT,
        coinstr);

    al_draw_scaled_bitmap(coin,
        0, 0, 16, 16,
        coin_x, coin_y, 64, 64,
        0);

    draw_button(backButton);
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        backButton.x + (backButton.w / 2),
        (backButton.y + 7) + 28 + backButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "BACK"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        backButton.x + (backButton.w / 2),
        (backButton.y + 7) + 31 + backButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "BACK"
    );

    char* using_message = "Used";

    if (player_type != PANDA) {
        draw_button(pandaButton);
    }
    else {
        al_draw_text(P2_FONT, al_map_rgb(255, 255, 255),
            pandaButton.x + (pandaButton.w / 2), pandaButton.y + (pandaButton.h / 4), ALLEGRO_ALIGN_CENTER,
            using_message);
    }

    if (player_type != FIONA) {
        draw_button(fionaButton);
    }
    else {
        al_draw_text(P2_FONT, al_map_rgb(255, 255, 255),
            fionaButton.x + (fionaButton.w / 2), fionaButton.y + (fionaButton.h / 4), ALLEGRO_ALIGN_CENTER,
            using_message);
    }

    if (player_weapon != SMG) {
        draw_button(smgButton);
    }
    else {
        al_draw_text(P2_FONT, al_map_rgb(255, 255, 255),
            smgButton.x + (smgButton.w / 2), smgButton.y + (smgButton.h / 4), ALLEGRO_ALIGN_CENTER,
            using_message);
    }

    if (player_weapon != SNIPER) {
        draw_button(sniperButton);
    }
    else {
        al_draw_text(P2_FONT, al_map_rgb(255, 255, 255),
            sniperButton.x + (sniperButton.w / 2), sniperButton.y + (sniperButton.h / 4), ALLEGRO_ALIGN_CENTER,
            using_message);
    }

    al_draw_tinted_scaled_bitmap(panda, al_map_rgb(255, 255, 255),
        0, 0, 32, 32,
        item1x, item1y, item_size, item_size,
        1);

    al_draw_tinted_scaled_bitmap(fiona, al_map_rgb(255, 255, 255),
        0, 0, 48, 48,
        item2x, item1y, item_size, item_size,
        0);

    al_draw_tinted_scaled_bitmap(smg, al_map_rgb(255, 255, 255),
        0, 0, 32, 32,
        item1x, item2y, item_size, item_size,
        0);

    al_draw_tinted_scaled_bitmap(sniper, al_map_rgb(255, 255, 255),
        0, 0, 32, 32,
        item2x, item2y, item_size, item_size,
        0);


}

static void destroy(void) {
    al_destroy_bitmap(coin);

    al_destroy_bitmap(smg);
    al_destroy_bitmap(sniper);
    al_destroy_bitmap(fiona);
    al_destroy_bitmap(panda);

    destroy_button(&backButton);
    destroy_button(&pandaButton);
    destroy_button(&fionaButton);
    destroy_button(&smgButton);
    destroy_button(&sniperButton);
}


Scene create_costumize_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "costumize";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}
