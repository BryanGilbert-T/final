#ifndef leaderboard_h
#define leaderboard_h

Scene create_leaderboard_scene(void);
Scene create_submit_scene(void);

void insert_to_leaderboard(char name[11], int point);

typedef struct _leaderboard {
	char name[11];
	int point;
} leaderboard;

#endif