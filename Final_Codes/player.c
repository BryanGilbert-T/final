#include "player.h"
#include "game.h"
#include "utility.h"
#include "map.h"

#include <math.h>

static bool isCollision(Player* player, Map* map);

ALLEGRO_BITMAP* explosion;

Player create_player(char * path, int row, int col, int speed, int health){
    Player player;
    memset(&player, 0, sizeof(player));
    
    player.coord = (Point){
        col * TILE_SIZE,
        row * TILE_SIZE
    };

    player.direction = 0;
    
    player.speed = speed;
    player.health = health;
    player.maxHealth = health;

    char* explosionPath = "Assets/timetravel/explosion.png";
    explosion = al_load_bitmap(explosionPath);
    if (!explosion) {
        game_abort("Cant find %s", explosionPath);
    }

    
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
        if (player->animation_tick >= 64) {
            player->animation_tick = 64 - 1;
        }
        return;
    }
    player->status = PLAYER_IDLE;
    
    // Knockback effect
    if(player->knockback_CD > 0){

        player->knockback_CD--;
        //if (!timetravel) {
            int next_x = player->coord.x + player->speed * cos(player->knockback_angle);
            int next_y = player->coord.y + player->speed * sin(player->knockback_angle);
            Point next;
            next = (Point){ next_x, player->coord.y };

            if (!isCollision(player, map)) {
                player->coord = next;
            }

            next = (Point){ player->coord.x, next_y };
            if (!isCollision(player, map)) {
                player->coord = next;
            }
       // }
       
    }

    if (player->inivisbility_CD) {
        player->inivisbility_CD--;
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
    // To make the plane straight     
    player->direction = UP;

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
        if (player->direction == RIGHT) {
            player->direction = UP;
        }
        else {
            player->direction = LEFT;
        }
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

int flag = 1;
void draw_player(Player * player, Point cam){
    int dy = player->coord.y - cam.y; // destiny y axis
    int dx = player->coord.x - cam.x; // destiny x axis
    
    
    /*
        [TODO Homework] 
        
        Draw Animation of Dying, Walking, and Idle
    */
    
    int framex = 0;
    int framey = 0;

    int red_tint = player->knockback_CD > 0 ? 0 : 255;

    // timetravel 
    if (timetravel) {
        if (player->status == PLAYER_DYING) {
            framex = 16 * (player->animation_tick / (64 / 5));
            al_draw_tinted_scaled_bitmap(player->image, al_map_rgb(255, 0, 0),
                16, 0, 16, 16, // source image x, y, width, height
                dx, dy, TILE_SIZE, TILE_SIZE, // destiny x, y, width, height
                flag // Flip or not
            );
            al_draw_scaled_bitmap(explosion,
                framex, 0, 16, 16,
                dx, dy, TILE_SIZE, TILE_SIZE,
                flag);
            return;
        }
        framex = 16;

        if (player->direction == RIGHT) {
            game_log("right");
            framex = 32;
        }
        else if (player->direction == LEFT) {
            game_log("left");
            framex = 0;
        }
        game_log("%d", framex);
   
        al_draw_tinted_scaled_bitmap(player->image, al_map_rgb(255, red_tint, red_tint),
            framex, framey, 16, 16, // source image x, y, width, height
            dx, dy, TILE_SIZE, TILE_SIZE, // destiny x, y, width, height
            flag // Flip or not
        );

        return;
    }

    
    if (player->direction == RIGHT) {
        if (player_type == PANDA) flag = 1;
        if (player_type == FIONA) flag = 0;
    }
    if (player->direction == LEFT) {
        if (player_type == PANDA) flag = 0;
        if (player_type == FIONA) flag = 1;
    }
    
    switch (player->status) {
        case(PLAYER_IDLE): {
            if (player_type == PANDA) {
                framex = (int)(player->animation_tick / 32);
            }
            
            break;
        }
        case(PLAYER_WALKING): {
            if (player_type == PANDA) {
                framey = 1;
                framex = (int)(player->animation_tick / 16);
            }
            if (player_type == FIONA) {
                framex = (int)(player->animation_tick / (64 / 9));
            }
            break;
        }
        case(PLAYER_DYING): {
            if (player_type == PANDA) {
                framey = 2;
                framex = (int)(player->animation_tick / 16);
            }
            red_tint = 255;
            break;
        }
    }

    int srcx;
    int srcy;

    int src_w;
    int src_h;
    if (player_type == PANDA) {
        src_w = 32;
        src_h = 32;

        srcx = framex * 32;
        srcy = framey * 32;
    }
    if (player_type == FIONA) {
        src_w = 48;
        src_h = 48;

        srcx = framex * 48;
        srcy = framey * 48;
    }
    
    al_draw_tinted_scaled_bitmap(player->image, al_map_rgb(255, red_tint, red_tint),
        srcx, srcy, src_w, src_h, // source image x, y, width, height
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
    al_destroy_bitmap(explosion);
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
    if(player->knockback_CD == 0 && player->inivisbility_CD == 0){
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
        player->inivisbility_CD = 48;

        player->health -= damage; // Should be - enemy->damage

        if (player->health <= 0) {
            player->health = 0;
            player->status = PLAYER_DYING;
            player->animation_tick = 0;
        }


     }
}
