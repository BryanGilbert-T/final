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
#include "cut_scene.h"

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

int content_size;
Content * contents[MAX_CONTENT];
int idx;
Content * now_content;

int timer;

Button skipButton;

char* selectCutscene(int ch) {
	switch (ch) {
		case 0:
			return "Assets/cutscene0.txt";
		case 1:
			return "Assets/cutscene1.txt";
		case 2:
			return "Assets/cutscene2.txt";
		case 3:
			return "Assets/cutscene3.txt";
		case 4:
			return "Assets/cutscene4.txt";
		case 5:
			return "Assets/cutscene5.txt";
		case 6:
			return "Assets/cutscene6.txt";
		case 7:
			return "Assets/cutscene7.txt";
		default:
			return "";
	}
}


void initCutscene(int episode) {
	content_size = 0;
	timer = 32;
	idx = 0;

	skipButton = button_create(SCREEN_W - TILE_SIZE * 2 - 30, SCREEN_H - TILE_SIZE - 20,
		TILE_SIZE * 2, TILE_SIZE,
		al_map_rgb(255, 255, 255),
		"Assets/skip_button.png", "Assets/skip_button_hovered.png");

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

	char* path = selectCutscene(episode);
	int err = fopen_s(&f, path, "r");
	if (err != 0 || f == NULL) {
		game_abort("Cant open %s", path);
	}

	fscanf_s(f, "%d", &content_size);
	for (int i = 0; i < content_size; i++) {
		Content* newContent = malloc(sizeof(Content));
		memset(newContent, 0, sizeof(Content));

		char left[20];
		char right[20];
		fscanf_s(f, "%s %s", left, (unsigned int)sizeof(left), right, (unsigned int)sizeof(right));

		if (strcmp(left, "Panda") == 0) {
			newContent->left = panda_cut;
		}
		else if (strcmp(left, "Fox") == 0) {
			newContent->left = fox_cut;
		}

		if (strcmp(right, "Panda") == 0) {
			newContent->right = panda_cut;
		}
		else if (strcmp(right, "Fox") == 0) {
			newContent->right = fox_cut;
		}

		char name[20];
		fscanf_s(f, "%s", name, (unsigned int)sizeof(name));
		strcpy_s(newContent->name, sizeof(newContent->name), name);
		if (strcmp(newContent->name, "Hu") == 0) {
			strcpy_s(newContent->name, sizeof(newContent->name), "Prof. Hu");
		}
		else if (strcmp(newContent->name, "()") == 0) {
			strcpy_s(newContent->name, sizeof(newContent->name), "");
		}

		fgets(name, sizeof(name), f);

		char chat[100];
		fgets(chat, sizeof(chat), f);
		strcpy_s(newContent->chat, sizeof(newContent->chat), chat);
		
		contents[i] = newContent;
	}
	now_content = contents[idx];
}

void updateCutscene(void) {
	update_button(&skipButton);
	if (skipButton.hovered && mouseState.buttons) {
		inCutscene = false;
		return;
	}
	if (timer) {
		timer--;
		return;
	}
	if (mouseState.buttons || keyState[ALLEGRO_KEY_SPACE] || keyState[ALLEGRO_KEY_ENTER]) {
		timer = 32;
		idx++;		
		if (idx == content_size) {
			inCutscene = false;
			return;
		}	
		now_content = contents[idx];
	}
	return;
}

void drawCutscene(void) {
	if (now_content->left) {
		al_draw_scaled_bitmap(now_content->left,
			0, 0, 32, 32,
			20, SCREEN_H /2 ,SCREEN_W / 3, SCREEN_H / 3,
			1);
	}
	if (now_content->right) {
		al_draw_scaled_bitmap(now_content->right,
			0, 0, 32, 32,
			SCREEN_W - (SCREEN_W / 3) - 20, SCREEN_H / 2, SCREEN_W / 3, SCREEN_H / 3,
			0);
	}

	al_draw_scaled_bitmap(chatbox,
		0, 0, 800, 200,
		0, SCREEN_H * 3 / 4, SCREEN_W, SCREEN_H * 1 / 4,
		0);

	al_draw_text(P2_FONT,
		al_map_rgb(0, 0, 0),
		60, SCREEN_H * 3 / 4 + 15,
		ALLEGRO_ALIGN_LEFT,
		now_content->name);

	al_draw_text(P2_FONT,
		al_map_rgb(0, 0, 0),
		35, SCREEN_H - 110,
		ALLEGRO_ALIGN_LEFT,
		now_content->chat);

	//draw_button(skipButton);
	ALLEGRO_COLOR color = (skipButton.hovered) ? al_map_rgb(219, 121, 29) : al_map_rgb(173, 92, 16);
	al_draw_text(P2_FONT,
		color,
		skipButton.x + (skipButton.w / 2), skipButton.y + (skipButton.h / 2),
		ALLEGRO_ALIGN_CENTER,
		"SKIP>");
	
}

void destroyCutscene(void) {
	al_destroy_bitmap(panda_cut);
	al_destroy_bitmap(fox_cut);
	al_destroy_bitmap(chatbox);
	destroy_button(&skipButton);

	for (int i = 0; i < content_size; i++) {
		free(contents[i]);
	}

}



