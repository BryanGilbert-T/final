#ifndef obstacles_h
#define obstacles_h

#include "player.h"

typedef enum _obstacle_status {
	OBSALIVE,
	OBSDYING,
} OBSTACLE_STATUS;

typedef enum _obstacle_type {
	BONBON,
	ALAN,
	LIPS,
} OBSTACLE_TYPE;

typedef struct _obstacle {
	int health;
	int damage;
	Point coord;
	int animation_tick;
	int death_animation_tick;
	int hit_animation_tick;
	int attackCD;

	OBSTACLE_STATUS status;
	OBSTACLE_TYPE type;
	ALLEGRO_BITMAP* image;
} Obstacle;

typedef struct _obstacleNode {
	Obstacle obstacle;
	struct _obstacleNode* next;
} obstacleNode;

void init_obstacle(void);
Obstacle create_obstacle(int row, int col, char ch);
void hitObstacle(Obstacle* obstacle, int damage);
obstacleNode* createObstacleNode(void);
void insertObstacleNode(obstacleNode* head, Obstacle obstacle);
void drawObstacleNode(obstacleNode* head, Point cam);
void updateObstacleNode(obstacleNode * head, Player * player, Map * map);
void destroyObstacleNode(obstacleNode* head);
void destroyObstacle(void);

#endif