#ifndef cutscene_h
#define cutscene_h

#define MAX_CONTENT 100

typedef struct _Content {
	ALLEGRO_BITMAP* left;
	ALLEGRO_BITMAP* right;
	char name[20];
	char chat[105];
} Content;

void initCutscene(int episode);
void destroyCutscene(void);
void drawCutscene(void);
void updateCutscene(void);

#endif