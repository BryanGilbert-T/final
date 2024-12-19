#include <allegro5/allegro.h>
#include <string.h>
#include <stdlib.h>
#include "utility.h"
#include "obstacles.h"
#include "player.h"

ALLEGRO_BITMAP* bonbonBitmap;
ALLEGRO_BITMAP* alanBitmap;
ALLEGRO_BITMAP* lipsBitmap;

static bool playerCollision(Point enemyCoord, Point playerCoord);

void init_obstacle(void) {
    // bonbon
    char* bonbonPath = "Assets/timetravel/bon_bon.png";
    bonbonBitmap = al_load_bitmap(bonbonPath);
    if (!bonbonBitmap) {
        game_abort("Error Load Bitmap with path : %s", bonbonBitmap);
    }

    // alan
    char* alanPath = "Assets/timetravel/alan.png";
    alanBitmap = al_load_bitmap(alanPath);
    if (!alanBitmap) {
        game_abort("Error Load Bitmap with path : %s", alanBitmap);
    }

    // Lips
    char* lipsPath = "Assets/timetravel/lips.png";
    lipsBitmap = al_load_bitmap(lipsPath);
    if (!lipsBitmap) {
        game_abort("Error Load Bitmap with path : %s", lipsBitmap);
    }
}

Obstacle create_obstacle(int row, int col, char ch) {
    Obstacle newObstacle;
    newObstacle.animation_tick = 0;
    newObstacle.death_animation_tick = 0;
    newObstacle.hit_animation_tick = 0;
    newObstacle.status = OBSALIVE;
    newObstacle.attackCD = 0;

    newObstacle.coord = (Point){
        col * TILE_SIZE,
        row * TILE_SIZE
    };
    switch (ch) {
        case 'L':
            newObstacle.damage = 10;
            newObstacle.health = 60;
            newObstacle.type = LIPS;
            newObstacle.image = lipsBitmap;
            break;
        case 'O':
            newObstacle.damage = 10;
            newObstacle.health = 40;
            newObstacle.type = BONBON;
            newObstacle.image = bonbonBitmap;
            break;
        case 'A':
            newObstacle.damage = 10;
            newObstacle.health = 20;
            newObstacle.type = ALAN;
            newObstacle.image = alanBitmap;
            break;
        default:
            newObstacle.damage = 10;
            newObstacle.health = 20;
            newObstacle.type = ALAN;
            newObstacle.image = alanBitmap;
            break;
    }

    return newObstacle;
}

bool updateObstacle(Obstacle * obstacle, Player * player, Map * map) {
    if (obstacle->status == OBSDYING) {
        obstacle->death_animation_tick = (obstacle->death_animation_tick + 1) % 32;
        if (obstacle->death_animation_tick == 32 - 1) {

            int tilex = (obstacle->coord.x + (TILE_SIZE / 2)) / TILE_SIZE;
            int tiley = (obstacle->coord.y + (TILE_SIZE / 2)) / TILE_SIZE;
            if (map->map[tiley][tilex] == FLOOR) {
                map->map[tiley][tilex] = COIN;
                map->coin_status[tiley][tilex] = APPEAR;
            }
            return true;
        }
        return false;
    }
    
    if (obstacle->status != OBSALIVE) return false;
    if (obstacle->attackCD) {
        obstacle->attackCD--;
    }
    if (obstacle->hit_animation_tick) {
        obstacle->hit_animation_tick--;
    }

    obstacle->animation_tick = (obstacle->animation_tick + 1) % 64;  
    if (playerCollision(obstacle->coord, player->coord) && obstacle->attackCD == 0) {
        obstacle->attackCD = 32;
        hitPlayer(player, obstacle->coord, obstacle->damage);
    }

    return false;
}

