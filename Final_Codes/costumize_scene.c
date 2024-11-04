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

}

static void update(void) {

}

static void draw(void) {
   
}

static void destroy(void) {
    
}


Scene create_loading_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "costumize";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}
