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
#include "leaderboard.h"

// False if havent played
bool cutscene0 = false; // Intro -> Conflict
bool cutscene1 = false; // Found the boss
bool cutscene2 = false; // Go back in time, need prof hu help

ALLEGRO_BITMAP* panda_cut;
ALLEGRO_BITMAP* fox_cut;
ALLEGRO_BITMAP* prof_cut;
ALLEGRO_BITMAP* aurick_cut;

ALLEGRO_BITMAP* chatbox;

Button submitButton;

FILE* f;

int content_size;
Content * contents[MAX_CONTENT];
int idx;
Content * now_content;

int timer;

Button skipButton;

bool outro;

int blackening_timer;

static int whichEps;

void initBlackGradually(int time) {
	blackening_timer = time * FPS;
	return;
}

void updateBlackGradually(void) {
	if (blackening_timer) {
		blackening_timer--;
	}
	return;
}

void drawBlackGradually(int time, int opacity) {
	int opacities = (float)opacity * (float)(1.0 - (float)((float)blackening_timer / (float)(time * FPS)));
	al_draw_filled_rectangle(0, 0, SCREEN_W, SCREEN_H, al_map_rgba(0, 0, 0, opacities));
	return;
}

char* selectCutscene(int ch) {
	switch (ch) {
		case 0:
			return "Assets/intro.txt";
		case 1:
			return "Assets/intro2.txt";
		case 2:
			return "Assets/first_fox_encounter.txt";
		case 3:
			return "Assets/intro_space.txt";
		case 4:
			return "Assets/outro_space.txt";
		case 5:
			return "Assets/intro_labyrinth.txt";
		case 6:
			return "Assets/intro_last_boss.txt";
		case 7:
			return "Assets/second_fox_encounter.txt";
		case 8:
			return "Assets/third_fox_encounter.txt";
		case 9:
			return "Assets/outro.txt";
		case 10:
			return "Assets/mino_lights_off.txt";
		case 11:
			return "Assets/fox_bunshin.txt";
		default:
			return "";
	}
}

Content* outroContent[10];
int outroSize = 9;
int curOutro;
int outroTimer;
bool runSubmitButton;
static bool changeSong;

Content* makeContentChat(char* chat, float seconds) {
	Content* newContent = (Content*)malloc(sizeof(Content));
	strcpy_s(newContent->chat, sizeof(newContent->chat), chat);
	newContent->seconds = seconds;
	return newContent;
}

void initOutro(void) {
	outroContent[0] = makeContentChat("I see", 3.0);
	outroContent[1] = makeContentChat("When I think about it", 4.0);
	outroContent[2] = makeContentChat("I guess he's right", 4.0);
	outroContent[3] = makeContentChat("it doesn't really matter", 4.0);
	outroContent[4] = makeContentChat("...", 2.25);
	outroContent[5] = makeContentChat("...", 2.25);
	outroContent[6] = makeContentChat("With that", 3.8);
	outroContent[7] = makeContentChat("I proudly present", 5.0);
	outroContent[8] = makeContentChat("GAME OVER", 8.0);
	runSubmitButton = false;
}

void destroyOutro(void) {
	for (int i = 0; i < outroSize; i++) {
		free(outroContent[i]);
	}
}

