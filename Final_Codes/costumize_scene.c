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
#include "background.h"

// Page 1
ALLEGRO_BITMAP* sniper;
ALLEGRO_BITMAP* smg;
ALLEGRO_BITMAP* panda;
ALLEGRO_BITMAP* fiona;

// Page 2
ALLEGRO_BITMAP* batman;
ALLEGRO_BITMAP* prof;
ALLEGRO_BITMAP* batarang;

ALLEGRO_BITMAP* coin;

Button nextButton;
Button backButton;
Button nextpageButton;
Button prevpageButton;

Button sniperButton;
Button smgButton;
Button pandaButton;
Button fionaButton;

Button batmanButton;
Button profButton;
Button batarangButton;

int item_size;

int item1x;
int item2x;

int item1y;
int item2y;

static int panda_price = 0;
static int fiona_price = 1;
static int batman_price = 20;
static int prof_price = 15;

static int smg_price = 0;
static int sniper_price = 2;
static int batarang_price = 20;

static int page;

static void init(void) {
    // Start at page 0
    page = 0;

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

    batarang = al_load_bitmap("Assets/batarang1.png");
    if (!batarang) {
        game_abort("Failed to load batarang.png");
    }

    panda = al_load_bitmap("Assets/panda2.png");
    if (!panda) {
        game_abort("Failed to load panda2.png");
    }

    fiona = al_load_bitmap("Assets/player.png");
    if (!fiona) {
        game_abort("Failed to load fiona.png");
    }

    batman = al_load_bitmap("Assets/aurick.png");
    if (!batman) {
        game_abort("Failed to load batman.png");
    }

    prof = al_load_bitmap("Assets/prof.png");
    if (!prof) {
        game_abort("Failed to load prof.png");
    }

    coin = al_load_bitmap("Assets/coin_icon.png");
    if (!coin) {
        game_abort("Failed to load coin_icon.png");
    }

    ALLEGRO_COLOR color = jurassic ? al_map_rgb(210, 134, 104) : al_map_rgb(255, 255, 255);

    backButton = button_create(
        SCREEN_W / 2 - 120 - 120 + 3,
        SCREEN_H - 120 - 10,
        200, 100,
        color,
        "Assets/UI_Button.png",
        "Assets/UI_Button_hovered.png");

    nextButton = button_create(
        SCREEN_W / 2 - 120 + 120 + 29,
        SCREEN_H - 120 - 10,
        200, 100,
        color,
        "Assets/UI_Button.png",
        "Assets/UI_Button_hovered.png");

    int mid_rect_x = 135 + ((SCREEN_W - 135 - 135) / 2);
    int mid_rect_y = 160 + ((SCREEN_H - 185 - 160) / 2);
    prevpageButton = button_create(60 - (TILE_SIZE / 2), mid_rect_y - (TILE_SIZE / 2),
        TILE_SIZE, TILE_SIZE,
        al_map_rgb(255, 255, 255),
        "Assets/button3_flip.png", "Assets/button3_flip.png");

    nextpageButton = button_create(SCREEN_H - 60 - (TILE_SIZE / 2), mid_rect_y - (TILE_SIZE / 2),
        TILE_SIZE, TILE_SIZE,
        al_map_rgb(255, 255, 255),
        "Assets/button3.png", "Assets/button3.png");

    int button_w = 140;
    int button_h = 70;

    int button_x = (item_size - button_w) / 2;
    int button_y = item_size + 30;


    pandaButton = button_create(
        item1x + button_x,
        item1y + button_y,
        button_w, button_h,
        color,
        "Assets/UI_Button.png",
        "Assets/UI_Button_hovered.png");

    fionaButton = button_create(
        item2x + button_x,
        item1y + button_y,
        button_w, button_h,
        color,
        "Assets/UI_Button.png",
        "Assets/UI_Button_hovered.png");

    smgButton = button_create(
        item1x + button_x,
        item2y + button_y - 10,
        button_w, button_h,
        color,
        "Assets/UI_Button.png",
        "Assets/UI_Button_hovered.png");

    sniperButton = button_create(
        item2x + button_x,
        item2y + button_y - 10,
        button_w, button_h,
        color,
        "Assets/UI_Button.png",
        "Assets/UI_Button_hovered.png");

    // Next page
    batmanButton = button_create(
        item1x + button_x,
        item1y + button_y,
        button_w, button_h,
        color,
        "Assets/UI_Button.png",
        "Assets/UI_Button_hovered.png");

    profButton = button_create(
        item2x + button_x,
        item1y + button_y,
        button_w, button_h,
        color,
        "Assets/UI_Button.png",
        "Assets/UI_Button_hovered.png");

    batarangButton = button_create(
        item1x + button_x,
        item2y + button_y - 10,
        button_w, button_h,
        color,
        "Assets/UI_Button.png",
        "Assets/UI_Button_hovered.png");

    init_bg();
    if (!gameDone) {
        change_bgm("Assets/audio/throne_room.mp3");
    }    
    
}

