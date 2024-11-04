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

Button backButton;
Button shopButton;

static void init(void) {
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

    fiona = al_load_bitmap("Assets/pixilart-sprite-2.png");
    if (!fiona) {
        game_abort("Failed to load fiona.png");
    }

    backButton = button_create(
        SCREEN_W / 2 - 50 - 240,
        SCREEN_H - 120 - 40,
        240, 120,
        "Assets/UI_Button.png",
        "Assets/UI_Button_hovered.png");
    shopButton = button_create(
        SCREEN_W / 2 + 50,
        SCREEN_H - 120 - 40,
        240, 120,
        "Assets/UI_Button.png",
        "Assets/UI_Button_hovered.png");

}

static void update(void) {
    update_button(&backButton);
    update_button(&shopButton);
}

static void draw(void) {
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

    draw_button(shopButton);
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        shopButton.x + (shopButton.w / 2),
        (shopButton.y + 7) + 28 + shopButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "SHOP"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        shopButton.x + (shopButton.w / 2),
        (shopButton.y + 7) + 31 + shopButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "SHOP"
    );



}

static void destroy(void) {
    al_destroy_bitmap(smg);
    al_destroy_bitmap(sniper);
    al_destroy_bitmap(fiona);
    al_destroy_bitmap(panda);

    destroy_button(&backButton);
    destroy_button(&shopButton);
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
