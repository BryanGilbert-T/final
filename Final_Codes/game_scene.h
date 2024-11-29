#ifndef game_scene_h
#define game_scene_h

#include "utility.h"
#include "enemy.h"

// For map.c to get is enemy empty
extern enemyNode * enemyList;

Scene create_game_scene(void);

#endif /* game_scene_h */
