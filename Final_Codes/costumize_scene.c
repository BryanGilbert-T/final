#include <allegro5/allegro.h>
#include <string.h>
#include "menu_scene.h"
#include "costumize_scene.h"
#include "game_scene.h"
#include "setting_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"
#include "loading_scene.h"

ALLEGRO_BITMAP* sniper;
ALLEGRO_BITMAP* smg;
ALLEGRO_BITMAP* panda;
ALLEGRO_BITMAP* fiona;

ALLEGRO_BITMAP* coin;

Button nextButton;
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

int panda_price = 0;
int fiona_price = 1;

int smg_price = 0;
int sniper_price = 2;

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
        SCREEN_W / 2 - 120 - 120 + 3,
        SCREEN_H - 120 - 10,
        200, 100,
        "Assets/UI_Button.png",
        "Assets/UI_Button_hovered.png");

    nextButton = button_create(
        SCREEN_W / 2 - 120 + 120 + 29,
        SCREEN_H - 120 - 10,
        200, 100,
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

    init_bg();

}

static void update(void) {
    update_button(&backButton);
    if (mouseState.buttons && backButton.hovered) {
        change_scene(create_menu_scene());
        al_rest(0.4);
    }

    update_button(&nextButton);
    if (mouseState.buttons && nextButton.hovered) {
        change_scene(create_loading_scene());
        al_rest(0.4);
    }

    update_button(&pandaButton);
    if (mouseState.buttons && pandaButton.hovered) {
        if (total_coins >= panda_price) {
            player_type = PANDA;
            total_coins -= panda_price;
            panda_price = 0;
        }
        
    }
    update_button(&fionaButton);
    if (mouseState.buttons && fionaButton.hovered) {
        if (total_coins >= fiona_price) {
            player_type = FIONA;
            total_coins -= fiona_price;
            fiona_price = 0;
        }
    }
    update_button(&smgButton);
    if (mouseState.buttons && smgButton.hovered) {
        if (total_coins >= smg_price) {
            player_weapon = SMG;
            total_coins -= smg_price;
            smg_price = 0;
        }
    }
    update_button(&sniperButton);
    if (mouseState.buttons && sniperButton.hovered) {
        if (total_coins >= sniper_price) {
            player_weapon = SNIPER;
            total_coins -= sniper_price;
            sniper_price = 0;
        }
    }

    update_bg();
}

