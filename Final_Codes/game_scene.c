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

Button continueButton;
Button menuButton;
Button timetravelButton;

Scene create_losing_scene(void);
Scene create_winning_scene(void);

Button pauseButton;

bool pause;
bool timetravel_req;
static int rect_w = 480;
static int rect_h = 240;

static void init(void){
    
    initEnemy();
    
    char map_num[3];
    snprintf(map_num, sizeof(map_num), "%d", map_number);
    char map_path[50] = "Assets/map";
    strcat_s(map_path, sizeof(map_path), map_num);
    strcat_s(map_path, sizeof(map_path), ".txt");
    map = create_map(map_path, 0);

    pause = false;
    timetravel_req = false;

    pauseButton = button_create(SCREEN_W - 90, 40,
        TILE_SIZE, TILE_SIZE,
        al_map_rgb(255, 255, 255),
        "Assets/pause_button.png", "Assets/pause_button_hovered.png");
    menuButton = button_create(SCREEN_W / 2 - ((rect_w - 150) / 2), SCREEN_H / 2 - 30 - 60,
        rect_w - 150, 80,
        al_map_rgb(255, 255, 255),
        "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    continueButton = button_create(SCREEN_W / 2 - ((rect_w - 150) / 2), SCREEN_H / 2,
        rect_w - 150, 80,
        al_map_rgb(255, 255, 255),
        "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    timetravelButton = button_create(SCREEN_W / 2 - ((rect_w - 150) / 2), SCREEN_H / 2 - 45,
        rect_w - 150, 80,
        al_map_rgb(255, 255, 255),
        "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");

    if (player_type == PANDA) {
        player = create_player("Assets/panda2.png", map.Spawn.x, map.Spawn.y, 4, 50);
    }
    if (player_type == FIONA) {
        player = create_player("Assets/player.png", map.Spawn.x, map.Spawn.y, 6, 30);
    }
   

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
    
    if (player_weapon == SMG) {
        weapon = create_weapon("Assets/guns.png", "Assets/yellow_bullet.png", 16, 8, 100);
    }
    if (player_weapon == SNIPER) {
        weapon = create_weapon("Assets/sniper.png", "Assets/orange_bullet.png", 150, 16, 100);
    }
    
    
    for(int i=0; i<map.EnemySpawnSize; i++){
        Enemy enemy = createEnemy(map.EnemySpawn[i].x, map.EnemySpawn[i].y, map.EnemyCode[i]);
        insertEnemyList(enemyList, enemy);
    }

    game_log("coord x:%d \n coords y:%d \n", map.Spawn.x, map.Spawn.y);
    change_bgm("Assets/audio/killer.mp3");

    if (map_number == 0) {
        which_cutscene = 0;
        inCutscene = true;
    }
    if (map_number == 1) {
        which_cutscene = 1;
        inCutscene = true;
    }
    if (inCutscene) {
        initCutscene(which_cutscene);
    }
}

