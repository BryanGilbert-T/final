#ifndef game_scene_h
#define game_scene_h

#include "utility.h"
#include "enemy.h"
#include "player.h"

// For map.c to get is enemy empty
extern enemyNode * enemyList;

extern Player player;

Scene create_game_scene(void);
Scene create_winning_scene(void);
Scene create_losing_scene(void);

#endif /* game_scene_h */