void drawObstacle(Obstacle obstacle, Point cam) {
    int dx = obstacle.coord.x - cam.x;
    int dy = obstacle.coord.y - cam.y;

    if (dx + TILE_SIZE < 0 || dy + TILE_SIZE < 0) return;

    ALLEGRO_COLOR color = (obstacle.hit_animation_tick) ? al_map_rgb(255, 0, 0) : al_map_rgb(255, 255, 255);
    if (obstacle.status == OBSALIVE) {
        switch (obstacle.type) {
            case BONBON: {
                int offsetx = 48;
                int offsety = 0;
                al_draw_tinted_scaled_bitmap(obstacle.image, color,
                    offsetx, offsety, 16, 16,
                    dx, dy, TILE_SIZE, TILE_SIZE,
                    0);
                break;
                }
            case ALAN: {
                int offsetx = 48 + 16 * (obstacle.animation_tick / (64 / 2));
                int offsety = 0;
                al_draw_tinted_scaled_bitmap(obstacle.image, color,
                    offsetx, offsety, 16, 16,
                    dx, dy, TILE_SIZE, TILE_SIZE,
                    0);
                break;
            }
            case LIPS: {
                int offsetx = 0 + 64 * (obstacle.animation_tick / (64 / 2));
                int offsety = 0;
                al_draw_tinted_scaled_bitmap(obstacle.image, color,
                    offsetx, offsety, 16, 16,
                    dx, dy, TILE_SIZE, TILE_SIZE,
                    0);
                break;
            }

        }
    }
    else if (obstacle.status == OBSDYING) {
        switch (obstacle.type) {
            case BONBON: {
                int offsetx = 16 * (obstacle.death_animation_tick / (32 / 2));
                int offsety = 0;
                al_draw_tinted_scaled_bitmap(obstacle.image, al_map_rgb(255, 0, 0),
                    offsetx, offsety, 16, 16,
                    dx, dy, TILE_SIZE, TILE_SIZE,
                    0);
                break;
            }
            case ALAN: {
                int offsetx = 16 * (obstacle.death_animation_tick / (32 / 2));
                int offsety = 0;
                al_draw_tinted_scaled_bitmap(obstacle.image, al_map_rgb(255, 0, 0),
                    offsetx, offsety, 16, 16,
                    dx, dy, TILE_SIZE, TILE_SIZE,
                    0);
                break;
            }
            case LIPS: {
                int offsetx = 16 * (obstacle.death_animation_tick / (32 / 3));
                int offsety = 0;
                al_draw_tinted_scaled_bitmap(obstacle.image, al_map_rgb(255, 0, 0),
                    offsetx, offsety, 16, 16,
                    dx, dy, TILE_SIZE, TILE_SIZE,
                    0);
                break;
            }
        }
    }
    
}

void destroyObstacle(void) {
    al_destroy_bitmap(bonbonBitmap);
    al_destroy_bitmap(alanBitmap);
    al_destroy_bitmap(lipsBitmap);
}

void hitObstacle(Obstacle * obstacle, int damage) {
    obstacle->health -= damage;
    obstacle->hit_animation_tick = 16;
    if (obstacle->health <= 0 && obstacle->status == OBSALIVE) {
        obstacle->health = 0;
        obstacle->hit_animation_tick = 0;
        obstacle->status = OBSDYING;
    }
}

static bool playerCollision(Point enemyCoord, Point playerCoord) {
    // Rectangle & Rectanlge Collision
    if (enemyCoord.x < playerCoord.x + TILE_SIZE &&
        enemyCoord.x + TILE_SIZE > playerCoord.x &&
        enemyCoord.y < playerCoord.y + TILE_SIZE &&
        enemyCoord.y + TILE_SIZE > playerCoord.y) {
        return true;
    }
    else {
        return false;
    }
}

obstacleNode* createObstacleNode(void) {
    obstacleNode* dummyhead = malloc(sizeof(obstacleNode));
    dummyhead->next = NULL;
    return dummyhead;
}

void insertObstacleNode(obstacleNode* head, Obstacle obstacle) {
    obstacleNode* newObstacleNode = malloc(sizeof(obstacleNode));
    newObstacleNode->obstacle = obstacle;
    newObstacleNode->next = head->next;
    head->next = newObstacleNode;
}

void updateObstacleNode(obstacleNode* head, Player* player, Map * map) {
    obstacleNode* cur = head->next;
    obstacleNode* prev = head;
    while (cur != NULL) {
        bool shouldDelete = updateObstacle(&cur->obstacle, player, map);
        if (shouldDelete) {
            obstacleNode* temp = cur;
            prev->next = cur->next;
            cur = cur->next;
            free(temp);
        }
        else {
            prev = cur;
            cur = cur->next;
        }
    }
}

void drawObstacleNode(obstacleNode* head, Point cam) {
    obstacleNode* cur = head->next;
    while (cur != NULL) {
        drawObstacle(cur->obstacle, cam);
        cur = cur->next;
    }
}

void destroyObstacleNode(obstacleNode* head) {
    obstacleNode* cur = head->next;
    while (cur != NULL) {
        obstacleNode* temp = cur;
        cur = cur->next;
        free(temp);
    }
    free(head);
}

