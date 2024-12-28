#define _USE_MATH_DEFINES
#include "enemy.h"
#include "utility.h"
#include "cut_scene.h"
#include "game_scene.h"

#include <math.h>
#include <stdlib.h>

#define QUEUE_SIZE 2000

/*
    ENEMY IMPLEMENTATION
 */

static Point bunshinSpawnPoint[9] = { {1, 12}, {3, 9 } , {3, 15},
                                        {11, 12}, {9, 9}, {9, 15},
                                        {6, 7}, {6, 17} };

ALLEGRO_BITMAP * slimeBitmap;
ALLEGRO_BITMAP* foxBitmap;
ALLEGRO_BITMAP* minoBitmap;

ALLEGRO_BITMAP* healthBarBitmap;
ALLEGRO_BITMAP* smokeBitmap;

static bool smoke;

// To check if p0 sprite and p1 sprite can go directly
static bool validLine(Map* map, Point p0, Point p1);
// To check if we can have a movement cutting directly to it
static bool bresenhamLine(Map * map, Point p0, Point p1);
// Find shortest path using BFS, could be better if you imeplement your own heap
static Point shortestPath(Map * map, Point src, Point dst);
// Calulate the movement speed to update and scaled it
static Point findScaledDistance(Point p1, Point p2);
// Return true if enemy have collision with unwalkable tiles in map
static bool isCollision(Point enemyCoord, Map* map, enemyType type);
// Return true if player collide with enemy
static bool playerCollision(Point enemyCoord, Point playerCoord, enemyType type);

ALLEGRO_SAMPLE* bunshinSound;

Point minoDelta;
int minoCounter;
bool minoRage;
static int smokeAnimationTick;

void initEnemy(void){
    // For memory efficiency, we load the image once
    // as every enemy bitmap are pointer, they will point to the same global bitmap variable

    // Slime
    char * slimePath = "Assets/Slime.png";
    slimeBitmap = al_load_bitmap(slimePath);
    if(!slimeBitmap){
        game_abort("Error Load Bitmap with path : %s", slimePath);
    }

    // Fox
    char* foxPath = "Assets/fox.png";
    foxBitmap = al_load_bitmap(foxPath);
    if (!foxBitmap) {
        game_abort("Error Load Bitmap with path : %s", foxBitmap);
    }    

    // Mino
    char* minoPath = "Assets/mino.png";
    minoBitmap = al_load_bitmap(minoPath);
    if (!minoBitmap) {
        game_abort("Error Load Bitmap with path : %s", minoBitmap);
    }

    char* healthBarPath = "Assets/slider.png";
    healthBarBitmap = al_load_bitmap(healthBarPath);
    if (!healthBarBitmap) {
        game_abort("Error Load Bitmap with path: %s", healthBarPath);
    }

    char* smokePath = "Assets/bunshin_smoke.png";
    smokeBitmap = al_load_bitmap(smokePath);
    if (!smokeBitmap) {
        game_abort("Error Load Bitmap with path: %s", smokePath);
    }

    bunshinSound = al_load_sample("Assets/audio/bunshinAppear.mp3");
    if (!bunshinSound) {
        game_abort("Error load bunshin sample");
    }

    minoDelta = (Point){ 0, 0 };
    minoCounter = 0;
    minoRage = false;
    smoke = false;
    smokeAnimationTick = 0;
}

Enemy createEnemy(int row, int col, char type){
    game_log("Creating Enemy at ( %d, %d )", row, col);
    
    Enemy enemy;
    memset(&enemy, 0, sizeof(enemy));
    
    enemy.animation_tick = 0;
    enemy.death_animation_tick = 0;
    enemy.status = ALIVE;
    
    enemy.coord = (Point){
        col * TILE_SIZE,
        row * TILE_SIZE
    };
    
    switch(type){
        case 'S':
            enemy.health = 100;
            enemy.type = slime;
            enemy.speed = 2;
            enemy.image = slimeBitmap;
            enemy.maxHealth = 100;
            break;
        // Insert more here to have more enemy variant
        case 'F':
            enemy.health = 300;
            enemy.type = fox;
            enemy.speed = 3;
            enemy.image = foxBitmap;
            enemy.maxHealth = 300;
            break;
        case 'V':
            enemy.health = 10;
            enemy.type = foxBunshin;
            enemy.speed = 3;
            enemy.image = foxBitmap;
            enemy.maxHealth = 10;
            break;
        case 'M':
            enemy.health = 10000;
            enemy.type = mino;
            enemy.speed = 8;
            enemy.image = minoBitmap;
            enemy.maxHealth = 10000;
            break;
        default:
            enemy.health = 100;
            enemy.type = slime;
            enemy.speed = 2;
            enemy.image = slimeBitmap;
            enemy.maxHealth = 100;
            break;
    }
    
    return enemy;
}