static void draw(void) {
    draw_bg();
    int startx = 120;
    int starty = 102;
    al_draw_filled_rounded_rectangle(
        startx, starty,
        SCREEN_W - startx, backButton.y - 25,
        25, 25,
        al_map_rgb(0, 109, 191));
    al_draw_filled_rounded_rectangle(
        startx + 10, starty + 10,
        SCREEN_W - (startx + 10), backButton.y - 25 - 10,
        25, 25,
        al_map_rgb(46, 146, 255));

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
        (backButton.y + 7) + 20 + backButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "BACK"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        backButton.x + (backButton.w / 2),
        (backButton.y + 7) + 23 + backButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "BACK"
    );

    draw_button(nextButton);
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        nextButton.x + (nextButton.w / 2),
        (nextButton.y + 7) + 20 + nextButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "PLAY"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        nextButton.x + (nextButton.w / 2),
        (nextButton.y + 7) + 23 + nextButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "PLAY"
    );

    char* using_message = "Used";

    if (player_type != PANDA) {
        draw_button(pandaButton);

        if (panda_price != 0) {
            al_draw_scaled_bitmap(coin,
                0, 0, 16, 16,
                pandaButton.x + (pandaButton.w / 6),
                pandaButton.y + (pandaButton.h / 10) + (pandaButton.hovered * 7),
                48, 48,
                0);

            char panda_str[5];
            snprintf(panda_str, sizeof(panda_str), "%02d", panda_price);
            // Coin string
            al_draw_text(
                P3_FONT,
                al_map_rgb(255, 255, 255),
                pandaButton.x + 50 + (pandaButton.w / 6),
                pandaButton.y + 15 + (pandaButton.h / 10) + (pandaButton.hovered * 7),
                ALLEGRO_ALIGN_LEFT,
                panda_str
            );
        }
        else {
            al_draw_text(
                P3_FONT,
                al_map_rgb(66, 76, 110),
                pandaButton.x + (pandaButton.w / 2),
                pandaButton.y + 12 + (pandaButton.h / 10) + (pandaButton.hovered * 7),
                ALLEGRO_ALIGN_CENTER,
                "Change"
            );
            al_draw_text(
                P3_FONT,
                al_map_rgb(255, 255, 255),
                pandaButton.x + (pandaButton.w / 2),
                pandaButton.y + 15 + (pandaButton.h / 10) + (pandaButton.hovered * 7),
                ALLEGRO_ALIGN_CENTER,
                "Change"
            );
        }

    }
    else {
        al_draw_text(P2_FONT, al_map_rgb(255, 255, 255),
            pandaButton.x + (pandaButton.w / 2), pandaButton.y + (pandaButton.h / 4), ALLEGRO_ALIGN_CENTER,
            using_message);

    }

    if (player_type != FIONA) {
        draw_button(fionaButton);

        if (fiona_price != 0) {
            al_draw_scaled_bitmap(coin,
                0, 0, 16, 16,
                fionaButton.x + (fionaButton.w / 6),
                fionaButton.y + (fionaButton.h / 10) + (fionaButton.hovered * 7),
                48, 48,
                0);

            char fiona_str[5];
            snprintf(fiona_str, sizeof(fiona_str), "%02d", fiona_price);
            // Coin string
            al_draw_text(
                P3_FONT,
                al_map_rgb(255, 255, 255),
                fionaButton.x + 50 + (fionaButton.w / 6),
                fionaButton.y + 15 + (fionaButton.h / 10) + (fionaButton.hovered * 7),
                ALLEGRO_ALIGN_LEFT,
                fiona_str
            );
        }
        else {
            al_draw_text(
                P3_FONT,
                al_map_rgb(66, 76, 110),
                fionaButton.x + (fionaButton.w / 2),
                fionaButton.y + 12 + (fionaButton.h / 10) + (fionaButton.hovered * 7),
                ALLEGRO_ALIGN_CENTER,
                "Change"
            );
            al_draw_text(
                P3_FONT,
                al_map_rgb(255, 255, 255),
                fionaButton.x + (fionaButton.w / 2),
                fionaButton.y + 15 + (fionaButton.h / 10) + (fionaButton.hovered * 7),
                ALLEGRO_ALIGN_CENTER,
                "Change"
            );
        }
    }
    else {
        al_draw_text(P2_FONT, al_map_rgb(255, 255, 255),
            fionaButton.x + (fionaButton.w / 2), fionaButton.y + (fionaButton.h / 4), ALLEGRO_ALIGN_CENTER,
            using_message);
    }

    if (player_weapon != SMG) {
        draw_button(smgButton);
        if (smg_price != 0) {
            al_draw_scaled_bitmap(coin,
                0, 0, 16, 16,
                smgButton.x + (smgButton.w / 6),
                smgButton.y + (smgButton.h / 10) + (smgButton.hovered * 7),
                48, 48,
                0);

            char smg_str[5];
            snprintf(smg_str, sizeof(smg_str), "%02d", smg_price);
            // Coin string
            al_draw_text(
                P3_FONT,
                al_map_rgb(255, 255, 255),
                smgButton.x + 50 + (smgButton.w / 6),
                smgButton.y + 15 + (smgButton.h / 10) + (smgButton.hovered * 7),
                ALLEGRO_ALIGN_LEFT,
                smg_str
            );
        }
        else {
            al_draw_text(
                P3_FONT,
                al_map_rgb(66, 76, 110),
                smgButton.x + (smgButton.w / 2),
                smgButton.y + 12 + (smgButton.h / 10) + (smgButton.hovered * 7),
                ALLEGRO_ALIGN_CENTER,
                "Change"
            );
            al_draw_text(
                P3_FONT,
                al_map_rgb(255, 255, 255),
                smgButton.x + (smgButton.w / 2),
                smgButton.y + 15 + (smgButton.h / 10) + (smgButton.hovered * 7),
                ALLEGRO_ALIGN_CENTER,
                "Change"
            );
        }
    }
    else {
        al_draw_text(P2_FONT, al_map_rgb(255, 255, 255),
            smgButton.x + (smgButton.w / 2), smgButton.y + (smgButton.h / 4), ALLEGRO_ALIGN_CENTER,
            using_message);
    }

    if (player_weapon != SNIPER) {
        draw_button(sniperButton);
        if (sniper_price != 0) {
            al_draw_scaled_bitmap(coin,
                0, 0, 16, 16,
                sniperButton.x + (sniperButton.w / 6),
                sniperButton.y + (sniperButton.h / 10) + (sniperButton.hovered * 7),
                48, 48,
                0);

            char sniper_str[5];
            snprintf(sniper_str, sizeof(sniper_str), "%02d", sniper_price);
            // Coin string
            al_draw_text(
                P3_FONT,
                al_map_rgb(255, 255, 255),
                sniperButton.x + 50 + (sniperButton.w / 6),
                sniperButton.y + 15 + (sniperButton.h / 10) + (sniperButton.hovered * 7),
                ALLEGRO_ALIGN_LEFT,
                sniper_str
            );
        }
        else {
            al_draw_text(
                P3_FONT,
                al_map_rgb(66, 76, 110),
                sniperButton.x + (sniperButton.w / 2),
                sniperButton.y + 12 + (sniperButton.h / 10) + (sniperButton.hovered * 7),
                ALLEGRO_ALIGN_CENTER,
                "Change"
            );
            al_draw_text(
                P3_FONT,
                al_map_rgb(255, 255, 255),
                sniperButton.x + (sniperButton.w / 2),
                sniperButton.y + 15 + (sniperButton.h / 10) + (sniperButton.hovered * 7),
                ALLEGRO_ALIGN_CENTER,
                "Change"
            );
        }
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

    destroy_button(&nextButton);
    destroy_button(&backButton);
    destroy_button(&pandaButton);
    destroy_button(&fionaButton);
    destroy_button(&smgButton);
    destroy_button(&sniperButton);

    destroy_bg();
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
