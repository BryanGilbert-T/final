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
    // camera
    cam = (Point){ 16, (200 * 64) - SCREEN_H };

    // map
    map = create_map("Assets/timetravel.txt", 0);
    init_timetravel_bg();


    // player
    player = create_player("Assets/timetravel/player_ship.png", map.Spawn.x, map.Spawn.y, 6, 30);

    // cutscene
    inCutscene = true;
    initCutscene(3);

    win = false;
}

void updateTime(void) {
    update_timetravel_bg(2);
    
    // cutscene
    if (inCutscene) {
        updateCutscene();
        return;
    }
    if (win) {
        return;
    }

    // camera and bg
    cam.y -= 2;
    player.coord.y -= 2;
    if (cam.y <= 9000) {
        cam.y -= 2;
        player.coord.y -= 2;
        update_timetravel_bg(2);
    }
    if (cam.y <= 4000) {
        cam.y -= 4;
        player.coord.y -= 4;
        update_timetravel_bg(4);
    }
    if (cam.y <= 0) {
        cam.y = 0;
        inCutscene = true;
        initCutscene(4);
        win = true;
    }
    game_log("%d %d", cam.x, cam.y);
    game_log("%d %d", player.coord.x, player.coord.y);

    // update map
    update_map(&map, player.coord, & coins_obtained);
    update_player(&player, &map);
    if (player.coord.y < cam.y) {
        player.coord.y = cam.y;
    }
    if (player.coord.y + TILE_SIZE > cam.y + SCREEN_H) {
        player.coord.y = cam.y + SCREEN_H - TILE_SIZE;
    }
}

void drawTime(void) {
    draw_map(&map, cam);
    draw_timetravel_bg();
    draw_player(&player, cam);
    if (inCutscene) {
        drawCutscene();
    }   

}

void destroyTime(void) {
    destroy_map(&map);
    destroy_timetravel_bg();
    delete_player(&player);
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