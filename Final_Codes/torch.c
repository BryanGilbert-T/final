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

#include "torch.h"

#include <math.h>

#define QUEUE_SIZE 2000

bool torch = false;

int torchTimer;

int idleAnimationTick;
int attackAnimationTick;
int appearAnimationTick;
int disappearAnimationTick;

int torchAmmo;
int maxTorch = 5;

int torchReload;
int speed = 6;

TorchStatus status;

ALLEGRO_BITMAP* torchImage;

Point coord;

Enemy enemyTarget;

bool findEnemyTarget(enemyNode* enemyList, Point playerCoord) {
    enemyNode* cur = enemyList->next;
    int shortest = 0;

    while (cur != NULL) {
            
    }

    return false;
}

void initTorch(void) {
    torchTimer = 0;
    torchAmmo = 5;
    torchReload = 5 * FPS;
    torch = false;

    torchImage = al_load_bitmap("Assets/fire_torch.png");
}

void updateTorch(Point playerCoord, enemyNode* enemyList) {
    if (!minoWreck) return;



    if (status == T_APPEAR || status == T_IDLE) {
        coord.x = playerCoord.x;
        coord.y = playerCoord.y - 64;
    }

    if (torchAmmo < maxTorch) {
        if (torchReload == 0) {
            torchReload = 5 * FPS;
            torchAmmo += 1;
        }
        else {
            torchReload--;
        }
    }

    if (torchTimer == 0 && status == T_IDLE) {
        status = T_DISAPPEAR;
        disappearAnimationTick = 0;
    }
    else if(status == T_IDLE){
        torchTimer--;
    }


    if (keyState[ALLEGRO_KEY_E]) {
        if (torch == false) {
            torch = true;
            torchTimer = FPS * 10;
            torchAmmo -= 1;
            appearAnimationTick = 0;
            status = T_APPEAR;
            coord.x = playerCoord.x + 32;
            coord.y = playerCoord.y - 32;
        }
    }

    if (mouseState.buttons == 2) {
        if (torch == true && status == T_IDLE) {
            status = T_ATTACK;
            attackAnimationTick = 0;
        }
    }


    // AnimationTicks
    if (status == T_APPEAR) {
        appearAnimationTick = (appearAnimationTick + 1) % 64;
        if (appearAnimationTick == 63) {
            appearAnimationTick = 0;
            status = T_IDLE;
        }
    }
    if (status == T_IDLE) {
        idleAnimationTick = (idleAnimationTick + 1) % 64;     
    }
    if (status == T_ATTACK) {
        attackAnimationTick = (attackAnimationTick + 1) % 32;
        if (attackAnimationTick == 31) {
            attackAnimationTick = 0;
            status = T_ATTACKING;
        }
    }
    if (status == T_ATTACKING) {
        attackAnimationTick = (attackAnimationTick + 1) % 32;
        // if hit enemy or hit wall disappear
    }
    if (status == T_DISAPPEAR) {
        disappearAnimationTick = (disappearAnimationTick + 1) % 32;
        if (disappearAnimationTick == 31) {
            disappearAnimationTick = 0;
            torch = false;
        }
    }
    
}

void drawTorch(Point cam) {
    if (!minoWreck)return;

    // Draw torch ammonition
    for (int i = 0; i < torchAmmo; i++) {
        al_draw_tinted_scaled_bitmap(torchImage, al_map_rgb(255, 255, 255),
            0, 64, 64, 64,
            400 + (i * TILE_SIZE), 100, 2 * TILE_SIZE, 2 * TILE_SIZE,
            0);
    }


    if (!torch) return;

    int dy = coord.y - cam.y; // destiny y axis
    int dx = coord.x - cam.x; // destiny x axis

    if (status == T_APPEAR) {
        int offsetx = 64 * (appearAnimationTick / (64 / 14));
        int offsety = 0;
        
        al_draw_tinted_scaled_bitmap(torchImage, al_map_rgb(255, 255, 255),
            offsetx, offsety, 64, 64,
            dx, dy, 2 * TILE_SIZE, 2 * TILE_SIZE, 
            0);
    }
    if (status == T_IDLE) {
        int offsetx = 64 * (idleAnimationTick / (64 / 4));
        int offsety = 64;

        al_draw_tinted_scaled_bitmap(torchImage, al_map_rgb(255, 255, 255),
            offsetx, offsety, 64, 64,
            dx, dy, 2 * TILE_SIZE, 2 * TILE_SIZE,
            0);
    }
    if (status == T_ATTACK) {
        int offsetx = 64 * (attackAnimationTick / (32 / 3));
        int offsety = 128;

        al_draw_tinted_scaled_bitmap(torchImage, al_map_rgb(255, 255, 255),
            offsetx, offsety, 64, 64,
            dx, dy, 2 * TILE_SIZE, 2 * TILE_SIZE,
            0);
    }
    if (status == T_ATTACKING) {
        int offsetx = 64 * (attackAnimationTick / (32 / 4));
        int offsety = 192;

        al_draw_tinted_scaled_bitmap(torchImage, al_map_rgb(255, 255, 255),
            offsetx, offsety, 64, 64,
            dx, dy, 2 * TILE_SIZE, 2 * TILE_SIZE,
            0);
    }
    if (status == T_DISAPPEAR) {
        int offsetx = 64 * (attackAnimationTick / (32 / 7));
        int offsety = 256;

        al_draw_tinted_scaled_bitmap(torchImage, al_map_rgb(255, 255, 255),
            offsetx, offsety, 64, 64,
            dx, dy, 2 * TILE_SIZE, 2 * TILE_SIZE,
            0);
    }

   
}