static void update(void){
    /*
        [TODO Homework]
        
        Change the scene if winning/losing to win/lose scene
    */
    if (timetravel) {
        change_scene(create_loading_scene());
        return;
    }

    if (timetravel_req) {
        update_button(&timetravelButton);
        if (timetravelButton.hovered && mouseState.buttons) {
            timetravel = true;
            total_coins += coins_obtained;
            points_accumulated += (coins_obtained * 10);
            coins_obtained = 0;
            return;
        }
    }

    if (pause) {
        update_button(&menuButton);
        if (menuButton.hovered && mouseState.buttons) {
            coins_obtained = 0;
            change_scene(create_menu_scene());
        }
        update_button(&continueButton);
        if (continueButton.hovered && mouseState.buttons) {
            pause = false;
        }
        return;
    }  

    update_button(&pauseButton);
    if ((pauseButton.hovered && mouseState.buttons) || keyState[ALLEGRO_KEY_ESCAPE]) {
        pause = true;
        return;
    }

    if (inCutscene) {
        updateCutscene();
        return;
    }
    
    if (player.status == PLAYER_DYING && player.animation_tick == 64 - 1) {
        coins_obtained = 0;
        change_scene(create_losing_scene());
        al_rest(1.0);
        return;
    }
    
    if (map.win) {
        al_play_sample(map.trophy_audio, SFX_VOLUME, 0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);

        if (map_number == 1) {
            timetravel_req = true;
            return;
        }

        map_number += 1;
        total_coins += coins_obtained;
        points_accumulated += (coins_obtained * 10);
        coins_obtained = 0;
        al_rest(2.0);
        if (map_number - 1 == max_map_number) {
            change_scene(create_submit_scene());
            return;
        }
        change_scene(create_winning_scene());
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

    if (Camera.x < 0) {
        Camera.x += 0 - Camera.x;
    }
    if (Camera.y < 0) {
        Camera.y += 0 - Camera.y;
    }

    int map_width = map.col * TILE_SIZE;
    int map_height = map.row * TILE_SIZE;

    if (Camera.x > map_width - SCREEN_W) {
        Camera.x = map_width - SCREEN_W;
    }

    if (Camera.y > map_height - SCREEN_H) {
        Camera.y = map_height - SCREEN_H;
    }


    updateEnemyList(enemyList, &map, &player);
    if (player.status != PLAYER_DYING) {
        update_weapon(&weapon, bulletList, player.coord, Camera);
    }
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

    if (Camera.x < 0) {
        Camera.x += 0 - Camera.x;
    }
    if (Camera.y < 0) {
        Camera.y += 0 - Camera.y;
    }

    int map_width = map.col * TILE_SIZE;
    int map_height = map.row * TILE_SIZE;

    if (Camera.x > map_width - SCREEN_W) {
        Camera.x = map_width - SCREEN_W;
    }

    if (Camera.y > map_height - SCREEN_H) {
        Camera.y = map_height - SCREEN_H;
    }

   
    // Draw
    draw_map(&map, Camera);
    drawEnemyList(enemyList, Camera);
    drawBulletList(bulletList, Camera);
    draw_player(&player, Camera);
    if (player.status != PLAYER_DYING) {
        draw_weapon(&weapon, player.coord, Camera);
    }

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

    int i;
    for (i = 0; i < (player.health / 10); i++) {
        al_draw_scaled_bitmap(heart,
            0, 0, 32, 32, // sx, sy, sw, sh (s = source)
            20 + (i * (image_width - 12)), 25, image_width, image_height, // x, y, w, h (in game)
            0); // flag
    }

    //al_draw_text(P2_FONT, al_map_rgb(255, 255, 255), // Font and color
    //    93, 40, ALLEGRO_ALIGN_LEFT,  // x, y, align
    //    healthstr); // string

    // UI of total coins
    al_draw_scaled_bitmap(coin,
        0, 0, 16, 16, // sx, sy, sw, sh (s = source)
        20, 85, image_width, image_height, // x, y, w, h (in game)
        0); // flag

    al_draw_text(P2_FONT, al_map_rgb(255, 255, 255), // Font and color
        93, 103, ALLEGRO_ALIGN_LEFT, // x, y, align
        coinstr); // string

    draw_button(pauseButton);

    if (inCutscene) {     
        drawCutscene();
        
    }

    if (pause) {
        al_draw_filled_rounded_rectangle(SCREEN_W / 2 - (rect_w / 2) - 10, SCREEN_H / 2 - (rect_h / 2) - 10,
            SCREEN_W / 2 + (rect_w / 2) + 10, SCREEN_H / 2 + (rect_h / 2) + 10,
            50, 50,
            al_map_rgb(0, 109, 191));
        al_draw_filled_rounded_rectangle(SCREEN_W / 2 - (rect_w / 2), SCREEN_H / 2 - (rect_h / 2),
            SCREEN_W / 2 + (rect_w / 2), SCREEN_H / 2 + (rect_h / 2),
            50, 50,
            al_map_rgb(46, 146, 255));
       /* draw_button(menuButton);
        draw_button(continueButton);*/
        ALLEGRO_COLOR menu_color = (menuButton.hovered) ? al_map_rgb(255, 200, 89) : al_map_rgb(225, 225, 225);
        al_draw_text(
            P2_FONT,
            al_map_rgb(66, 76, 110),
            menuButton.x + (menuButton.w / 2),
            (menuButton.y + 7) + 20,
            ALLEGRO_ALIGN_CENTER,
            "MENU"
        );
        al_draw_text(
            P2_FONT,
            menu_color,
            menuButton.x + (menuButton.w / 2),
            (menuButton.y + 7) + 24,
            ALLEGRO_ALIGN_CENTER,
            "MENU"
        );

        ALLEGRO_COLOR continue_color = (continueButton.hovered) ? al_map_rgb(255, 200, 89) : al_map_rgb(225, 225, 225);
        al_draw_text(
            P2_FONT,
            al_map_rgb(66, 76, 110),
            continueButton.x + (continueButton.w / 2),
            (continueButton.y + 7) + 20,
            ALLEGRO_ALIGN_CENTER,
            "CONTINUE"
        );
        al_draw_text(
            P2_FONT,
            continue_color,
            continueButton.x + (continueButton.w / 2),
            (continueButton.y + 7) + 24,
            ALLEGRO_ALIGN_CENTER,
            "CONTINUE"
        );
    }

    if (timetravel_req) {
        al_draw_filled_rounded_rectangle(SCREEN_W / 2 - (rect_w / 2) - 10, SCREEN_H / 2 - (rect_h / 2) - 10,
            SCREEN_W / 2 + (rect_w / 2) + 10, SCREEN_H / 2 + (rect_h / 2) + 10,
            50, 50,
            al_map_rgb(0, 109, 191));
        al_draw_filled_rounded_rectangle(SCREEN_W / 2 - (rect_w / 2), SCREEN_H / 2 - (rect_h / 2),
            SCREEN_W / 2 + (rect_w / 2), SCREEN_H / 2 + (rect_h / 2),
            50, 50,
            al_map_rgb(46, 146, 255));

        ALLEGRO_COLOR timecolor = (timetravelButton.hovered) ? al_map_rgb(255, 200, 89) : al_map_rgb(225, 225, 225);
        al_draw_text(
            P2_FONT,
            al_map_rgb(66, 76, 110),
            timetravelButton.x + (timetravelButton.w / 2),
            (timetravelButton.y + 7) + 20,
            ALLEGRO_ALIGN_CENTER,
            "TIME TRAVEL?"
        );
        al_draw_text(
            P2_FONT,
            timecolor,
            timetravelButton.x + (timetravelButton.w / 2),
            (timetravelButton.y + 7) + 24,
            ALLEGRO_ALIGN_CENTER,
            "TIME TRAVEL?"
        );
    }

}

static void destroy(void){
    delete_player(&player);
    delete_weapon(&weapon);
    destroy_map(&map);
    destroyBulletList(bulletList);
    destroyEnemyList(enemyList);
    terminateEnemy();
    destroy_button(&menuButton);
    destroy_button(&continueButton);
    destroy_button(&pauseButton);
    destroy_button(&timetravelButton);
    destroyCutscene();
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

    menuButton = button_create(SCREEN_W / 2 - 50 - 240, SCREEN_H - 200, 240, 120,
        al_map_rgb(255, 255, 255),
        "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    continueButton = button_create(SCREEN_W / 2 + 50, SCREEN_H - 200, 240, 120,
        al_map_rgb(255, 255, 255),
        "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");

}

static void draw_lose(void) {
    al_draw_scaled_bitmap(losing_panda,
        0, 0, 64, 64,
        SCREEN_W / 4,SCREEN_H / 4 - 40, SCREEN_W / 2, SCREEN_H / 2,
        0);

    draw_button(menuButton);
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        SCREEN_W / 2 - 50 - 120,
        (SCREEN_H - 190) + 28 + menuButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "MENU"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        SCREEN_W / 2 - 50 - 120,
        (SCREEN_H - 190) + 31 + menuButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "MENU"
    );

    draw_button(continueButton);
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        SCREEN_W / 2 + 50 + 120,
        (SCREEN_H - 190) + 28 + continueButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "TRY AGAIN"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        SCREEN_W / 2 + 50 + 120,
        (SCREEN_H - 190) + 31 + continueButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "TRY AGAIN"
    );

    al_draw_text(
        P1_FONT,
        al_map_rgb(225, 225, 225),
        SCREEN_W / 2,
        100,
        ALLEGRO_ALIGN_CENTER,
        "YOU LOSE"
    );

}