static int waitTimer;
void initCutscene(int episode) {
	content_size = 0;
	timer = 32;
	idx = 0;

	whichEps = episode;

	skipButton = button_create(SCREEN_W - TILE_SIZE * 2 - 30, SCREEN_H - TILE_SIZE - 20,
		TILE_SIZE * 2, TILE_SIZE,
		al_map_rgb(255, 255, 255),
		"Assets/skip_button.png", "Assets/skip_button_hovered.png");

	submitButton = button_create(SCREEN_W / 2 - ((SCREEN_W / 4 + 80) / 2), SCREEN_H - 90,
		SCREEN_W / 4 + 80, 60,
		al_map_rgb(255, 255, 255),
		"Assets/UI_Button.png", "Assets/UI_Button.png");

	char* pandaPath = "Assets/panda2.png";
	panda_cut = al_load_bitmap(pandaPath);
	if (!panda_cut) {
		game_abort("Cant load bitmap with path %s", pandaPath);
	}

	char* foxPath = "Assets/fokse.png";
	fox_cut = al_load_bitmap(foxPath);
	if (!fox_cut) {
		game_abort("Cant load bitmap with path %s", foxPath);
	}

	char* chatboxPath = "Assets/chatbox.png";
	chatbox = al_load_bitmap(chatboxPath);
	if (!chatbox) {
		game_abort("Cant load bitmap with path %s", chatboxPath);
	}

	char* aurickPath = "Assets/aurick.png";
	aurick_cut = al_load_bitmap(aurickPath);
	if (!aurick_cut) {
		game_abort("Cant laod bitamp with path %s", aurickPath);
	}

	char* profPath = "Assets/prof.png";
	prof_cut = al_load_bitmap(profPath);
	if (!prof_cut) {
		game_abort("Cant laod bitamp with path %s", profPath);
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
			newContent->left_who = PAN;
		}
		else if (strcmp(left, "Fox") == 0) {
			newContent->left = fox_cut;
			newContent->left_who = FOX;
		}
		else if (strcmp(left, "Aurick") == 0) {
			newContent->left = aurick_cut;
			newContent->left_who = TA;
		}
		else if (strcmp(left, "Hu") == 0) {
			newContent->left = prof_cut;
			newContent->left_who = PROF;
		}

		if (strcmp(right, "Panda") == 0) {
			newContent->right = panda_cut;
			newContent->right_who = PAN;
		}
		else if (strcmp(right, "Fox") == 0) {
			newContent->right = fox_cut;
			newContent->right_who = FOX;
		}
		else if (strcmp(right, "Aurick") == 0) {
			newContent->right = aurick_cut;
			newContent->right_who = TA;
		}
		else if (strcmp(right, "Hu") == 0) {
			newContent->right = prof_cut;
			newContent->right_who = PROF;
		}

		char name[20];
		fscanf_s(f, "%s", name, (unsigned int)sizeof(name));
		strcpy_s(newContent->name, sizeof(newContent->name), name);
		if (strcmp(newContent->name, "Hu") == 0) {
			strcpy_s(newContent->name, sizeof(newContent->name), "Prof. Hu");
		}
		if (strcmp(newContent->name, "Aurick") == 0) {
			strcpy_s(newContent->name, sizeof(newContent->name), "TA Aurick");
		}
		else if (strcmp(newContent->name, "()") == 0) {
			strcpy_s(newContent->name, sizeof(newContent->name), "");
		}

		fgets(name, sizeof(name), f);

		char chat[100];
		fgets(chat, sizeof(chat), f);
		strcpy_s(newContent->chat, sizeof(newContent->chat), chat);
		newContent->seconds = 0;

		contents[i] = newContent;
	}
	now_content = contents[idx];

	outro = false;
	changeSong = false;
	initOutro();
	waitTimer = 0;
}

void updateCutscene(void) {
	if (whichEps == 9) {
		if (changeSong == false) {
			if (!turnDownVolume()) {

			}
			else {
				change_bgm("Assets/audio/oogway_ascends.mp3");
				changeSong = true;
			}
		}		
		
	}
	if (outro) {
		if (curOutro == 3) {
			turnDownVolume();
		}
		if (blackening_timer) {
			blackening_timer--;
			return;
		}
		if (outroTimer) {
			outroTimer--;
		}
		else {
			if (curOutro + 1 < outroSize) {
				curOutro++;
				outroTimer = outroContent[curOutro]->seconds * (float)FPS;
				if (curOutro == 5) {
					change_bgm("Assets/audio/sparks.mp3");
				}
			}
			else {
				runSubmitButton = true;
				update_button(&submitButton);
				if (submitButton.hovered && mouseState.buttons) {
					change_scene(create_submit_scene());
					al_rest(1.2);
				}
			}
		}
		return;
	}
	update_button(&skipButton);
	if (skipButton.hovered && mouseState.buttons) {
		if (which_cutscene == 9) {
			outro = true;
			curOutro = 0;
			outroTimer = outroContent[curOutro]->seconds * (float)FPS;
			blackening_timer = FPS * 4;
			return;
		}
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
		if (which_cutscene == 9) {
			if (idx == content_size) {
				outro = true;
				curOutro = 0;
				outroTimer = outroContent[curOutro]->seconds * (float)FPS;
				blackening_timer = 4 * FPS;
				return;
			}
		}
		if (idx == content_size) {
			inCutscene = false;
			return;
		}	
		now_content = contents[idx];
	}
	return;
}