static Point shortestPathMino(Map * map, Point minoCoord, Point playerCoord) {
    Point case1 = shortestPath(map, minoCoord, playerCoord);
    // Point case2 = shortestPath(map, (Point) { minoCoord.x + TILE_SIZE, minoCoord.y }, playerCoord);
    // Point case3 = shortestPath(map, (Point) { minoCoord.x, minoCoord.y + TILE_SIZE }, playerCoord);
    Point case4 = shortestPath(map, (Point) { minoCoord.x + TILE_SIZE , minoCoord.y + TILE_SIZE }, playerCoord);

    int x = 0;
    int y = 0;

    if (case1.x) {
        x = case1.x;
    }

    else if (case4.x) {
        x = case4.x;
    }
    
    if (case1.y) {
        y = case1.y;
    }

    else if (case4.y) {
        y = case4.y;
    }
    

    return (Point) { x, y };
}

// Return True if the enemy is dead
bool updateEnemy(Enemy * enemy, Map * map, Player * player){    
    if(enemy->status == DYING){
        /*
            [TODO Homework]
            
            Configure the death animation tick for dying animation,
            Return true when the enemy is dead
        */ 
        enemy->death_animation_tick = enemy->death_animation_tick + 1;
        if (enemy->type == slime) {
            if (enemy->death_animation_tick >= 64) {
                enemy->death_animation_tick = 64;
                int tilex = (enemy->coord.x + TILE_SIZE / 2) / TILE_SIZE;
                int tiley = (enemy->coord.y + TILE_SIZE / 2) / TILE_SIZE;
                if (map->map[tiley][tilex] == FLOOR) {
                    map->map[tiley][tilex] = COIN;
                    map->coin_status[tiley][tilex] = APPEAR;
                }
                return true;
            }
        }
        if (enemy->type == fox) {
            if (enemy->death_animation_tick >= 64) {
                if (map_number == 1) {
                    which_cutscene = 2;
                    boss_fight = false;
                    inCutscene = true;                  
                    initCutscene(which_cutscene);
                }
                if (map_number == 3) {
                   
                    return true;
                }
                enemy->death_animation_tick = 64;
                int tilex = (enemy->coord.x + TILE_SIZE / 2) / TILE_SIZE;
                int tiley = (enemy->coord.y + TILE_SIZE / 2) / TILE_SIZE;
                if (map->map[tiley][tilex] == FLOOR) {
                    map->map[tiley][tilex] = TROPHY;
                }
                return true;
            }
        }
        if (enemy->type == foxBunshin) {
            if (enemy->death_animation_tick >= 64) {
                return true;
            }
        }
       
    }
    
    if(enemy->status != ALIVE) return false;
    if(enemy->type == fox && !boss_fight) return false;
    
    enemy->animation_tick = (enemy->animation_tick + 1) % 64;

    if(enemy->animation_hit_tick > 0){
        enemy->animation_tick = (enemy->animation_tick + 1) % 64;
        enemy->animation_hit_tick--;
    }
    
    if(enemy->knockback_CD > 0 && enemy->type != mino){
        enemy->knockback_CD--;

        int next_x = enemy->coord.x + 4 * cos(enemy->knockback_angle);
        int next_y = enemy->coord.y + 4 * sin(enemy->knockback_angle);
        Point next;
        next = (Point){next_x, enemy->coord.y};
        
        if(!isCollision(next, map, enemy->type)){
            enemy->coord = next;
        }
        
        next = (Point){enemy->coord.x, next_y};
        if(!isCollision(next, map, enemy->type)){
            enemy->coord = next;
        }
    }
    else{
        /*
            [TODO Homework]
            
            Replace delta variable with the function below to start enemy movement
            Point delta = shortestPath(map, enemy->coord, player->coord);
        */
        if (enemy->type == mino) {
            if (minoCounter) {
                minoCounter--;
                return false;
            }

            if (minoRage) {
                enemy->speed = 8 + 4;
            }

            if (minoDelta.x == 0 && minoDelta.y == 0) {
                minoDelta = shortestPathMino(map, enemy->coord, player->coord);
            }
            if (minoDelta.x == 0 && minoDelta.y == 0) {
                minoCounter = 64;
            }

            Point initCoord = enemy->coord;
            Point next, prev = enemy->coord;

            if (minoDelta.x > 0) enemy->dir = RIGHT;
            if (minoDelta.x < 0) enemy->dir = LEFT;

            next = (Point){ enemy->coord.x + minoDelta.x * enemy->speed, enemy->coord.y };
            if (!isCollision(next, map, enemy->type)) {
                enemy->coord = next;
            }

            next = (Point){ enemy->coord.x, enemy->coord.y + minoDelta.y * enemy->speed };
            if (!isCollision(next, map, enemy->type)) {
                enemy->coord = next;
            }

            if (enemy->coord.x == initCoord.x && enemy->coord.y == initCoord.y && (minoDelta.x != 0 || minoDelta.y != 0)) {
                minoCounter = 64;

                if (!minoWreck) {
                    if (minoDelta.x == 1) {
                        map->earthquake_dir = RIGHT;
                    }
                    else if (minoDelta.x == -1) {
                        map->earthquake_dir = LEFT;
                    }
                    if (minoDelta.y == 1) {
                        map->earthquake_dir = DOWN;
                    }
                    else if (minoDelta.y == -1) {
                        map->earthquake_dir = UP;
                    }
                    map->richter = 5;
                }                

                minoDelta.x = 0;
                minoDelta.y = 0;
            }

            // To fix bug if the enemy need to move a little bit, the speed will not be use
            if (enemy->coord.x == prev.x && enemy->coord.y == prev.y) {
                next = (Point){ enemy->coord.x + minoDelta.x, enemy->coord.y };
                if (!isCollision(next, map, enemy->type)) {
                    enemy->coord = next;
                }

                next = (Point){ enemy->coord.x, enemy->coord.y + minoDelta.y };
                if (!isCollision(next, map, enemy->type)) {
                    enemy->coord = next;
                }
            }           

            if (playerCollision(enemy->coord, player->coord, enemy->type) && enemy->animation_hit_tick == 0) {
                minoCounter = 2 * 64;
                minoDelta.x = 0;
                minoDelta.y = 0;
                hitPlayer(player, enemy->coord, 10);
            }
           
            return false;
        }

       
        Point delta = shortestPath(map, enemy->coord, player->coord);
        

        Point next, prev = enemy->coord;
        
        if(delta.x > 0) enemy->dir = RIGHT;
        if(delta.x < 0) enemy->dir = LEFT;
        
        next = (Point){enemy->coord.x + delta.x * enemy->speed, enemy->coord.y};
        if(!isCollision(next, map, enemy->type))
            enemy->coord = next;
        
        next = (Point){enemy->coord.x, enemy->coord.y + delta.y * enemy->speed};
        if(!isCollision(next, map, enemy->type))
            enemy->coord = next;
        
        // To fix bug if the enemy need to move a little bit, the speed will not be use
        if(enemy->coord.x == prev.x && enemy->coord.y == prev.y){
            next = (Point){enemy->coord.x + delta.x, enemy->coord.y};
            if(!isCollision(next, map, enemy->type))
                enemy->coord = next;
            
            next =(Point){enemy->coord.x, enemy->coord.y + delta.y};
            if(!isCollision(next, map, enemy->type))
                enemy->coord = next;
        }
        
        if (enemy->type == slime) {
            if (playerCollision(enemy->coord, player->coord, enemy->type) && enemy->animation_hit_tick == 0) {
                enemy->animation_tick = 0;
                enemy->animation_hit_tick = 32;
                hitPlayer(player, enemy->coord, 10);
            }
        }
        if (enemy->type == fox || enemy->type == foxBunshin) {
            if (playerCollision(enemy->coord, player->coord, enemy->type) && enemy->animation_hit_tick == 0) {
                enemy->animation_tick = 0;
                enemy->animation_hit_tick = 32;
                hitPlayer(player, enemy->coord, 10);
            }
        }
    }

    return false;
}


