#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

#include "map.h"
#include "game_scene.h"
#include "cut_scene.h"

/*
    [OFFSET CALCULATOR FUNCTIONS]
      You might want to modify this functions if you are using different assets
      In short, this functions help us to find which asset to draw
 */
static void get_map_offset(Map * map);
static Point get_wall_offset_assets(Map * map, int i, int j);
static Point get_floor_offset_assets(Map * map, int i, int j);
static Point get_hole_offset_assets(Map * map, int i, int j);
static const int offset = 16;

static int coin_animation = 0;
static int trophy_animation = 0;

int button_counter;
int door_counter;

static bool tile_collision(Point player, Point tile_coord);
void fix_pairs(Point buttons[MAX_DOORS], Point door_pairs[MAX_DOORS * 5][2]);

Map create_map(char * path, uint8_t type){
    Map map;

    button_counter = 0;
    door_counter = 0;

    memset(&map, 0, sizeof(Map));
    
    FILE * f = fopen(path, "r");
    if(!f){
        game_abort("Can't load map file : %s", path);
    }
    
    game_log("Using %s to load the map", path);
    
    fscanf(f, "%d %d", &map.row, &map.col);
    char ch;
    
    // Map Array
    map.map = (uint8_t **) malloc(map.row * sizeof(uint8_t *));
    for(int i=0; i<map.row; i++){
        map.map[i] = (uint8_t *) malloc(map.col * sizeof(uint8_t));
    }
    
    // Map Offset Resource Pack Array
    map.offset_assets = (Point **) malloc(map.row * sizeof(Point *));
    for(int i=0; i<map.row; i++){
        map.offset_assets[i] = (Point *) malloc(map.col * sizeof(Point));
    }

    int coin_counter = 0;
    
    // Scan the map to the array
    for(int i = 0; i < map.row; i++){
        for(int j = 0; j < map.col; j++){
            fscanf(f, " %c", &ch);
            
            switch(ch)
            {
                case '#': // Case Wall
                    map.map[i][j] = WALL;
                    break;

                case '.': // Case Floor
                    map.map[i][j] = FLOOR;
                    break;

                case 'P': // Spawn Point
                    map.map[i][j] = FLOOR;
                    map.Spawn = (Point){i, j};
                    break;

                case 'S': // Slime Enemy
                    map.map[i][j] = FLOOR;
                    map.EnemyCode[map.EnemySpawnSize] = ch;
                    map.EnemySpawn[map.EnemySpawnSize++] = (Point){i, j};
                    break;

                case 'F':
                    map.map[i][j] = FLOOR;
                    map.EnemyCode[map.EnemySpawnSize] = ch;
                    map.EnemySpawn[map.EnemySpawnSize++] = (Point){ i, j };
                    break;

                case 'L':
                    map.map[i][j] = FLOOR;
                    map.EnemyCode[map.EnemySpawnSize] = ch;
                    map.EnemySpawn[map.EnemySpawnSize++] = (Point){ i, j };
                    break;

                case 'O':
                    map.map[i][j] = FLOOR;
                    map.EnemyCode[map.EnemySpawnSize] = ch;
                    map.EnemySpawn[map.EnemySpawnSize++] = (Point){ i, j };
                    break;

                case 'A':
                    map.map[i][j] = FLOOR;
                    map.EnemyCode[map.EnemySpawnSize] = ch;
                    map.EnemySpawn[map.EnemySpawnSize++] = (Point){ i, j };
                    break;

                case 'C': // Coins
                    map.map[i][j] = COIN;
                    map.coin_status[i][j] = APPEAR;
                    coin_counter++;
                    break;

                case '_': // Nothing
                    map.map[i][j] = HOLE;
                    break;

                case 'T': //Trophy
                    map.map[i][j] = TROPHY;
                    break;

                default: // If not determined, set it as black
                    if (isdigit(ch)) {
                        if ((ch - '0') % 2 == 0) {
                            map.map[i][j] = BUTTON;
                            int idx = ((ch - '0') / 2) - 1;
                            map.buttons[idx] = (Point){ i, j };
                            map.button_animation[i][j] = 0;
                            button_counter++;
                        }
                        else {
                            map.map[i][j] = DOOR_CLOSE;
                            map.door_status[i][j] = CLOSED;
                            int button_pair = ch - '0' + 1;
                            map.door_pairs[door_counter][0] = (Point){ button_pair, button_pair };
                            map.door_pairs[door_counter][1] = (Point){ i, j };
                            map.door_animation[i][j] = 0;
                            door_counter++;
                        }
                        break;
                    }
                    map.map[i][j] = NOTHING;
                    break;
            }
        }
    }
    
    if (jurassic) {
        map.assets = al_load_bitmap("Assets/map_packets.png");
        if (!map.assets) {
            game_abort("Cant load map assets");
        }
    }
    else {
        map.assets = al_load_bitmap("Assets/map_packets.png");
        if (!map.assets) {
            game_abort("Can't load map assets");
        }
    }
   
    map.coin_assets = al_load_bitmap("Assets/coins.png");
    if (!map.coin_assets) {
        game_abort("Can't load coin assets");
    }
    
    // load the offset for each tiles type
    get_map_offset(&map);

    map.coin_audio = al_load_sample("Assets/audio/coins.mp3");
    if(!map.coin_audio){
        game_abort("Can't load coin audio");
    }

    // Trophy properties

    map.trophy_assets = al_load_bitmap("Assets/trophy.png");
    if (!map.trophy_assets) {
        game_abort("Can't load trophy assets");
    }

    map.trophy_audio = al_load_sample("Assets/audio/win.mp3");
    if (!map.trophy_audio) {
        game_abort("Can't load trophy audio");
    }

    // Door Assets
    map.door_assets = al_load_bitmap("Assets/Door.png");
    if (!map.door_assets) {
        game_abort("Can't load door assets");
    }

    // Button Assets
    map.button_assets = al_load_bitmap("Assets/Button.png");
    if (!map.button_assets) {
        game_abort("Can't load button assets");
    }

    // Not win
    map.win = false;

    fix_pairs(map.buttons, map.door_pairs);

    fclose(f);
    
    return map;
}

