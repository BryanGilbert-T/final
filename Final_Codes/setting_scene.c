#include <allegro5/allegro.h>
#include "setting_scene.h"
#include "menu_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"

static Button backButton;

static Button bgmVolumeSlider;
static Button sfxVolumeSlider;

static Button bgmVolumeSlider2;
static Button sfxVolumeSlider2;

static Button bgmButton;
static Button sfxButton;


static void init(void) {
    backButton = button_create(SCREEN_W / 2 - 200, 650, 400, 100,
        al_map_rgb(255, 255, 255), 
        "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");

    //BGM
    bgmVolumeSlider = button_create(
        SCREEN_W / 2 - 265,
        100,
        600, 20,
        al_map_rgb(255, 255, 255),
        "Assets/slider.png", "Assets/slider.png");
    bgmButton = button_create(
        bgmVolumeSlider.x + ((bgmVolumeSlider.w - 140) * BGM_VOLUME),
        bgmVolumeSlider.y - (bgmVolumeSlider.h),
        140, 140/2,
        al_map_rgb(255, 255, 255),
        "Assets/UI_Button.png", "Assets/UI_Button.png");
    bgmVolumeSlider2 = button_create(
        bgmVolumeSlider.x,
        bgmVolumeSlider.y,
        bgmButton.x - bgmVolumeSlider.x, bgmVolumeSlider.h,
        al_map_rgb(255, 255, 255),
        "Assets/slider_hovered.png", "Assets/slider_hovered.png");

    //SFX
    sfxVolumeSlider = button_create(
        SCREEN_W / 2 - 265,
        200,
        600, 20,
        al_map_rgb(255, 255, 255),
        "Assets/slider.png", "Assets/slider.png");
    sfxButton = button_create(
        sfxVolumeSlider.x + ((sfxVolumeSlider.w - 140) * SFX_VOLUME),
        sfxVolumeSlider.y - (sfxVolumeSlider.h),
        140, 140 / 2,
        al_map_rgb(255, 255, 255),
        "Assets/UI_Button.png", "Assets/UI_Button.png");
    sfxVolumeSlider2 = button_create(
        sfxVolumeSlider.x,
        sfxVolumeSlider.y,
        sfxButton.x - sfxVolumeSlider.x, sfxVolumeSlider.h,
        al_map_rgb(255, 255, 255),
        "Assets/slider_hovered.png", "Assets/slider_hovered.png");

    init_bg();
}

static void update(void) {

    update_button(&backButton);
    if (mouseState.buttons && backButton.hovered == true) {
        change_scene(create_menu_scene());
        al_rest(0.4);
    }

    //BGM
    update_button(&bgmVolumeSlider);
    update_button(&bgmButton);
    update_button(&bgmVolumeSlider2);
    if(mouseState.buttons && (bgmVolumeSlider.hovered == true || bgmButton.hovered == true)){
        if (mouseState.x >= bgmVolumeSlider.x &&
            mouseState.x < bgmVolumeSlider.x + bgmVolumeSlider.w) {
            bgmButton.x = mouseState.x - (bgmButton.w / 2);
            if (bgmButton.x < bgmVolumeSlider.x) {
                bgmButton.x = bgmVolumeSlider.x;
            }
            if (bgmButton.x + bgmButton.w > bgmVolumeSlider.x + bgmVolumeSlider.w) {
                bgmButton.x = (bgmVolumeSlider.x + bgmVolumeSlider.w) - bgmButton.w;
            }
        }
        bgmVolumeSlider2.w = bgmButton.x - bgmVolumeSlider.x;
        BGM_VOLUME = ((float)(bgmVolumeSlider2.w) /
            ((float)bgmVolumeSlider.w - bgmButton.w));
    }

    //SFX
    update_button(&sfxVolumeSlider);
    update_button(&sfxButton);
    update_button(&sfxVolumeSlider2);
    if (mouseState.buttons && (sfxVolumeSlider.hovered == true || sfxButton.hovered == true)) {
        if (mouseState.x >= sfxVolumeSlider.x &&
            mouseState.x < sfxVolumeSlider.x + sfxVolumeSlider.w) {
            sfxButton.x = mouseState.x - (sfxButton.w / 2);
            if (sfxButton.x < sfxVolumeSlider.x) {
                sfxButton.x = sfxVolumeSlider.x;
            }
            if (sfxButton.x + sfxButton.w > sfxVolumeSlider.x + sfxVolumeSlider.w) {
                sfxButton.x = (sfxVolumeSlider.x + sfxVolumeSlider.w) - sfxButton.w;
            }
        }
        sfxVolumeSlider2.w = sfxButton.x - sfxVolumeSlider.x;
        SFX_VOLUME = ((float)(sfxVolumeSlider2.w) /
            ((float)sfxVolumeSlider.w - sfxButton.w));
    }

    update_bg();
}

static void draw(void) {
    draw_bg();
    // button
    draw_button(backButton);
    //button text
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        SCREEN_W / 2,
        650 + 28 + backButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "BACK"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        SCREEN_W / 2,
        650 + 31 + backButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "BACK"
    );

    //BGM
    draw_button(bgmVolumeSlider);
    draw_button(bgmVolumeSlider2);
    draw_button(bgmButton);
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        bgmButton.x + (bgmButton.w / 2),
        bgmButton.y + (bgmButton.h / 4),
        ALLEGRO_ALIGN_CENTER,
        "BGM"
    );

    char bgmvol[5];
    snprintf(bgmvol, sizeof(bgmvol), "%.2f", BGM_VOLUME);

    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        bgmVolumeSlider.x - 65,
        bgmVolumeSlider.y,
        ALLEGRO_ALIGN_CENTER,
        bgmvol
    );

    //SFX
    draw_button(sfxVolumeSlider);
    draw_button(sfxVolumeSlider2);
    draw_button(sfxButton);
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        sfxButton.x + (sfxButton.w / 2),
        sfxButton.y + (sfxButton.h / 4),
        ALLEGRO_ALIGN_CENTER,
        "SFX"
    );

    char sfxvol[5];
    snprintf(sfxvol, sizeof(sfxvol), "%.2f", SFX_VOLUME);

    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        sfxVolumeSlider.x - 65,
        sfxVolumeSlider.y,
        ALLEGRO_ALIGN_CENTER,
        sfxvol
    );

}

static void destroy(void) {
    destroy_button(&backButton);
    destroy_button(&bgmVolumeSlider);
    destroy_button(&bgmButton);
    destroy_button(&bgmVolumeSlider2);
    destroy_button(&sfxVolumeSlider);
    destroy_button(&sfxButton);
    destroy_button(&sfxVolumeSlider2);

    destroy_bg();
}

Scene create_setting_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "setting";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}
