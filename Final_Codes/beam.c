#include <allegro5/allegro.h>
#include <string.h>
#include <stdlib.h>
#include "utility.h"
#include "beam.h"
#include "obstacles.h"

Beam createBeam(char* path, int speed, int damage, Point playerCoord) {
	Beam newBeam;
	newBeam.image = al_load_bitmap(path);
	newBeam.speed = speed;
	newBeam.damage = damage;
	newBeam.coord = playerCoord;
	newBeam.coord.y -= TILE_SIZE / 2;
	return newBeam;
}

bool updateBeam(Beam * beam, Point cam, int speed, obstacleNode* enemyList) {
	beam->coord.y -= beam->speed;
	beam->coord.y -= speed;


	if (beam->coord.y < cam.y) {
		return true;
	}

	
	obstacleNode* cur = enemyList->next;
	while (cur != NULL) {
		 Point enemyCoord = cur->obstacle.coord;
		 if ((beam->coord.x + 32 >= enemyCoord.x && beam->coord.y + 32 >= enemyCoord.y) &&
			 (beam->coord.x + 32 <= enemyCoord.x + TILE_SIZE && beam->coord.y + 32 <= enemyCoord.y + TILE_SIZE) &&
			 (beam->coord.x + 32 >= enemyCoord.x && beam->coord.y + 32 <= enemyCoord.y + TILE_SIZE) &&
			 (beam->coord.x + 32 <= enemyCoord.x + TILE_SIZE && beam->coord.y + 32 >= enemyCoord.y)) {
			 if (cur->obstacle.status != OBSDYING) {
				 hitObstacle(cur, beam->damage);
				 return true;
			 }
		 }
		 else if ((beam->coord.x >= enemyCoord.x && beam->coord.y >= enemyCoord.y) &&
			 (beam->coord.x <= enemyCoord.x + TILE_SIZE && beam->coord.y <= enemyCoord.y + TILE_SIZE) &&
			 (beam->coord.x >= enemyCoord.x && beam->coord.y <= enemyCoord.y + TILE_SIZE) &&
			 (beam->coord.x <= enemyCoord.x + TILE_SIZE && beam->coord.y >= enemyCoord.y)) {
			 if (cur->obstacle.status != OBSDYING) {
				 hitObstacle(cur, beam->damage);
				 return true;
			 }
		 }
		 else if ((beam->coord.x + TILE_SIZE >= enemyCoord.x && beam->coord.y + TILE_SIZE >= enemyCoord.y) &&
			 (beam->coord.x + TILE_SIZE <= enemyCoord.x + TILE_SIZE && beam->coord.y + TILE_SIZE <= enemyCoord.y + TILE_SIZE) &&
			 (beam->coord.x + TILE_SIZE >= enemyCoord.x && beam->coord.y + TILE_SIZE <= enemyCoord.y + TILE_SIZE) &&
			 (beam->coord.x + TILE_SIZE <= enemyCoord.x + TILE_SIZE && beam->coord.y + TILE_SIZE >= enemyCoord.y)) {
			 if (cur->obstacle.status != OBSDYING) {
				 hitObstacle(cur, beam->damage);
				 return true;
			 }
		 }
			  cur = cur->next;
	}
	
	return false;
}

void drawBeam(Beam beam, Point cam) {
	int dx = beam.coord.x - cam.x;
	int dy = beam.coord.y - cam.y;

	al_draw_scaled_bitmap(beam.image,
		0, 0, 16, 16,
		dx, dy, TILE_SIZE, TILE_SIZE,
		0);
}

void deleteBeam(Beam beam) {
	al_destroy_bitmap(beam.image);
}


BeamNode* createBeamNode(void) {
	BeamNode* dummyhead = malloc(sizeof(BeamNode));
	//dummyhead->beam;
	dummyhead->next = NULL;
	return dummyhead;
}

void insertBeamNode(BeamNode* head, Beam beam) {
	BeamNode* newNode = malloc(sizeof(BeamNode));
	newNode->beam = beam;
	newNode->next = head->next;
	head->next = newNode;
}

void updateBeamNode(BeamNode* beamNode, Point cam, int speed, obstacleNode* enemyList) {
	BeamNode* prev = beamNode;
	BeamNode* cur = beamNode->next;
	int i = 0;
	while (cur != NULL) {
		bool shouldDelete = updateBeam(&cur->beam, cam, speed, enemyList);
		if (shouldDelete) {
			BeamNode* temp = cur;
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

void drawBeamNode(BeamNode* beamNode, Point cam) {
	BeamNode* prev = beamNode;
	BeamNode* cur = beamNode->next;
	while (cur != NULL) {
		drawBeam(cur->beam , cam);
		prev = cur;
		cur = cur->next;
	}
}

void deleteBeamNode(BeamNode* beamNode) {
	BeamNode* cur = beamNode->next;
	while (cur != NULL) {
		BeamNode* temp = cur;
		cur = cur->next;
		deleteBeam(temp->beam);
		free(temp);
	}
	free(beamNode);
}