void drawEnemy(Enemy * enemy, Point cam){

    int dy = enemy->coord.y - cam.y; // destiny y axis
    int dx = enemy->coord.x - cam.x; // destiny x axis

    if(enemy->status == ALIVE){
        int flag = enemy->dir == RIGHT ? 1 : 0;     
        float ratio = (float)((float)enemy->health / (float)enemy->maxHealth);        
        if (enemy->type == slime) {
            int offset = 16 * (int)(enemy->animation_tick / 8);
            if (enemy->animation_hit_tick > 0) {
                offset += 16 * 8;
            }
            int tint_red = enemy->knockback_CD > 0 ? 255 : 0;
            al_draw_tinted_scaled_rotated_bitmap_region(enemy->image, offset, 0, 16, 16, al_map_rgb(tint_red, 255, 255),
                0, 0, dx, dy, TILE_SIZE / 16, TILE_SIZE / 16,
                0, flag);

            al_draw_tinted_scaled_bitmap(healthBarBitmap, al_map_rgb(0, 0, 0),
                0, 0, 600, 20,
                dx, dy, TILE_SIZE, 7,
                0);

            al_draw_tinted_scaled_bitmap(healthBarBitmap, al_map_rgb(200, 0, 0),
                0, 0, 600, 20,
                dx, dy, TILE_SIZE * ratio, 7,
                0);
        }
        if (enemy->type == fox) {
            int offsetx = 32 * (int)(enemy->animation_tick / (64 / 4));
            int offsety = 32;
            if (enemy->animation_hit_tick > 0) {
                offsetx = 0;
                offsety = 64;
            }
            int tint_red = enemy->knockback_CD > 0 ? 0 : 255;
            al_draw_tinted_scaled_bitmap(enemy->image, al_map_rgb(255, tint_red, tint_red),
                offsetx, offsety,
                32, 32,
                dx, dy, TILE_SIZE, TILE_SIZE,
                flag);        
            if (!bunshin) {
                al_draw_tinted_scaled_bitmap(healthBarBitmap, al_map_rgb(0, 0, 0),
                    0, 0, 600, 20,
                    dx, dy, TILE_SIZE, 7,
                    0);

                al_draw_tinted_scaled_bitmap(healthBarBitmap, al_map_rgb(200, 0, 0),
                    0, 0, 600, 20,
                    dx, dy, TILE_SIZE * ratio, 7,
                    0);               
            }
           
        }

        if (enemy->type == foxBunshin) {
            int offsetx = 32 * (int)(enemy->animation_tick / (64 / 4));
            int offsety = 32;
            if (enemy->animation_hit_tick > 0) {
                offsetx = 0;
                offsety = 64;
            }
            //int tint_red = enemy->knockback_CD > 0 ? 0 : 255;
            al_draw_tinted_scaled_bitmap(enemy->image, al_map_rgba(82, 167, 236, 120),
                offsetx, offsety,
                32, 32,
                dx, dy, TILE_SIZE, TILE_SIZE,
                flag);      

            if (smoke) {
                smokeAnimationTick++;
                if (smokeAnimationTick == 128) {
                    smokeAnimationTick = 0;
                    smoke = false;
                }
                int smokeOffset = 32 * (smokeAnimationTick / (128 / 4));
                al_draw_scaled_bitmap(smokeBitmap,
                    smokeOffset, 0, 32, 32,
                    dx, dy, TILE_SIZE, TILE_SIZE,
                    0);
            }
        }
        
        if (enemy->type == mino) {
            flag = (enemy->dir == RIGHT) ? 0 : 1;
            int minoFrame = (minoCounter) ? 4 : 7;
            int offsetx = 20 + (96 * (enemy->animation_tick / (64 / minoFrame)));
            int offsety = (minoCounter) ? 106 + 96 : 106;
            ALLEGRO_COLOR color = (minoRage) ? al_map_rgb(255, 105, 105) : al_map_rgb(255, 255, 255);
            al_draw_tinted_scaled_bitmap(enemy->image, color,
                offsetx, offsety,
                64, 64 - 4,
                dx, dy, TILE_SIZE * 2, TILE_SIZE * 2,
                flag);
        }
    }
    else if(enemy->status == DYING){
        /*
            [TODO Homework]

            Draw Dying Animation for enemy
        */
        

        int flag = enemy->dir == RIGHT ? 1 : 0;
        int tint_red = enemy->knockback_CD > 0 ? 255 : 0;

        int ratio = enemy->health / enemy->maxHealth;
       
        if (enemy->type == slime) {
            int offset = 16 * (int)(enemy->death_animation_tick / 8);
            al_draw_tinted_scaled_rotated_bitmap_region(enemy->image, offset, 16, 16, 16, al_map_rgb(tint_red, 255, 255),
                0, 0, dx, dy, TILE_SIZE / 16, TILE_SIZE / 16,
                0, flag);
        }
        if (enemy->type == fox) {
            int offset = 32 * (int)(enemy->death_animation_tick / (64 / 7));          
            al_draw_tinted_scaled_bitmap(enemy->image, al_map_rgb(255, tint_red, tint_red),
                offset, 64,
                32, 32,
                dx, dy, TILE_SIZE, TILE_SIZE,
                flag);
        }
        if (enemy->type == foxBunshin) {
            int offset = 32 * (int)(enemy->death_animation_tick / (64 / 7));
            al_draw_tinted_scaled_bitmap(enemy->image, al_map_rgb(255, tint_red, tint_red),
                offset, 64,
                32, 32,
                dx, dy, TILE_SIZE, TILE_SIZE,
                flag);
        }
    }
    int area = 1;
    if (enemy->type == mino) {
        area = 2;
    }
#ifdef DRAW_HITBOX
    al_draw_rectangle(
        dx, dy, dx + (area * TILE_SIZE), dy + (area * TILE_SIZE),
        al_map_rgb(255, 0, 0), 1
    );
#endif
}

