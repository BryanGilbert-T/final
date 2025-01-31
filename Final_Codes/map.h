#ifndef map_h
#define map_h

#include <stdint.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#include "utility.h"

// IF YOU WANT TO INCREASE YOUR MAP SIZE OR ENEMY SIZE MAKE SURE TO INCREASE DEFINE VARIABLE BELOW IF IT EXCEED THE MAXIMUM
#define MAX_MAP_ROW 100
#define MAX_MAP_COL 100
#define MAX_ENEMY_SPAWN 100
#define MAX_DOORS 4

/*
    Map Tiles type
    Hint : Some type might need a background, to configure it try to modify "get_map_offset(Map * map)"
*/
typedef enum _BLOCK_TYPE {
    FLOOR,
    WALL,
    DOOR_CLOSE,
    HOLE,
    COIN,
    NOTHING,
    TROPHY,
    BUTTON,
    DOOR_OPEN,
    S_POTION,
} BLOCK_TYPE;

typedef enum _COIN_STATUS {
    APPEAR,
    DISAPPEARING,
    DISAPPEAR
} COIN_STATUS;

typedef enum _DOOR_STATUS {
    CLOSED,
    OPENING,
    OPEN,
    CLOSING
} DOOR_STATUS;

// Map Struct
typedef struct Map_{
    uint8_t ** map; // Map array
    Point ** offset_assets; // Assets
    
    int row, col;
    
    // Map assets
    ALLEGRO_BITMAP* assets;
    ALLEGRO_BITMAP* coin_assets;

    ALLEGRO_BITMAP* trophy_assets;
    ALLEGRO_BITMAP* potion_assets;
    ALLEGRO_SAMPLE* potion_sample;
   
    // Coin Properties
    ALLEGRO_SAMPLE* coin_audio;
    COIN_STATUS coin_status[MAX_MAP_ROW][MAX_MAP_COL];
    int coin_disappear_animation[MAX_MAP_ROW][MAX_MAP_COL];

    // Trophy Properties
    ALLEGRO_SAMPLE* trophy_audio;

    // Door Properties
    ALLEGRO_BITMAP* door_assets;
    DOOR_STATUS door_status[MAX_MAP_ROW][MAX_MAP_COL];
    int door_animation[MAX_MAP_ROW][MAX_MAP_COL];
    int button_animation[MAX_MAP_ROW][MAX_MAP_COL];
    Point buttons[MAX_DOORS];
    Point door_pairs[MAX_DOORS * 5][2];
    ALLEGRO_SAMPLE* door_audio;
    ALLEGRO_SAMPLE* volcanoBoom;
    ALLEGRO_SAMPLE* earthquake;

    // Button Assets
    ALLEGRO_BITMAP* button_assets;
    
    // Spawn Coordinate
    Point Spawn;
    Point EnemySpawn[MAX_ENEMY_SPAWN];
    char EnemyCode[MAX_ENEMY_SPAWN];
    uint8_t EnemySpawnSize;

    bool win;

    DIRECTION earthquake_dir;
    int richter;

} Map;

/*
    MAP FUNCTION
    Feel free to add more if you have some idea or interaction with the map
 */
Map create_map(char * path, uint8_t type); // Create a map based on given file path
void draw_map(Map * map, Point cam); // Draw the map
void update_map(Map * map, Point player_coord, int * total_coins); // Update map : you might want add some parameter here
void destroy_map(Map * map); // Destroy map
void draw_coins(Map* map, Point cam); // Draw coins and coins only


bool isWalkable(BLOCK_TYPE block);

#endif /* map_h */
