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

#include <math.h>

Player player; // Player
Map map; // Map
enemyNode * enemyList; // Enemy List
BulletNode * bulletList; // Bullet List


// Weapon
Weapon weapon; 
int coins_obtained;

ALLEGRO_BITMAP* heart;
ALLEGRO_BITMAP* coin;

ALLEGRO_BITMAP* winning_panda;
ALLEGRO_BITMAP* losing_panda;

Scene create_losing_scene(void);

static void init(void){
    
    initEnemy();
    
    map = create_map("Assets/map0.txt", 0);

    player = create_player("Assets/panda2.png", map.Spawn.x, map.Spawn.y);

    enemyList = createEnemyList();
    bulletList = createBulletList();

    heart = al_load_bitmap("Assets/heart.png");
    if (!heart) {
        game_abort("Failed to load heart bitmap");
    }
    
    coin = al_load_bitmap("Assets/coin_icon.png");
    if (!coin) {
        game_abort("Failed to load coin bitmap");
    }

    weapon = create_weapon("Assets/guns.png", "Assets/yellow_bullet.png", 16, 8, 10);
    
    for(int i=0; i<map.EnemySpawnSize; i++){
        Enemy enemy = createEnemy(map.EnemySpawn[i].x, map.EnemySpawn[i].y, map.EnemyCode[i]);
        insertEnemyList(enemyList, enemy);
    }

    game_log("coord x:%d \n coords y:%d \n", map.Spawn.x, map.Spawn.y);
    change_bgm("Assets/audio/game_bgm.mp3");
}

static void update(void){
    /*
        [TODO Homework]
        
        Change the scene if winning/losing to win/lose scene
    */

    if (player.status == PLAYER_DYING && player.animation_tick == 64 - 1) {
        change_scene(create_losing_scene());
        al_rest(2.0);
        return;
    }

    update_player(&player, &map);


    Point Camera;
    /*
        [TODO HACKATHON 1-3]
        
        Calcualte the formula for the Camera
        Camera.x = ...
        Camera.y = ...

        Hint: Adjust it based on player position variable, then subtract it with half of the gameplay screen
    */
    Camera = (Point){ 0, 0 };
    Camera.x = (player.coord.x - (SCREEN_W / 2) + (TILE_SIZE/2));
    Camera.y = (player.coord.y - (SCREEN_H / 2) + (TILE_SIZE/2));


    updateEnemyList(enemyList, &map, &player);
    update_weapon(&weapon, bulletList, player.coord, Camera);
    updateBulletList(bulletList, enemyList, &map);
    update_map(&map, player.coord, &coins_obtained);
    
}

static void draw(void){
    Point Camera;
    /*
        [TODO HACKATHON 1-4]
        
        Calcualte the formula for the Camera
        Camera.x = ...
        Camera.y = ...

        Hint: Just copy from the [TODO HACKATHON 1-3]
    */
    Camera = (Point){ 0, 0 };
    Camera.x = (player.coord.x - (SCREEN_W / 2) + (TILE_SIZE/2));
    Camera.y = (player.coord.y - (SCREEN_H / 2) + (TILE_SIZE/2));
    
    // Draw
    draw_map(&map, Camera);
    drawEnemyList(enemyList, Camera);
    drawBulletList(bulletList, Camera);
    draw_player(&player, Camera);
    draw_weapon(&weapon, player.coord, Camera);

    /*
        [TODO Homework]
        
        Draw the UI of Health and Total Coins
    */
    // UI of health
    int image_width = 64;
    int image_height = 64;

    char healthstr[5];
    snprintf(healthstr, sizeof(healthstr), "%d", player.health);

    char coinstr[5];
    snprintf(coinstr, sizeof(coinstr), "%02d", coins_obtained);

    al_draw_scaled_bitmap(heart, 
        0, 0, 32, 32, // sx, sy, sw, sh (s = source)
        20, 25, image_width, image_height, // x, y, w, h (in game)
        0); // flag

    al_draw_text(P2_FONT, al_map_rgb(255, 255, 255), // Font and color
        93, 40, ALLEGRO_ALIGN_LEFT,  // x, y, align
        healthstr); // string

    // UI of total coins
    al_draw_scaled_bitmap(coin,
        0, 0, 16, 16, // sx, sy, sw, sh (s = source)
        20, 85, image_width, image_height, // x, y, w, h (in game)
        0); // flag

    al_draw_text(P2_FONT, al_map_rgb(255, 255, 255), // Font and color
        93, 103, ALLEGRO_ALIGN_LEFT, // x, y, align
        coinstr); // string
}

static void destroy(void){
    delete_player(&player);
    delete_weapon(&weapon);
    destroy_map(&map);
    destroyBulletList(bulletList);
    destroyEnemyList(enemyList);
    terminateEnemy();
}


Scene create_game_scene(void){
    Scene scene;
    memset(&scene, 0, sizeof(Scene));
    
    scene.name = "game";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;
    
    return scene;
}

static void init_lose(void) {
    losing_panda = al_load_bitmap("Assets/panda_lose.png");
    if (!losing_panda) {
        game_abort("Failed to load Assets/panda_lose.png");
    }
}

static void draw_lose(void) {
    al_draw_scaled_bitmap(losing_panda,
        0, 0, 64, 64,
        SCREEN_W / 4,SCREEN_H / 4, SCREEN_W / 2, SCREEN_H / 2,
        0);
}

static void update_lose(void) {
    draw_lose();
}

static void destroy_lose(void) {
    al_destroy_bitmap(losing_panda);
}


Scene create_losing_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "lose";
    scene.init = &init_lose;
    scene.draw = &draw_lose;
    scene.update = &update_lose;
    scene.destroy = &destroy_lose;

    return scene;
}
