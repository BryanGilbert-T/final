#ifndef UI_H
#define UI_H
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <string.h>
#include "utility.h"

typedef struct _Button {
	int x, y, w, h;

	ALLEGRO_BITMAP* default_img;
	ALLEGRO_BITMAP* hovered_img;

	bool hovered;
}Button;

typedef struct _Form {
	int x, y;
	int x2, y2;
	int rx, ry;

	ALLEGRO_COLOR default_color;
	ALLEGRO_COLOR clicked_color;

	bool clicked;

	char input[30];
} Form;

Button button_create(int x, int y, int w, int h, const char* default_image_path, const char* hovered_image_path);
void draw_button(Button button);
void update_button(Button* button);
void destroy_button(Button* button);

Form form_create(int x, int y, int x2, int y2, int rx, int ry,
	ALLEGRO_COLOR default_color, ALLEGRO_COLOR clicked_color);
void draw_form(Form form);
void update_form(Form* form);
void destroy_form(Form* form);

#endif