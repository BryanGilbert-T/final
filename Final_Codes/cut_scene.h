#ifndef cutscene_h
#define cutscene_h

#define MAX_CONTENT 100

typedef enum _Shei {
	PAN,
	TA,
	PROF,
	FOX,
	NONE,
} Shei;


typedef struct _Content {
	ALLEGRO_BITMAP* left;
	ALLEGRO_BITMAP* right;
	Shei left_who;
	Shei right_who;
	char name[20];
	char chat[105];
	float seconds;
} Content;


void initBlackGradually(int time);
void updateBlackGradually(void);
void drawBlackGradually(int time, int opacity);

void initCutscene(int episode);
void destroyCutscene(void);
void drawCutscene(void);
void updateCutscene(void);

#endif