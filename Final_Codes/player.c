#include "player.h"
#include "game.h"
#include "utility.h"
#include "map.h"

#include <math.h>

static bool isCollision(Player* player, Map* map);

Player create_player(char * path, int row, int col){
    Player player;
    memset(&player, 0, sizeof(player));
    
    player.coord = (Point){
        col * TILE_SIZE,
        row * TILE_SIZE
    };

    player.direction = 0;
    
    player.speed = 4;
    player.health = 50;

    
    player.image = al_load_bitmap(path);
    if(!player.image){
        game_abort("Error Load Bitmap with path : %s", path);
    }
    
    return player;
}

void update_player(Player * player, Map* map){

    Point original = player->coord;
    if (player->status == PLAYER_DYING) {
        player->animation_tick += 1;
        if (player->animation_tick >= 64 + 16) {
            player->animation_tick = 64 + 16;
        }
        return;
    }
    player->status = PLAYER_IDLE;
    
    // Knockback effect
    if(player->knockback_CD > 0){

        player->knockback_CD--;
        int next_x = player->coord.x + player->speed * cos(player->knockback_angle);
        int next_y = player->coord.y + player->speed * sin(player->knockback_angle);
        Point next;
        next = (Point){next_x, player->coord.y};
        
        if(!isCollision(player, map)){
            player->coord = next;
        }
        
        next = (Point){player->coord.x, next_y};
        if(!isCollision(player, map)){
            player->coord = next;
        }
    }
    
    /*
        [TODO HACKATHON 1-1] 
        
        Player Movement
        Adjust the movement by player->speed

        if (keyState[ALLEGRO_KEY_W]) {
            player->coord.y = ...
            player->direction = ...
        }
        if (keyState[ALLEGRO_KEY_S]) {
            player->coord.y = ...
            player->direction = ...
        }
    */
    if (keyState[UP_KEY]) {
        player->coord.y -= player->speed;
        player->direction = UP;
        player->status = PLAYER_WALKING;
    }
    if (keyState[DOWN_KEY]) {
        player->coord.y += player->speed;
        player->direction = DOWN;
        player->status = PLAYER_WALKING;
    }
    //-----------------------------------
    
    // if Collide, snap to the grid to make it pixel perfect
    if(isCollision(player, map)){
        player->coord.y = round((float)original.y / (float)TILE_SIZE) * TILE_SIZE;
    }
    
    /*
        [TODO HACKATHON 1-2] 
        
        Player Movement
        Add/Subtract the movement by player->speed

        if (keyState[ALLEGRO_KEY_A]) {
            player->coord.y = ...
            player->direction = ...
        }
        if (keyState[ALLEGRO_KEY_D]) {
            player->coord.y = ...
            player->direction = ...

    }
    */
    if (keyState[RIGHT_KEY]) {
        player->coord.x += player->speed;
        player->direction = RIGHT;
        player->status = PLAYER_WALKING;
    }
    if (keyState[LEFT_KEY]) {
        player->coord.x -= player->speed;
        player->direction = LEFT;
        player->status = PLAYER_WALKING;
    }
    //-----------------------------
    
    if(isCollision(player, map)){
        player->coord.x = round((float)original.x / (float)TILE_SIZE) * TILE_SIZE;
    }
    
    /*
        [TODO Homework]  

        Calculate the animation tick to draw animation later
    */
    player->animation_tick = (player->animation_tick + 1) % 64;
    
}

void draw_player(Player * player, Point cam){
    int dy = player->coord.y - cam.y; // destiny y axis
    int dx = player->coord.x - cam.x; // destiny x axis
    
    int flag = player->direction % 2;
    
    
    /*
        [TODO Homework] 
        
        Draw Animation of Dying, Walking, and Idle
    */
    
    int framex = 0;
    int framey = 0;
    
    switch (player->status) {
        case(PLAYER_IDLE): {
                if (player->animation_tick > 32) {
                    framex = (framex + 1) % 2;
                }
                break;
            }
        case(PLAYER_WALKING): {
            framey = 1;
            if (player->animation_tick > 16) {
                framex = 1;
            }
            if (player->animation_tick > 32) {
                framex = 2;
            }
            if (player->animation_tick > 48) {
                framex = 3;
            }
            break;
        }
        case(PLAYER_DYING): {
            framey = 2;
            if (player->animation_tick > 16) {
                framex = 1;
            }
            if (player->animation_tick > 32) {
                framex = 2;
            }
            if (player->animation_tick > 48) {
                framex = 3;
            }
            break;
        }
    }

    int srcx = framex * 32;
    int srcy = framey * 32;
    
    al_draw_tinted_scaled_bitmap(player->image, al_map_rgb(255, 255, 255),
        srcx, srcy, 32, 32, // source image x, y, width, height
        dx, dy, TILE_SIZE, TILE_SIZE, // destiny x, y, width, height
        flag // Flip or not
    );

    
#ifdef DRAW_HITBOX
    al_draw_rectangle(
        dx, dy, dx + TILE_SIZE, dy + TILE_SIZE,
        al_map_rgb(255, 0, 0), 1
    );
#endif
    
}

void delete_player(Player * player){
    al_destroy_bitmap(player->image);
}

static bool isCollision(Player* player, Map* map){

    if (player->coord.x < 0 ||
        player->coord.y < 0 ||
        (player->coord.x + TILE_SIZE - 1) / TILE_SIZE >= map->col ||
        (player->coord.y + TILE_SIZE - 1) / TILE_SIZE >= map->row)
        return true;

    /*

        [TODO HACKATHON 2-1] 
    
        Check every corner of enemy if it's collide or not

        We check every corner if it's collide with the wall/hole or not
        You can activate DRAW_HITBOX in utility.h to see how the collision work

        if(!isWalkable(map->map[...][...])) return true;
        if(!isWalkable(map->map[...][...])) return true;
        if(!isWalkable(map->map[...][...])) return true;
        if(!isWalkable(map->map[...][...])) return true;

    */  

    int tileX1 = player->coord.x / TILE_SIZE;
    int tileY1 = player->coord.y / TILE_SIZE;

    int tileX2 = (player->coord.x + TILE_SIZE - 1) / TILE_SIZE;
    int tileY2 = (player->coord.y + TILE_SIZE - 1) / TILE_SIZE;

    if (!isWalkable(map->map[tileY1][tileX1])) return true;
    if (!isWalkable(map->map[tileY2][tileX2])) return true;
    if (!isWalkable(map->map[tileY2][tileX1])) return true;
    if (!isWalkable(map->map[tileY1][tileX2])) return true;

    return false;
}

void hitPlayer(Player * player, Point enemy_coord, int damage){
    if(player->knockback_CD == 0){
        float dY = player->coord.y - enemy_coord.y;
        float dX = player->coord.x - enemy_coord.x;
        float angle = atan2(dY, dX);

        /*
            [TODO Homework]
            
            Game Logic when the player get hit or die

            player->knockback_angle = angle;
            player->knockback_CD = 32;

            player->health = ...
            if(player->health <= 0){

                player->health = 0;
                player->status = ...

                ...
            }
        */
        player->knockback_angle = angle;
        player->knockback_CD = 32;

        player->health -= 10; // Should be - enemy->damage

        if (player->health <= 0) {
            player->health = 0;
            player->status = PLAYER_DYING;
            player->animation_tick = 0;
        }


     }
}
