#ifndef leaderboard_h
#define leaderboard_h

Scene create_leaderboard_scene(void);

bool insert_to_leaderboard(char name[11], int point);

typedef struct _leaderboard {
	char name[11];
	int point;
} leaderboard;

#endif