void drawCutscene(void) {
	if (outro) {
		int opacity = 255.0 * (float)(1.0 - (float)((float)blackening_timer / (float)(4 * FPS)));

		al_draw_filled_rectangle(0, 0, SCREEN_W, SCREEN_H,
			al_map_rgba(0, 0, 0, opacity));

		// Draw content
		al_draw_text(P2_FONT,
			al_map_rgb(235, 235, 235),
			SCREEN_W / 2, SCREEN_H / 2 - 5,
			ALLEGRO_ALIGN_CENTER,
			outroContent[curOutro]->chat);



		if (runSubmitButton) {
			//draw_button(submitButton);
			ALLEGRO_COLOR color = submitButton.hovered ? al_map_rgb(255, 200, 89) : al_map_rgb(235, 235, 235);
			al_draw_text(P3_FONT, color,
				submitButton.x + (submitButton.w / 2), submitButton.y + 15,
				ALLEGRO_ALIGN_CENTER,
				"Submit to Leaderboard");
		}

		return;
	}
	if (now_content->left) {
		if (now_content->left_who == PAN) {
			al_draw_scaled_bitmap(now_content->left,
				0, 0, 32, 32,
				20, SCREEN_H / 2, SCREEN_W / 3, SCREEN_H / 3,
				1);
		}
		else if (now_content->left_who == FOX) {
			al_draw_scaled_bitmap(now_content->left,
				0, 0, 134, 134,
				20, SCREEN_H / 2, SCREEN_W / 3, SCREEN_H / 3,
				1);
		}
		else if (now_content->left_who == TA) {
			al_draw_scaled_bitmap(now_content->left,
				0, 0, 424, 588,
				20, SCREEN_H / 2, SCREEN_W / 3, SCREEN_H / 3,
				1);
		}
		else if (now_content->left_who == PROF) {
			al_draw_scaled_bitmap(now_content->left,
				0, 0, 350, 350,
				20, SCREEN_H / 2, SCREEN_W / 3, SCREEN_H / 3,
				1);
		}
		
	}
	if (now_content->right) {
		if (now_content->right_who == PAN) {
			al_draw_scaled_bitmap(now_content->right,
				0, 0, 32, 32,
				SCREEN_W - (SCREEN_W / 3) - 20, SCREEN_H / 2, SCREEN_W / 3, SCREEN_H / 3,
				0);
		}
		else if (now_content->right_who == FOX) {
			al_draw_scaled_bitmap(now_content->right,
				0, 0, 134, 134,
				SCREEN_W - (SCREEN_W / 3) - 20, SCREEN_H / 2 - 30, SCREEN_W / 3, SCREEN_H / 3,
				0);
		}
		else if (now_content->right_who == TA) {
			al_draw_scaled_bitmap(now_content->right,
				0, 0, 424, 588,
				SCREEN_W - (SCREEN_W / 3) - 20, SCREEN_H / 2 - 10, SCREEN_W / 3, SCREEN_H / 3,
				0);
		}
		else if (now_content->right_who == PROF) {
			al_draw_scaled_bitmap(now_content->right,
				0, 0, 350, 350,
				SCREEN_W - (SCREEN_W / 3) - 20, SCREEN_H / 2 - 20, SCREEN_W / 3, SCREEN_H / 3,
				0);
		}
		
	}

	al_draw_scaled_bitmap(chatbox,
		0, 0, 800, 200,
		0, SCREEN_H * 3 / 4, SCREEN_W, SCREEN_H * 1 / 4,
		0);

	int ifNotPan = (strcmp(now_content->name, "Panda") == 0) ? 0 : 650;
	int ifNotPanAlignment = (strcmp(now_content->name, "Panda") == 0) ? ALLEGRO_ALIGN_LEFT : ALLEGRO_ALIGN_RIGHT;
	if (strcmp(now_content->name, "?") == 0) {
		ifNotPan = 600;
	}
	if (strcmp(now_content->name, "Fox") == 0) {
		ifNotPan = 630;
	}
	al_draw_text(P2_FONT,
		al_map_rgb(0, 0, 0),
		60 + ifNotPan, SCREEN_H * 3 / 4 + 15,
		ifNotPanAlignment,
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
	al_destroy_bitmap(aurick_cut);
	al_destroy_bitmap(chatbox);
	destroy_button(&skipButton);
	destroy_button(&submitButton);

	for (int i = 0; i < content_size; i++) {
		free(contents[i]);
	}

	destroyOutro();

}



