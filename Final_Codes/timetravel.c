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
#include "cut_scene.h"
#include "timetravel.h"

// what i need:
// - plane
// - enemy
// - dynamic background -> map
// - bullet, how to attack
// - UI health UI coin


void initTime(void) {

}

void updateTime(void) {

}

void drawTime(void) {

}

void destroyTime(void) {

}

Scene create_timetravel_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "timetravel";
    scene.init = &initTime;
    scene.draw = &drawTime;
    scene.update = &updateTime;
    scene.destroy = &destroyTime;

    return scene;
}