void fix_pairs(Point buttons[MAX_DOORS], Point door_pairs[MAX_DOORS * 5][2]) {
    for (int i = 0; i < door_counter; i++) {
        int idx = (door_pairs[i][0].x / 2) - 1;
        door_pairs[i][0] = buttons[idx];
    }
}

void draw_coins(Map* map, Point cam) {

    for (int i = 0; i < map->row; i++) {
        for (int j = 0; j < map->col; j++) {
            int dy = i * TILE_SIZE - cam.y; // destiny y axis
            int dx = j * TILE_SIZE - cam.x; // destiny x axis

            int src_coin_width = 16;
            int src_coin_height = 16;
            switch (map->map[i][j]) {
                case COIN: {
                    int offsetx = 0;
                    int offsety = 0;
                    if (map->coin_status[i][j] == APPEAR) {
                        offsetx = src_coin_width * (int)(coin_animation / 8);
                    }
                    else if (map->coin_status[i][j] == DISAPPEARING) {
                        offsetx = src_coin_width * (int)(map->coin_disappear_animation[i][j] / 8);
                        offsety = 1 * src_coin_height;
                        map->coin_disappear_animation[i][j] += 1;
                        if (map->coin_disappear_animation[i][j] == 64) {
                            map->coin_status[i][j] = DISAPPEAR;
                        }
                    }
                    else if (map->coin_status[i][j] == DISAPPEAR) {
                        map->map[i][j] = FLOOR;
                        break;
                    }
                    al_draw_scaled_bitmap(map->coin_assets,
                        offsetx, offsety, src_coin_width, src_coin_height,
                        dx, dy, TILE_SIZE, TILE_SIZE,
                        0);
                    break;
                }
                default:
                    break;
            }
        }
    }
       
}

