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

char* selectCutscene(int ch) {
	switch (ch) {
		case 0:
			return "Assets/cutscene0.txt";
		case 1:
			return "Assets/cutscene1.txt";
		case 2:
			return "Assets/cutscene2.txt";
		default:
			return "";
	}
}


void initCutscene(int episode) {
	content_size = 0;
	timer = 64;
	idx = 0;

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
		game_log("%d", i);

		char left[20];
		char right[20];
		fscanf_s(f, "%s %s", left, (unsigned int)sizeof(left), right, (unsigned int)sizeof(right));
		game_log("%s %s", left, right);

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
		game_log("%s", newContent->name);

		fgets(name, sizeof(name), f);

		char chat[100];
		fgets(chat, sizeof(chat), f);
		strcpy_s(newContent->chat, sizeof(newContent->chat), chat);
		game_log("%s", newContent->chat);
		
		contents[i] = newContent;
	}
	now_content = contents[idx];
}

void updateCutscene(void) {
	if (timer) {
		timer--;
		return;
	}
	if (mouseState.buttons || keyState[ALLEGRO_KEY_SPACE] || keyState[ALLEGRO_KEY_ENTER]) {
		timer = 64;
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
			SCREEN_W - (20 + al_get_bitmap_width(now_content->right) * 2), SCREEN_H / 2, SCREEN_W / 3, SCREEN_H / 3,
			0);
	}

	al_draw_scaled_bitmap(chatbox,
		0, 0, 800, 200,
		0, SCREEN_H * 3 / 4, SCREEN_W, SCREEN_H * 1 / 4,
		0);

	al_draw_text(P2_FONT,
		al_map_rgb(0, 0, 0),
		40, SCREEN_H * 3 / 4,
		ALLEGRO_ALIGN_LEFT,
		now_content->name);

	al_draw_text(P2_FONT,
		al_map_rgb(0, 0, 0),
		20, SCREEN_H - 100,
		ALLEGRO_ALIGN_LEFT,
		now_content->chat);
	
}

void destroyCutscene(void) {
	al_destroy_bitmap(panda_cut);
	al_destroy_bitmap(fox_cut);
	al_destroy_bitmap(chatbox);

	for (int i = 0; i < content_size; i++) {
		free(contents[i]);
	}

}