static void update(void) {
    update_button(&backButton);
    if (mouseState.buttons && backButton.hovered) {
        al_play_sample(buttonSound, SFX_VOLUME, 0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        change_scene(create_menu_scene());
        al_rest(0.4);
    }

    update_button(&nextButton);
    if (mouseState.buttons && nextButton.hovered) {
        al_play_sample(buttonSound, SFX_VOLUME, 0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        change_scene(create_loading_scene());
        al_rest(0.4);
    }

    if (page == 0) {
        update_button(&nextpageButton);
        if (mouseState.buttons && nextpageButton.hovered) {
            page = 1;
            al_rest(0.2);
        }

        update_button(&pandaButton);
        if (panda_price == 0) {
            pandaButton.color = al_map_rgb(202, 250, 202);
        }
        if (mouseState.buttons && pandaButton.hovered) {
            if (total_coins >= panda_price) {
                if (player_type != PANDA) {
                    al_play_sample(buttonSound, SFX_VOLUME, 0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                }
                player_type = PANDA;
                total_coins -= panda_price;
                panda_price = 0;
            }

        }
        update_button(&fionaButton);
        if (fiona_price == 0) {
            fionaButton.color = al_map_rgb(202, 250, 202);
        }
        if (mouseState.buttons && fionaButton.hovered) {
            if (total_coins >= fiona_price) {
                if (player_type != FIONA) {
                    al_play_sample(buttonSound, SFX_VOLUME, 0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                }
                player_type = FIONA;
                total_coins -= fiona_price;
                fiona_price = 0;
            }
        }
        update_button(&smgButton);
        if (smg_price == 0) {
            smgButton.color = al_map_rgb(202, 250, 202);
        }
        if (mouseState.buttons && smgButton.hovered) {
            if (total_coins >= smg_price) {
                if (player_weapon != SMG) {
                    al_play_sample(buttonSound, SFX_VOLUME, 0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                }
                player_weapon = SMG;
                total_coins -= smg_price;
                smg_price = 0;
            }
        }
        update_button(&sniperButton);
        if (sniper_price == 0) {
            sniperButton.color = al_map_rgb(202, 250, 202);
        }
        if (mouseState.buttons && sniperButton.hovered) {
            if (total_coins >= sniper_price) {
                if (player_weapon != SNIPER) {
                    al_play_sample(buttonSound, SFX_VOLUME, 0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                }
                player_weapon = SNIPER;
                total_coins -= sniper_price;
                sniper_price = 0;
            }
        }
    }
   
    if (page == 1) {
        update_button(&prevpageButton);
        if (mouseState.buttons && prevpageButton.hovered) {
            page = 0;
            al_rest(0.2);
        }

        // Next page
        update_button(&batmanButton);
        if (batman_price == 0) {
            batmanButton.color = al_map_rgb(202, 250, 202);
        }
        if (mouseState.buttons && batmanButton.hovered) {
            if (total_coins >= batman_price && map_number >= 2) {
                if (player_type != TAP) {
                    al_play_sample(buttonSound, SFX_VOLUME, 0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                }
                player_type = TAP;
                total_coins -= batman_price;
                batman_price = 0;
            }
        }

        update_button(&profButton);
        if (prof_price == 0) {
            profButton.color = al_map_rgb(202, 250, 202);
        }
        if (mouseState.buttons && profButton.hovered) {
            if (total_coins >= prof_price && map_number >= 1) {
                if (player_type != PROFP) {
                    al_play_sample(buttonSound, SFX_VOLUME, 0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                }
                player_type = PROFP;
                total_coins -= prof_price;
                prof_price = 0;
            }
        }

        update_button(&batarangButton);
        if (batarang_price == 0) {
            batarangButton.color = al_map_rgb(202, 250, 202);
        }
        if (mouseState.buttons && batarangButton.hovered) {
            if (total_coins >= batarang_price && map_number >= 2) {
                if (player_weapon != BATARANG) {
                    al_play_sample(buttonSound, SFX_VOLUME, 0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                }
                player_weapon = BATARANG;
                total_coins -= batarang_price;
                batarang_price = 0;
            }
        }
    }
    

    update_bg();
}

static void draw(void) {
    draw_bg();

    if (page == 0) {
        draw_button(nextpageButton);
    }

    if (page == 1) {
        draw_button(prevpageButton);
    }

    int startx = 120;
    int starty = 102;

    ALLEGRO_COLOR color_back = jurassic ? al_map_rgb(20, 20, 20) : al_map_rgb(0, 109, 191);
    ALLEGRO_COLOR color_front = jurassic ? al_map_rgb(120, 85, 75) : al_map_rgb(46, 146, 255);

    al_draw_filled_rounded_rectangle(
        startx, starty,
        SCREEN_W - startx, backButton.y - 25,
        25, 25,
        color_back);
    al_draw_filled_rounded_rectangle(
        startx + 10, starty + 10,
        SCREEN_W - (startx + 10), backButton.y - 25 - 10,
        25, 25,
        color_front);

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

    if (page == 0) {
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
   
    if (page == 1) {
        // Next Page
        if (player_type != TAP) {
            draw_button(batmanButton);

            if (batman_price != 0) {
                al_draw_scaled_bitmap(coin,
                    0, 0, 16, 16,
                    batmanButton.x + (batmanButton.w / 6),
                    batmanButton.y + (batmanButton.h / 10) + (batmanButton.hovered * 7),
                    48, 48,
                    0);

                char batman_str[5];
                snprintf(batman_str, sizeof(batman_str), "%02d", batman_price);
                if (map_number < 2) strcpy_s(batman_str, sizeof(batman_str), "?");

                // Coin string
                al_draw_text(
                    P3_FONT,
                    al_map_rgb(255, 255, 255),
                    batmanButton.x + 50 + (batmanButton.w / 6),
                    batmanButton.y + 15 + (batmanButton.h / 10) + (batmanButton.hovered * 7),
                    ALLEGRO_ALIGN_LEFT,
                    batman_str
                );
            }
            else {
                al_draw_text(
                    P3_FONT,
                    al_map_rgb(66, 76, 110),
                    batmanButton.x + (batmanButton.w / 2),
                    batmanButton.y + 12 + (batmanButton.h / 10) + (batmanButton.hovered * 7),
                    ALLEGRO_ALIGN_CENTER,
                    "Change"
                );
                al_draw_text(
                    P3_FONT,
                    al_map_rgb(255, 255, 255),
                    batmanButton.x + (batmanButton.w / 2),
                    batmanButton.y + 15 + (batmanButton.h / 10) + (batmanButton.hovered * 7),
                    ALLEGRO_ALIGN_CENTER,
                    "Change"
                );
            }

        }
        else {
            al_draw_text(P2_FONT, al_map_rgb(255, 255, 255),
                batmanButton.x + (batmanButton.w / 2), batmanButton.y + (batmanButton.h / 4), ALLEGRO_ALIGN_CENTER,
                using_message);

        }

        if (player_type != PROFP) {
            draw_button(profButton);

            if (prof_price != 0) {
                al_draw_scaled_bitmap(coin,
                    0, 0, 16, 16,
                    profButton.x + (profButton.w / 6),
                    profButton.y + (profButton.h / 10) + (profButton.hovered * 7),
                    48, 48,
                    0);

                char prof_str[5];
                snprintf(prof_str, sizeof(prof_str), "%02d", prof_price);
                if (map_number < 1) strcpy_s(prof_str, sizeof(prof_str), "?");

                // Coin string
                al_draw_text(
                    P3_FONT,
                    al_map_rgb(255, 255, 255),
                    profButton.x + 50 + (profButton.w / 6),
                    profButton.y + 15 + (profButton.h / 10) + (profButton.hovered * 7),
                    ALLEGRO_ALIGN_LEFT,
                    prof_str
                );
            }
            else {
                al_draw_text(
                    P3_FONT,
                    al_map_rgb(66, 76, 110),
                    profButton.x + (profButton.w / 2),
                    profButton.y + 12 + (profButton.h / 10) + (profButton.hovered * 7),
                    ALLEGRO_ALIGN_CENTER,
                    "Change"
                );
                al_draw_text(
                    P3_FONT,
                    al_map_rgb(255, 255, 255),
                    profButton.x + (profButton.w / 2),
                    profButton.y + 15 + (profButton.h / 10) + (profButton.hovered * 7),
                    ALLEGRO_ALIGN_CENTER,
                    "Change"
                );
            }

        }
        else {
            al_draw_text(P2_FONT, al_map_rgb(255, 255, 255),
                profButton.x + (profButton.w / 2), profButton.y + (profButton.h / 4), ALLEGRO_ALIGN_CENTER,
                using_message);

        }

        if (player_weapon != BATARANG) {
            draw_button(batarangButton);

            if (batarang_price != 0) {
                al_draw_scaled_bitmap(coin,
                    0, 0, 16, 16,
                    batarangButton.x + (batarangButton.w / 6),
                    batarangButton.y + (batarangButton.h / 10) + (batarangButton.hovered * 7),
                    48, 48,
                    0);

                char batarang_str[5];
                snprintf(batarang_str, sizeof(batarang_str), "%02d", batarang_price);
                if (map_number < 2) strcpy_s(batarang_str, sizeof(batarang_str), "?");

                // Coin string
                al_draw_text(
                    P3_FONT,
                    al_map_rgb(255, 255, 255),
                    batarangButton.x + 50 + (batarangButton.w / 6),
                    batarangButton.y + 15 + (batarangButton.h / 10) + (batarangButton.hovered * 7),
                    ALLEGRO_ALIGN_LEFT,
                    batarang_str
                );
            }
            else {
                al_draw_text(
                    P3_FONT,
                    al_map_rgb(66, 76, 110),
                    batarangButton.x + (batarangButton.w / 2),
                    batarangButton.y + 12 + (batarangButton.h / 10) + (batarangButton.hovered * 7),
                    ALLEGRO_ALIGN_CENTER,
                    "Change"
                );
                al_draw_text(
                    P3_FONT,
                    al_map_rgb(255, 255, 255),
                    batarangButton.x + (batarangButton.w / 2),
                    batarangButton.y + 15 + (batarangButton.h / 10) + (batarangButton.hovered * 7),
                    ALLEGRO_ALIGN_CENTER,
                    "Change"
                );
            }

        }
        else {
            al_draw_text(P2_FONT, al_map_rgb(255, 255, 255),
                batarangButton.x + (batarangButton.w / 2), batarangButton.y + (batarangButton.h / 4), ALLEGRO_ALIGN_CENTER,
                using_message);
        }

        ALLEGRO_COLOR hidden_color_map2 = (map_number >= 1) ? al_map_rgb(255, 255, 255) : al_map_rgb(0, 0, 0);
        ALLEGRO_COLOR hidden_color_map3 = (map_number >= 2) ? al_map_rgb(255, 255, 255) : al_map_rgb(0, 0, 0);

        al_draw_tinted_scaled_bitmap(batman, hidden_color_map3,
            0, 0, 424, 588,
            item1x, item1y, item_size, item_size,
            1);

        al_draw_tinted_scaled_bitmap(prof, hidden_color_map2,
            0, 0, 350, 350,
            item2x, item1y, item_size, item_size,
            0);

        al_draw_tinted_scaled_bitmap(batarang, hidden_color_map3,
            0, 0, 32, 32,
            item1x, item2y, item_size, item_size,
            0);
    }
   
}

static void destroy(void) {
    al_destroy_bitmap(coin);

    al_destroy_bitmap(smg);
    al_destroy_bitmap(sniper);
    al_destroy_bitmap(fiona);
    al_destroy_bitmap(panda);
    al_destroy_bitmap(batman);
    al_destroy_bitmap(prof);
    al_destroy_bitmap(batarang);

    destroy_button(&nextButton);
    destroy_button(&backButton);
    destroy_button(&nextpageButton);
    destroy_button(&prevpageButton);
    destroy_button(&pandaButton);
    destroy_button(&fionaButton);
    destroy_button(&smgButton);
    destroy_button(&sniperButton);
    destroy_button(&batmanButton);
    destroy_button(&profButton);
    destroy_button(&batarangButton);

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
