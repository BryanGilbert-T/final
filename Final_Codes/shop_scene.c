#include "utility.h"
#include "UI.h"
#include "shop_scene.h"

ShopItem sniper;

void init_shop(void) {
    
}

void draw_shop(void) {

}

void update_shop(void) {

}

void destroy_shop(void) {

}

Scene create_shop_scene(void) {
	Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "shop";
    scene.init = &init_shop;
    scene.draw = &draw_shop;
    scene.update = &update_shop;
    scene.destroy = &destroy_shop;

    return scene;

	
}
