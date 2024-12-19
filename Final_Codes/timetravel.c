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
#include "beam.h"
#include "obstacles.h"

// what i need:
// - plane
// - enemy
// - dynamic background -> map
// - bullet, how to attack
// - UI health UI coin

Player player; // Player
Map map; // Map
BeamNode* beamNode;
obstacleNode* obstacleList;

int coins_obtained;
int shootCD;
int speed;

ALLEGRO_BITMAP* health_UI;
ALLEGRO_BITMAP* coin_UI;

ALLEGRO_SAMPLE* imperialMarch;

Button pauseButton;
Button continueButton;
Button menuButton;

Point cam;

bool win;
bool pause;

static int rect_w = 480;
static int rect_h = 240;

void initTime(void) {
    // camera
    cam = (Point){ 16, (200 * 64) - SCREEN_H };

    // map
    map = create_map("Assets/timetravel.txt", 0);
    init_timetravel_bg();


    // player
    player = create_player("Assets/timetravel/player_ship.png", map.Spawn.x, map.Spawn.y, 6, 50);

    init_obstacle();
    beamNode = createBeamNode();
    obstacleList = createObstacleNode();

    for (int i = 0; i < map.EnemySpawnSize; i++) {
        Obstacle obstacle = create_obstacle(map.EnemySpawn[i].x, map.EnemySpawn[i].y, map.EnemyCode[i]);
        insertObstacleNode(obstacleList, obstacle);
    }

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

    char* health_path = "Assets/timetravel/player_ship.png";
    health_UI = al_load_bitmap(health_path);
    if (!health_UI) {
        game_abort("can't find %s", health_path);
    }

    char* coin_path = "Assets/coin_icon.png";
    coin_UI = al_load_bitmap(coin_path);
    if (!coin_UI) {
        game_abort("cant find %s", coin_path);
    }

    char* imperialPath = "Assets/audio/imperial_march.mp3";
    change_bgm(imperialPath);

    // cutscene
    inCutscene = true;
    pause = false;
    initCutscene(3);

    win = false;

    shootCD = 0;
    coins_obtained = 0;
}

void updateTime(void) {
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

    update_timetravel_bg(2);
    speed = 0;

    // cutscene
    if (inCutscene) {
        updateBeamNode(beamNode, cam, speed, obstacleList);
        updateCutscene();
        return;
    }
    if (win) {
        if (player.coord.y > cam.y) {
            player.direction = UP;
            player.coord.y -= player.speed;
        }
        else {
            change_scene(create_winning_scene());
        }
        return;
    }
    if (player.status == PLAYER_DYING && player.animation_tick == 63) {
        coins_obtained = 0;
        change_scene(create_losing_scene());
        al_rest(1.0);
        return;
    }

    // camera and bg
    cam.y -= 2;
    player.coord.y -= 2;
    speed += 2;
    if (cam.y <= 9000) {
        cam.y -= 4;
        player.coord.y -= 4;
        speed += 4;
        update_timetravel_bg(4);
    }
    if (cam.y <= 4000) {
        cam.y -= 4;
        player.coord.y -= 4;
        speed += 4;
        update_timetravel_bg(4);
    }
    if (cam.y <= 0) {
        cam.y = 0;
        inCutscene = true;
        initCutscene(4);
        win = true;
    }
    if (shootCD) {
        shootCD--;
    }
    if ((keyState[ALLEGRO_KEY_SPACE] || mouseState.buttons & 1) && shootCD == 0) {
        Beam beam = createBeam("Assets/timetravel/player_beam.png", 10, 50, player.coord);
        insertBeamNode(beamNode, beam);
        shootCD = 32;
    }

    // update map
    update_map(&map, player.coord, &coins_obtained);
    update_player(&player, &map);    
    updateBeamNode(beamNode, cam, speed, obstacleList);
    updateObstacleNode(obstacleList, &player, &map);
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
    drawObstacleNode(obstacleList, cam);
    draw_coins(&map, cam);
    drawBeamNode(beamNode, cam);
    draw_player(&player, cam);
    if (inCutscene) {
        drawCutscene();
    }   

    // UI health
    for (int i = 0; i < (player.health / 10); i++) {
        al_draw_tinted_scaled_bitmap(health_UI, al_map_rgb(225, 225, 225),
            16, 0, 16, 16, // sx, sy, sw, sh (s = source)
            20 + (i * (TILE_SIZE + 10)), 25, TILE_SIZE, TILE_SIZE, // x, y, w, h (in game)
            0); // flag
    }

    // UI Coin
    al_draw_scaled_bitmap(coin_UI,
        0, 0, 16, 16, // sx, sy, sw, sh (s = source)
        20, 85 + 5, TILE_SIZE, TILE_SIZE, // x, y, w, h (in game)
        0); // flag

    char coinstr[5];
    snprintf(coinstr, sizeof(coinstr), "%02d", coins_obtained + total_coins);

    al_draw_text(P2_FONT, al_map_rgb(255, 255, 255), // Font and color
        93, 103 + 5, ALLEGRO_ALIGN_LEFT, // x, y, align
        coinstr); // string

    draw_button(pauseButton);
    // Pause
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

}

void destroyTime(void) {
    destroy_map(&map);
    destroy_timetravel_bg();
    delete_player(&player);
    destroyCutscene(); 
    deleteBeamNode(beamNode);

    al_destroy_bitmap(health_UI);
    al_destroy_bitmap(coin_UI);

    destroy_button(&menuButton);
    destroy_button(&continueButton);
    destroy_button(&pauseButton);

    destroyObstacleNode(obstacleList);
    destroyObstacle();
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