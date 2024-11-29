#ifndef leaderboard_h
#define leaderboard_h

Scene create_leaderboard_scene(void);

typedef struct _leaderboard {
	char* name;
	int point;
} leaderboard;

#endif