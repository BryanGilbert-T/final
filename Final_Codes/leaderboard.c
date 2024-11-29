#include <allegro5/allegro.h>
#include <string.h>
#include "menu_scene.h"
#include "leaderboard.h"
#include "game_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"

leaderboard *leaderboards;

void init(void) {
    char* path = "Assets/leaderboard.txt";
    FILE* f = fopen(path, "r");
    if (!f) {
        game_abort("Can't load map file : %s", path);
    }

    int size;
    fscanf_s(f, "%d", &size);

    leaderboards = malloc(size * sizeof(leaderboard));

    for (int i = 0; i < size; i++) {
        fscanf_s(f, "%s %d", leaderboards[i].name, sizeof(leaderboards[i].name),
            leaderboards[i].point);
    }

    fclose(f);
}

void draw(void) {

}

void update(void) {

}

void destroy(void) {
    free(leaderboards);
}


Scene create_leaderboard_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "leaderboard";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}

