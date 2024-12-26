#ifndef torch_h
#define torch_h

#include "player.h"
#include "enemy.h"

void initTorch(void);
void updateTorch(Point playerCoord, enemyNode * enemyList);
void drawTorch(Point cam);
void destroyTorch(void);

extern bool torch;

typedef enum TorchStatus {
	T_APPEAR,
	T_IDLE,
	T_ATTACK,
	T_ATTACKING,
	T_DISAPPEAR,
} TorchStatus;

#endif