void destroyTorch(void) {
    al_destroy_bitmap(torchImage);
}

static bool validLine(Map* map, Point p0, Point p1) {
    int offsetX[4] = { 0, 0, TILE_SIZE - 1, TILE_SIZE - 1 };
    int offsetY[4] = { 0, TILE_SIZE - 1, 0, TILE_SIZE - 1 };

    for (int i = 0; i < 4; i++) {
        Point n1 = (Point){
            p0.x + offsetX[i],
            p0.y + offsetY[i]
        };

        Point n2 = (Point){
            p1.x + offsetX[i],
            p1.y + offsetY[i]
        };
        if (bresenhamLine(map, n1, n2)) return false;
    }

    return true;
}

static Point shortestPath(Map* map, Point enemy, Point player) {
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
    static int dx[4] = { 1, -1, 0, 0 };
    static int dy[4] = { 0, 0, 1, -1 };
    static DIRECTION move[4] = { UP, DOWN, LEFT, RIGHT }; // To backtrack

    while (front != rear) {
        Point cur = Queue[front++];

        // Found the destiny
        if (cur.x == dst.x && cur.y == dst.y) {
            found = true;
            break;
        };

        for (int i = 0; i < 4; i++) {
            Point next = (Point){
                cur.x + dx[i],
                cur.y + dy[i]
            };

            if (next.x < 0 || next.y < 0) continue;
            if (next.x >= map->row || next.y >= map->col) continue;

            if (isWalkable(map->map[next.x][next.y]) && !visit[next.x][next.y]) {
                dir[next.x][next.y] = move[i];
                visit[next.x][next.y] = true;
                Queue[rear++] = next;
            }
        }
    }

    // Toward a String-Pulling Approach to Path Smoothing on Grid Graphs
    // http://idm-lab.org/bib/abstracts/papers/socs20c.pdf
    if (found) {
        if (validLine(map, enemy, player))
            return findScaledDistance(enemy, player);

        int max_iteration = 1000;
        Point it = dst;
        while (max_iteration--) {
            Point translate_it = (Point){
                it.y * TILE_SIZE,
                it.x * TILE_SIZE
            };

            if (validLine(map, enemy, translate_it))
                return findScaledDistance(enemy, translate_it);

            switch (dir[it.x][it.y]) {
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
    return (Point) { 0, 0 };
}

static bool bresenhamLine(Map* map, Point p0, Point p1) {
    int dx = abs(p1.x - p0.x), sx = p0.x < p1.x ? 1 : -1;
    int dy = -abs(p1.y - p0.y), sy = p0.y < p1.y ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        int tile_x = p0.y / TILE_SIZE;
        int tile_y = p0.x / TILE_SIZE;
        if (!isWalkable(map->map[tile_x][tile_y])) return true;

        if (p0.x == p1.x && p0.y == p1.y) break;
        e2 = 2 * err;

        if (e2 > dy) {
            err += dy;
            p0.x += sx;
        }
        else if (e2 < dx) {
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
        return (Point) { 0, 0 };
    }

    double dxUnit = dx / d;
    double dyUnit = dy / d;

    return (Point) { round(dxUnit), round(dyUnit) };
}