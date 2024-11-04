#ifndef shop_scene_h
#define shop_scene_h

typedef struct _ShopItem {
	// Item Name
	char* name;
	ALLEGRO_BITMAP* item_picture;

	int price;
	bool bought;
} ShopItem;


#endif //shop_scene.h//