void destroyEnemy(Enemy * enemy){

}

void terminateEnemy(void) {
    al_destroy_bitmap(slimeBitmap);
    al_destroy_bitmap(minoBitmap);
    al_destroy_bitmap(foxBitmap);
    al_destroy_bitmap(smokeBitmap);
    al_destroy_bitmap(healthBarBitmap);
    al_destroy_sample(bunshinSound);
}

void hitEnemy(Enemy * enemy, int damage, float angle, int knockbackCD){

    /*  
        [TODO Homework]

        Decrease the enemy health with damage, if the health < 0, then set the status to DYING
    
        enemy->health = ...
        if(...){
            enemy->status = DYING;
        }
    */
    if (enemy->type == fox && boss_fight == false) return;
    if (enemy->type == mino) return;

    enemy->health -= damage;
    if (enemy->health <= 0) {
        if (map_number == 3 && enemy->type == fox && bunshin == false) {
            bunshin = true;
            inCutscene = true;
            initCutscene(11);
            enemy->health = enemy->maxHealth;
            smoke = true;
            for (int i = 0; i < 8; i++) {
                insertEnemyList(enemyList, createEnemy(bunshinSpawnPoint[i].x, bunshinSpawnPoint[i].y, 'V'));
                al_play_sample(bunshinSound, SFX_VOLUME, 0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
            }
            return;
        }
        enemy->health = 0;
        enemy->death_animation_tick = 0;
        enemy->status = DYING;
    }

    enemy->knockback_angle = angle;
    enemy->knockback_CD = knockbackCD;
}

/*
    LINKED LIST IMPLEMENTATION FOR ENEMY
    THERE ARE :
        (1) CREATE      : INITIALIZING DUMMY HEAD
        (2) INSERT      : INSERT NEW NODE OF ENEMY
        (3) UPDATE      : ITERATE EVERYTHING AND UPDATE THE ENEMY
        (4) DRAW        : ITERATE EVERYTHING AND DRAW THE ENEMY
        (5) DESTROY     : DESTROY THE LINKED LIST
 */

enemyNode * createEnemyList(void){
    enemyNode * dummyhead = (enemyNode *) malloc(sizeof(enemyNode));
    dummyhead->next = NULL;
    return dummyhead;
}

void insertEnemyList(enemyNode * dummyhead, Enemy _enemy){
    enemyNode * tmp = (enemyNode *) malloc(sizeof(enemyNode));
    tmp->enemy = _enemy;
    tmp->next = dummyhead->next;
    dummyhead->next = tmp;
}

void updateEnemyList(enemyNode * dummyhead, Map * map, Player * player){
    enemyNode * cur = dummyhead->next;
    enemyNode * prev = dummyhead;

    while(cur != NULL){
        bool shouldDelete = updateEnemy(&cur->enemy, map, player);
        if(shouldDelete){
            prev->next = cur->next;
            destroyEnemy(&cur->enemy);
            free(cur);
            cur = prev->next;
        }
        else{
            prev = cur;
            cur = cur->next;
        }
    }
}

void drawEnemyList(enemyNode * dummyhead, Point cam){
    enemyNode * cur = dummyhead->next;
    
    while(cur != NULL){
        drawEnemy(&cur->enemy, cam);
        cur = cur->next;
    }
}

void destroyEnemyList(enemyNode * dummyhead){
    while(dummyhead != NULL){
        enemyNode * del = dummyhead;
        dummyhead = dummyhead->next;
        destroyEnemy(&del->enemy);
        free(del);
    }
}

static bool validLine(Map * map, Point p0, Point p1){
    int offsetX[4] = {0, 0, TILE_SIZE-1, TILE_SIZE-1};
    int offsetY[4] = {0, TILE_SIZE-1, 0, TILE_SIZE-1};
    
    for(int i=0; i<4; i++){
        Point n1 = (Point){
            p0.x + offsetX[i],
            p0.y + offsetY[i]
        };
        
        Point n2 = (Point){
            p1.x + offsetX[i],
            p1.y + offsetY[i]
        };
        if(bresenhamLine(map, n1, n2)) return false;
    }
    
    return true;
}

static Point shortestPath(Map * map, Point enemy, Point player){
    // Point enemy & player is pixel coordinate
    static DIRECTION dir[MAX_MAP_ROW][MAX_MAP_COL]; // to backtrack from dst to src
    static bool visit[MAX_MAP_ROW][MAX_MAP_COL];
    memset(visit, 0, sizeof(visit));
    
    // Point declared below is not coordinate of pixel, but COORDINATE OF ARRAY!
    Point src = (Point){
        enemy.y / TILE_SIZE,
        enemy.x / TILE_SIZE
    };
    Point dst = (Point){
        player.y / TILE_SIZE,
        player.x / TILE_SIZE
    };
    
    static Point Queue[QUEUE_SIZE];
    int front = 0, rear = 0;
    
    Queue[rear++] = src;
    bool found = false;
    
    // Movement set
    static int dx[4] = {1, -1, 0, 0};
    static int dy[4] = {0, 0, 1, -1};
    static DIRECTION move[4] = {UP, DOWN, LEFT, RIGHT}; // To backtrack
    
    while(front != rear){
        Point cur = Queue[front++];
        
        // Found the destiny
        if(cur.x == dst.x && cur.y == dst.y) {
            found = true;
            break;
        };
        
        for(int i=0; i<4; i++){
            Point next = (Point){
                cur.x + dx[i],
                cur.y + dy[i]
            };
            
            if(next.x < 0 || next.y < 0) continue;
            if(next.x >= map->row || next.y >= map->col) continue;
            
            if(isWalkable(map->map[next.x][next.y]) && !visit[next.x][next.y]){
                dir[next.x][next.y] = move[i];
                visit[next.x][next.y] = true;
                Queue[rear++] = next;
            }
        }
    }
    
    // Toward a String-Pulling Approach to Path Smoothing on Grid Graphs
    // http://idm-lab.org/bib/abstracts/papers/socs20c.pdf
    if(found){
        if(validLine(map, enemy, player))
            return findScaledDistance(enemy, player);
        
        int max_iteration = 1000;
        Point it = dst;
        while(max_iteration--){
            Point translate_it = (Point){
                it.y * TILE_SIZE,
                it.x * TILE_SIZE
            };
            
            if(validLine(map, enemy, translate_it))
                return findScaledDistance(enemy, translate_it);
            
            switch(dir[it.x][it.y]){
                case UP:
                    it.x--;
                    break;
                case DOWN:
                    it.x++;
                    break;
                case LEFT:
                    it.y--;
                    break;
                case RIGHT:
                    it.y++;
                    break;
                default:
                    goto END;
            }
            
        }
        END:
        game_log("FAILED TO ITERATE");
    }
    return (Point){0, 0};
}

static bool bresenhamLine(Map * map, Point p0, Point p1){
    int dx =  abs(p1.x - p0.x), sx = p0.x < p1.x ? 1 : -1;
    int dy =  -abs(p1.y - p0.y), sy = p0.y < p1.y ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        int tile_x = p0.y / TILE_SIZE;
        int tile_y = p0.x / TILE_SIZE;
        if(!isWalkable(map->map[tile_x][tile_y])) return true;
        
        if (p0.x == p1.x && p0.y == p1.y) break;
        e2 = 2 * err;

        if (e2 > dy) {
            err += dy;
            p0.x += sx;
        } else if (e2 < dx) {
            err += dx;
            p0.y += sy;
        }
    }
    
    return false;
}

