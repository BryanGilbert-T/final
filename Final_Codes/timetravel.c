#include <allegro5/allegro.h>
#include <string.h>
#include "game_scene.h"
#include "menu_scene.h"
#include "loading_scene.h"
#include "game.h"
#include "player.h"
#include "map.h"
#include "enemy.h"
#include "weapon.h"
#include "UI.h"
#include "utility.h"
#include "costumize_scene.h"
#include "leaderboard.h"
#include "cut_scene.h"
#include "background.h"

// what i need:
// - plane
// - enemy
// - dynamic background -> map
// - bullet, how to attack
// - UI health UI coin

Player player; // Player
Map map; // Map
enemyNode* enemyList; // Enemy List
BulletNode* bulletList; // Bullet List

int coins_obtained;

Point cam;

bool win;

void initTime(void) {
    cam = (Point){ 16, (200 * 64) - SCREEN_H };
    map = create_map("Assets/timetravel.txt", 0);
    init_timetravel_bg();

    inCutscene = true;
    initCutscene(3);

    win = false;
}

void updateTime(void) {
    update_timetravel_bg(2);
    if (inCutscene) {
        updateCutscene();
        return;
    }
    if (win) {
        return;
    }
    cam.y -= 2;
    if (cam.y <= 9000) {
        cam.y -= 2;
        update_timetravel_bg(2);
    }
    if (cam.y <= 4000) {
        cam.y -= 4;
        update_timetravel_bg(4);
    }
    if (cam.y <= 0) {
        cam.y = 0;
        inCutscene = true;
        initCutscene(4);
        win = true;
    }
    game_log("%d", cam.y);
    update_map(&map, (Point){ 0, 0 }, &coins_obtained);
}

void drawTime(void) {
    draw_map(&map, cam);
    draw_timetravel_bg();
    if (inCutscene) {
        drawCutscene();
    }   

}

void destroyTime(void) {
    destroy_map(&map);
    destroy_timetravel_bg();
    destroyCutscene();  
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