void draw_map(Map * map, Point cam){
    // Draw map based on the camera point coordinate
    al_clear_to_color(al_map_rgb(24, 20, 37));   
   

    for(int i=0; i<map->row; i++){
        for(int j=0; j<map->col; j++){
            
            int dy = i * TILE_SIZE - cam.y; // destiny y axis
            int dx = j * TILE_SIZE - cam.x; // destiny x axis
            
            Point offset_asset = map->offset_assets[i][j];
            
            al_draw_scaled_bitmap(map->assets, // image
                                  offset_asset.x, offset_asset.y, 16, 16, // source x, source y, width, height
                                  dx, dy, TILE_SIZE, TILE_SIZE, // destiny x, destiny y, destiny width, destiny height
                                  0 // flag : set 0
                                  );

            int src_coin_width = 16;
            int src_coin_height = 16;
            switch(map->map[i][j]){
                case COIN: {
                    int offsetx = 0;
                    int offsety = 0;
                    if (map->coin_status[i][j] == APPEAR) {
                        offsetx = src_coin_width * (int)(coin_animation / 8);
                    }
                    else if (map->coin_status[i][j] == DISAPPEARING) {
                        offsetx = src_coin_width * (int)(map->coin_disappear_animation[i][j] / 8);                        
                        offsety = 1 * src_coin_height;
                        map->coin_disappear_animation[i][j] += 1;
                        if (map->coin_disappear_animation[i][j] == 64) {
                            map->coin_status[i][j] = DISAPPEAR;
                        }
                    }
                    else if (map->coin_status[i][j] == DISAPPEAR) {
                        map->map[i][j] = FLOOR;
                        break;
                    }
                    al_draw_scaled_bitmap(map->coin_assets,
                        offsetx, offsety, src_coin_width, src_coin_height,
                        dx, dy, TILE_SIZE, TILE_SIZE,
                        0);
                    break;
                }
                case TROPHY: {
                    int offsetx = 32 * (int)(trophy_animation / (64 / 9));
                    if (offsetx > 32 * 9) {
                        offsetx = 0;
                    }
                    int offsety = 0;

                    // All enemy dead? Change trophy color
                    int tinted_color = /*(enemyList->next != NULL) ? 155 :*/ 255;
                    al_draw_tinted_scaled_bitmap(map->trophy_assets,
                        al_map_rgb(tinted_color, tinted_color, tinted_color),
                        offsetx, offsety, 32, 32,
                        dx, dy, TILE_SIZE, TILE_SIZE,
                        0);
                    break;
                }
                case DOOR_CLOSE: {
                    int offsetx = 32 * (int)(map->door_animation[i][j] / (64 / 6));
                    int offsety = 0;

                    if (offsetx > 32 * 6) {
                        map->door_status[i][j] = OPEN;
                        map->map[i][j] = DOOR_OPEN;
                        map->door_animation[i][j] = 0;
                        offsetx = 32 * 6 - 1;
                    }

                    al_draw_scaled_bitmap(map->door_assets,
                        offsetx, offsety, 32, 16,
                        dx, dy, TILE_SIZE, TILE_SIZE,
                        0);
                    break;
                }

                case DOOR_OPEN: {
                    int offsetx = 32 * (int)(map->door_animation[i][j] / (64 / 6));
                    int offsety = 16;
                   
                    if (offsetx > 32 * 6) {
                        map->door_status[i][j] = CLOSED;
                        map->map[i][j] = DOOR_CLOSE;
                        map->door_animation[i][j] = 0;
                        offsetx = 32 * 6 - 1;
                    }

                    al_draw_scaled_bitmap(map->door_assets,
                        offsetx, offsety, 32, 16,
                        dx, dy, TILE_SIZE, TILE_SIZE,
                       0);
                    break;
                }
                case BUTTON: {
                    int offsetx = 16 * (int)(map->button_animation[i][j] / (32 / 2));
                    int offsety = 0;
                  
                    al_draw_scaled_bitmap(map->button_assets,
                        offsetx, offsety, 16, 16,
                        dx, dy, TILE_SIZE, TILE_SIZE,
                        0);
                    break;
                }
                default:
                    break;
            }
        #ifdef DRAW_HITBOX
            al_draw_rectangle(
                dx, dy, dx + TILE_SIZE, dy + TILE_SIZE,
                al_map_rgb(0, 255, 0), 1
            );
        #endif
        }
    }
}

