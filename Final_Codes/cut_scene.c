#include <allegro5/allegro.h>
#include <string.h>
#include "menu_scene.h"
#include "costumize_scene.h"
#include "game_scene.h"
#include "setting_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"
#include "loading_scene.h"

// False if havent played
bool cutscene0 = false; // Intro -> Conflict
bool cutscene1 = false; // Found the boss
bool cutscene2 = false; // Go back in time, need prof hu help

ALLEGRO_BITMAP* panda_cut;
ALLEGRO_BITMAP* fox_cut;
ALLEGRO_BITMAP* hu_cut;
ALLEGRO_BITMAP* aurick_cut;

ALLEGRO_BITMAP* chatbox;

FILE* f;
FILE* f0;
FILE* f1;
FILE* f2;

ALLEGRO_BITMAP* left;
ALLEGRO_BITMAP* right;

char* content;
char* name;

void initCutscene(void) {
	char* pandaPath = "Assets/panda2.png";
	panda_cut = al_load_bitmap(pandaPath);
	if (!panda_cut) {
		game_abort("Cant load bitmap with path %s", pandaPath);
	}

	char* foxPath = "Assets/fox.png";
	fox_cut = al_load_bitmap(foxPath);
	if (!fox_cut) {
		game_abort("Cant load bitmap with path %s", foxPath);
	}

	char* chatboxPath = "Assets/chatbox.png";
	chatbox = al_load_bitmap(chatboxPath);
	if (!chatbox) {
		game_abort("Cant load bitmap with path %s", chatboxPath);
	}

	char* path0 = "cutscene0.txt";
	f0 = fopen(path0, "r");
	if (!f0) {
		game_abort("cant load %s", path0);
	}

	char* path1 = "cutscene1.txt";
	f1 = fopen(path1, "r");
	if (!f1) {
		game_abort("cant load %s", path1);
	}

	char* path2 = "cutscene2.txt";
	f2 = fopen(path2, "r");
	if (!f2) {
		game_abort("cant load %s", path2);
	}

	left = NULL;
	right = NULL;
}

void select_cut_scene(char ch) {
	switch (ch) {
		case '0':
			f = f0;
			break;
		case '1':
			f = f1;
			break;
		case '2':
			f = f2;
			break;
	}
}

void updateCutscene(void) {
	char* firstline;
	fscanf_s(f, "%[^\n]", firstline);

	char* left_pict = strtok(firstline, " ");
	if (left_pict) {
		if (strcmp(left_pict, "Panda") == 0) {
			left = panda_cut;
		}
		else if (strcmp(left_pict, "Fox") == 0) {
			left = fox_cut;
		}
	}
	
	char* right_pict = strtok(NULL, " ");
	if (right_pict) {
		if (strcmp(left_pict, "Panda") == 0) {
			left = panda_cut;
		}
		else if (strcmp(left_pict, "Fox") == 0) {
			left = fox_cut;
		}
	}

	char* secondline;
	fscanf_s(f, "%[^\n]", secondline);
	strcpy(name, secondline);

	char* thirdline;
	fscanf_s(f, "%[^\n]", thirdline);
	strcpy(content, thirdline);

	char* trash;
	fscanf_s(f, "%s", trash);
}

void drawCutscene(void) {
	al_draw_scaled_bitmap(chatbox,
		0, 0, 800, 200,
		0, SCREEN_H * 3 / 4, SCREEN_W, SCREEN_H * 1 / 4,
		0);

	if (left) {
		al_draw_scaled_bitmap(left,
			0, 0, 32, 32,
			0, SCREEN_H * 1 / 2, SCREEN_W * 1/4, SCREEN_H * 1/4,
			1);
	}
	if (right) {
		al_draw_scaled_bitmap(right,
			0, 0, 32, 32,
			0, SCREEN_H * 1 / 2, SCREEN_W * 1/4, SCREEN_H * 1/4,
			0);
	}

	al_draw_text(P2_FONT,
		al_map_rgb(225, 225, 225),
		20, SCREEN_H * 3 / 4 - 20,
		ALLEGRO_ALIGN_CENTER,
		name);

	al_draw_text(P2_FONT,
		al_map_rgb(225, 225, 225),
		20, SCREEN_H * 3 / 4 + 50,
		ALLEGRO_ALIGN_LEFT,
		content);
}

void destroyCutscene(void) {
	al_destroy_bitmap(panda_cut);
	al_destroy_bitmap(fox_cut);
	al_destroy_bitmap(chatbox);

}