static Point findScaledDistance(Point p1, Point p2) {
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    
    double d = sqrt(dx * dx + dy * dy);
    
    // Floating error fix, when smaller than delta it will be immediately 0
    if (d < 0.001) {
        return (Point) {0, 0};
    }

    double dxUnit = dx / d;
    double dyUnit = dy / d;
    
    return (Point){round(dxUnit), round(dyUnit)};
}


static bool playerCollision(Point enemyCoord, Point playerCoord, enemyType type){
    int area = 1;
    if (type == mino) {
        area = 2;
    }
    // Rectangle & Rectanlge Collision
    if (enemyCoord.x < playerCoord.x + TILE_SIZE &&
        enemyCoord.x + (area * TILE_SIZE) > playerCoord.x &&
        enemyCoord.y < playerCoord.y + TILE_SIZE &&
        enemyCoord.y + (area * TILE_SIZE) > playerCoord.y) {
            return true;
    } else {
        return false;
    }
}
    
static bool isCollision(Point enemyCoord, Map* map, enemyType type){
    int area = 1;
    if (type == mino) {
        area = 2;
    }
    if( enemyCoord.x < 0 || 
        enemyCoord.y < 0 || 
        (enemyCoord.x + (area * TILE_SIZE) - 1) / TILE_SIZE >= map->col ||
        (enemyCoord.y + (area * TILE_SIZE) - 1) / TILE_SIZE >= map->row) 
        return true;

    /* 
        [TODO HACKATHON 2-2] 

        Check every corner of enemy if it's collide or not
        Return true if it's not walkable

        if(!isWalkable(map->map[...][...])) return true;
        if(!isWalkable(map->map[...][...])) return true;
        if(!isWalkable(map->map[...][...])) return true;
        if(!isWalkable(map->map[...][...])) return true;
    */

    int tileX1 = enemyCoord.x / TILE_SIZE;
    int tileY1 = enemyCoord.y / TILE_SIZE;

    int tileX2 = (enemyCoord.x + (area * TILE_SIZE) - 1) / TILE_SIZE;
    int tileY2 = (enemyCoord.y + (area * TILE_SIZE) - 1) / TILE_SIZE;

    if (!isWalkable(map->map[tileY1][tileX1])) return true;
    if (!isWalkable(map->map[tileY2][tileX2])) return true;
    if (!isWalkable(map->map[tileY2][tileX1])) return true;
    if (!isWalkable(map->map[tileY1][tileX2])) return true;

    return false;
}