void update_map(Map * map, Point player_coord, int* total_coins){
    /*
        Hint: To check if it's collide with object in map, you can use tile_collision function
        e.g. to update the coins if you touch it
    */
    coin_animation = (coin_animation + 1) % 64;
    trophy_animation = (trophy_animation + 1) % (64 * 2);
        
    for (int i = 0; i < door_counter; i++) {
        Point button = map->door_pairs[i][0];
        Point door = map->door_pairs[i][1];
        if (map->door_status[door.x][door.y] == OPENING) {
            map->button_animation[button.x][button.y] = map->button_animation[button.x][button.y] + 1;
            if (map->button_animation[button.x][button.y] > 32) {
                map->button_animation[button.x][button.y] = 32;
            }
            map->door_animation[door.x][door.y] = map->door_animation[door.x][door.y] + 1;
        }
        if (map->door_status[door.x][door.y] == CLOSING) {
            map->door_animation[door.x][door.y] = map->door_animation[door.x][door.y] + 1;
        }
    }

    int center_x = (int)((player_coord.x + (int)(TILE_SIZE / 2)) / TILE_SIZE);
    int center_y = (int)((player_coord.y + (int)(TILE_SIZE / 2)) / TILE_SIZE);

    if (map_number == 1) {
        if (center_y == 11 && 
            (center_x >= 35 && center_x <= 39) && 
            boss_fight == false) {
            map->door_status[13][36] = CLOSING;
            map->door_status[13][37] = CLOSING;
            map->door_status[13][38] = CLOSING; 

            boss_fight = true;
            inCutscene = true;
            initCutscene(7);
        }
    }
    
    if (map_number == 3) {
        if (center_y == 10 &&
            (center_x >= 10 && center_x <= 14) &&
            boss_fight == false) {
            map->door_status[12][11] = CLOSING;
            map->door_status[12][12] = CLOSING;
            map->door_status[12][13] = CLOSING;

            boss_fight = true;
            inCutscene = true;
            initCutscene(8);
        }
    }

    if (map->map[center_y][center_x] == COIN &&
        map->coin_status[center_y][center_x] == APPEAR) {
        *total_coins += 1;
        map->coin_disappear_animation[center_y][center_x] = 0;
        map->coin_status[center_y][center_x] = DISAPPEARING;
        al_play_sample(map->coin_audio, SFX_VOLUME, 0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    }

    // Center is trophy and enemy all dead
    if (map->map[center_y][center_x] == TROPHY && map->win != true) {
        al_play_sample(map->trophy_audio, SFX_VOLUME, 0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        map->win = true;
    }

    if (map->map[center_y][center_x] == BUTTON) {
        for (int i = 0; i < door_counter; i++) {
            Point door_coord;
            if (map->door_pairs[i][0].x == center_y && map->door_pairs[i][0].y == center_x) {
                door_coord.x = map->door_pairs[i][1].x;
                door_coord.y = map->door_pairs[i][1].y;
                if (map->door_status[door_coord.x][door_coord.y] == CLOSED) {
                    map->door_status[door_coord.x][door_coord.y] = OPENING;
                }
                
            }
        }
    }
}

void destroy_map(Map * map){
    for(int i = 0; i < map->row; i++){
        free(map->map[i]);
        free(map->offset_assets[i]);
    }
    free(map->map);
    free(map->offset_assets);

    al_destroy_bitmap(map->assets);
    al_destroy_bitmap(map->coin_assets);
    al_destroy_sample(map->coin_audio);
}

bool isWalkable(BLOCK_TYPE block){
    if(block == FLOOR ||  block == COIN || block == TROPHY || block == BUTTON || block == DOOR_OPEN) return true;
    return false;
}

/*
    DON'T CHANGE CODE BELOW UNLESS YOU ARE UNDERSTAND WELL
    OR WANT TO CHANGE TO DIFFERENT ASSETS
 */
static bool isWall(Map * map, int i, int j);
//static bool isFloorHole(Map * map, int i, int j);
static bool isFloor(Map * map, int i, int j);
static bool isNothing(Map * map, int i, int j);
static bool isDoor(Map* map, int i, int j);

static Point get_floor_offset_assets(Map * map, int i, int j){
    bool up = isWall(map, i-1, j);
    bool left = isWall(map, i, j-1);
    bool right = isWall(map, i, j+1);
    bool top_left = isWall(map, i-1, j-1);
    bool top_right = isWall(map, i-1, j+1);
    
    if(up && left && right){
        return (Point){ offset * 12, offset * 3 };
    }
    if(up && left){
        return (Point){ offset * 11, 0 };
    }
    if(up && right){
        return (Point){ offset * 13, 0 };
    }
    if(left){
        if(top_left)
            return (Point){ offset * 11, offset * 1 };
        else
            return (Point){ offset * 15, offset * 1 };
        
    }
    if(right){
        if(top_right)
            return (Point){ offset * 13, offset * 1 };
        else
            return (Point){ offset * 14, offset * 1 };
    }
    if(up){
        return (Point){ offset * 12, 0 };
    }
    if(top_left){
        return (Point){ offset * 11, offset * 3 };
    }
    if(top_right){
        return (Point){ offset * 13, offset * 3 };
    }
    
    return (Point){12 * offset, 4 * offset};
}   

// Calculate the offset from the source assets
static Point get_wall_offset_assets(Map* map, int i, int j) {
    bool up = isWall(map, i - 1, j);
    bool down = isWall(map, i + 1, j);
    bool left = isWall(map, i, j - 1);
    bool right = isWall(map, i, j + 1);

    bool upD = isDoor(map, i - 1, j);
    bool downD = isDoor(map, i + i, j);
    bool leftD = isDoor(map, i, j - 1);
    bool rightD = isDoor(map, i, j + 1);

    up = up || upD;
    down = down || downD;
    left = left || leftD;
    right = right || rightD;

    if(up && down && left && right){
        return (Point){ 3 * offset, 5 * offset };
    }
    if(up && down && right){
        bool left_floor = isFloor(map, i, j - 1);
        bool right_down = isWall(map, i + 1, j + 1);
        bool right_up = isWall(map, i - 1, j + 1);
        if (right_down && right_up)
            return (Point) { 2 * offset, 5 * offset };
        if (left_floor) {
            return (Point) { 1 * offset, 3 * offset };
        }
        else {
            return (Point) { 4 * offset, 5 * offset };
        }
    }
    if (up && down && left) {
        bool right_floor = isFloor(map, i, j + 1);
        bool left_down = isWall(map, i + 1, j - 1);
        bool left_up = isWall(map, i - 1, j - 1);
        if (left_down && left_up)
            return (Point) { 4 * offset, 5 * offset };
        if (right_floor) {
            return (Point) { 1 * offset, 3 * offset };
        }
        else {
            return (Point) { 2 * offset, 5 * offset };
        }
    }
    if(down && right && left){
        bool down_right_wall = isWall(map, i + 1, j + 1);
        bool down_left_wall = isWall(map, i + 1, j - 1);
        bool down_right = isFloor(map, i+1, j+1);
        bool down_left = isFloor(map, i+1, j-1);
        if(down_right_wall && down_left_wall)
            return (Point) { 3 * offset, 4 * offset };
        if((down_right ^ down_left) == 0)
            return (Point){ 8 * offset, 5 * offset };
        if(down_right)
            return (Point){ 4 * offset, 4 * offset };
        if(down_left)
            return (Point){ 2 * offset, 4 * offset };
    }
    if(left && right){
        if(isFloor(map, i+1, j))
            return (Point){ 7 * offset, 5 * offset};
        else
            return (Point){ 7 * offset, 7 * offset};
    }
    if(down && up){
        bool left_ = isFloor(map, i, j-1);
        bool right_ = isFloor(map, i, j+1);
        if((left_ ^ right_) == 0)
            return (Point){ 1 * offset, 3 * offset};
        if(left_)
            return (Point){ 2 * offset, 5 * offset};
        return (Point){ 4 * offset, 5 * offset};
    }
    if(down && right){
        if(isFloor(map, i+1, j+1))
            return (Point){ 4 * offset, 5 * offset };
        else
            return (Point){ 2 * offset, 4 * offset };
    }
    if(down && left){
        if (isFloor(map, i, j + 1) && isFloor(map, i + 1, j - 1))
            return (Point) { 1 * offset, 2 * offset };
        else if(isFloor(map, i+1, j-1))
            return (Point){ 2 * offset, 5 * offset };
        else
            return (Point){ 4 * offset, 4 * offset};
    }
    if(up && right){
        if(isFloor(map, i+1, j))
            return (Point){ 2 * offset, 6 * offset};
        else
            return (Point){ 5 * offset, 6 * offset};
    }
    if(up && left){
        if(isFloor(map, i+1, j))
            return (Point){ 4 * offset, 6 * offset};
        else
            return (Point){ 6 * offset, 6 * offset};
    }
    if(left || right){
        if(isFloor(map, i+1, j))
            return (Point){ 7 * offset, 5 * offset};
        else
            return (Point){ 7 * offset, 7 * offset};
    }
    if(down){
        return (Point){ 1 * offset, 2 * offset};
    }
    if(up){
        return (Point){ 1 * offset, 4 * offset};
    }
    
    return (Point){ 0, 0 };
}

static Point get_hole_offset_assets(Map * map, int i, int j){
    bool up = isNothing(map, i-1, j);
    bool U_Floor = isFloor(map, i-1, j);
    
    if(up){
        return (Point){ 0, 0 };
    }
    if(U_Floor){
        bool UL = isFloor(map, i-1, j-1);
        bool UR = isFloor(map, i-1, j+1);
        if(UL && UR) return (Point){ offset * 13, offset * 11 };
        if(UL) return (Point){ offset * 14, offset * 11 };
        if(UR) return (Point){ offset * 12, offset * 11 };
        return (Point){ offset * 10, offset * 12 };
    }
    return (Point){ 0, 0 };
}

static Point get_nothing_offset_assets(Map * map, int i, int j){
    bool U_Floor = isFloor(map, i-1, j);
    if(U_Floor){
        bool UL = isFloor(map, i-1, j-1);
        bool UR = isFloor(map, i-1, j+1);
        if(UL && UR) return (Point){ offset * 13, offset * 11 };
        if(UL) return (Point){ offset * 14, offset * 11 };
        if(UR) return (Point){ offset * 12, offset * 11 };
        return (Point){ offset * 10, offset * 12 };
    }
    return (Point){ 0, 0 };
}

static void get_map_offset(Map * map){
    // Calculate once to use it later on when draw() function called
    for(int i = 0; i < map->row; i++){
        for(int j = 0; j < map->col; j++){
            switch(map->map[i][j]){
                case WALL:
                    map->offset_assets[i][j] = get_wall_offset_assets(map, i, j);
                    break;
                case FLOOR:
                case COIN:
                    map->offset_assets[i][j] = get_floor_offset_assets(map, i, j);
                    break;

                case TROPHY:
                    map->offset_assets[i][j] = get_floor_offset_assets(map, i, j);
                    break;

                case DOOR_OPEN:
                    map->offset_assets[i][j] = get_floor_offset_assets(map, i, j);
                    break;

                case DOOR_CLOSE:
                    map->offset_assets[i][j] = get_floor_offset_assets(map, i, j);
                    break;

                case HOLE:
                    map->offset_assets[i][j] = get_hole_offset_assets(map, i, j);
                    break;

                case NOTHING:
                default:
                    map->offset_assets[i][j] = (Point){ 0, 0 };
                    break;
            }
        }
    }
}

static bool isWall(Map * map, int i, int j){
    if(i < 0 || j < 0 || i >= map->row || j >= map->col) return false;
    if(map->map[i][j] == WALL) return true;
    return false;
}

static bool isDoor(Map* map, int i, int j) {
    if (i < 0 || j < 0 || i >= map->row || j >= map->col) return false;
    if (map->map[i][j] == DOOR_CLOSE) return true;
    return false;
}

static bool isFloor(Map * map, int i, int j){
    if(i < 0 || j < 0 || i >= map->row || j >= map->col) return false;
    if(map->map[i][j] == FLOOR) return true;
    return false;
}

static bool isNothing(Map * map, int i, int j){
    if(i < 0 || j < 0 || i >= map->row || j >= map->col) return true;
    if(map->map[i][j] == NOTHING || map->map[i][j] == HOLE) return true;
    return false;
}

/*
    Collision
 */
static bool tile_collision(Point player, Point tile_coord){
    
    if (player.x < tile_coord.x + TILE_SIZE &&
        player.x + TILE_SIZE > tile_coord.x &&
        player.y < tile_coord.y + TILE_SIZE &&
        player.y + TILE_SIZE > tile_coord.y) {
            return true; // Rectangles collide
    } else {
        return false; // Rectangles do not collide
    }
}
