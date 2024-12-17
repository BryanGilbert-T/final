#include <allegro5/allegro.h>
#include <string.h>
#include <stdlib.h>
#include "utility.h"
#include "beam.h"

Beam createBeam(char* path, int speed, int damage, Point playerCoord) {
	Beam newBeam;
	newBeam.image = al_load_bitmap(path);
	newBeam.speed = speed;
	newBeam.damage = damage;
	newBeam.coord = playerCoord;
	newBeam.coord.y -= TILE_SIZE / 2;
	return newBeam;
}

bool updateBeam(Beam * beam, Point cam, int speed) {
	beam->coord.y -= beam->speed;
	beam->coord.y -= speed;


	if (beam->coord.y < cam.y) {
		return true;
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
	free(beam.image);
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

void updateBeamNode(BeamNode* beamNode, Point cam, int speed) {
	BeamNode* prev = beamNode;
	BeamNode* cur = beamNode->next;
	int i = 0;
	while (cur != NULL) {
		bool shouldDelete = updateBeam(&cur->beam, cam, speed);
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