static void update_lose(void) {
    draw_lose();
    update_button(&menuButton);
    update_button(&continueButton);

    if (mouseState.buttons && menuButton.hovered) {
        change_scene(create_menu_scene());
        al_rest(1.0);
        return;
    }

    if (mouseState.buttons && continueButton.hovered) {
        change_scene(create_costumize_scene());
        al_rest(1.0);
        return;
    }
}

static void destroy_lose(void) {
    al_destroy_bitmap(losing_panda);
    destroy_button(&menuButton);
    destroy_button(&continueButton);
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

static void init_win(void) {
    winning_panda = al_load_bitmap("Assets/panda_win.png");
    if (!winning_panda) {
        game_abort("Fail to load panda_win.png");
    }

    menuButton = button_create(SCREEN_W / 2 - 50 - 240, SCREEN_H - 200, 240, 120,
        al_map_rgb(255, 255, 255), 
        "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    continueButton = button_create(SCREEN_W / 2 + 50, SCREEN_H - 200, 240, 120,
        al_map_rgb(255, 255, 255),
        "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
}

static void draw_win(void) {
    int pandaY = SCREEN_H / 4 - 60;
    draw_button(menuButton);
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        SCREEN_W / 2 - 50 - 120,
        (SCREEN_H - 190) + 28 + menuButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "MENU"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        SCREEN_W / 2 - 50 - 120,
        (SCREEN_H - 190) + 31 + menuButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "MENU"
    );

    draw_button(continueButton);
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        SCREEN_W / 2 + 50 + 120,
        (SCREEN_H - 190) + 28 + continueButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "CONTINUE"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        SCREEN_W / 2 + 50 + 120,
        (SCREEN_H - 190) + 31 + continueButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "CONTINUE"
    );

    al_draw_text(
        P1_FONT,
        al_map_rgb(225, 225, 225),
        SCREEN_W / 2,
        100,
        ALLEGRO_ALIGN_CENTER,
        "YOU WIN"
    );

    al_draw_scaled_bitmap(winning_panda,
        0, 0, 64, 64,
        SCREEN_W / 4, pandaY, SCREEN_W / 2, SCREEN_H / 2,
        0);

}

static void update_win(void) {
    draw_win();
    update_button(&menuButton);
    update_button(&continueButton);

    if (mouseState.buttons && menuButton.hovered) {
        change_scene(create_menu_scene());
        return;
    }

    if (mouseState.buttons && continueButton.hovered) {
        change_scene(create_costumize_scene());
        al_rest(0.2);
        return;
    }
}

static void destroy_win(void) {
    al_destroy_bitmap(winning_panda);
    destroy_button(&menuButton);
    destroy_button(&continueButton);
}


Scene create_winning_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "win";
    scene.init = &init_win;
    scene.draw = &draw_win;
    scene.update = &update_win;
    scene.destroy = &destroy_win;

    return scene;
}
