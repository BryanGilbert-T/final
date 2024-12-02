#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <string.h>
#include "UI.h"
#include "utility.h"
#include "game.h"

static bool mouse_in_rect(Point mouse, RecArea rect);

int cursor_animation;

Form form_create(int x, int y, int x2, int y2, int rx, int ry,
	ALLEGRO_COLOR default_color, ALLEGRO_COLOR clicked_color) {
	Form form;
	memset(&form, 0, sizeof(Form));

	form.default_color = default_color;
	form.clicked_color = clicked_color;

	form.x = x;
	form.y = y;
	form.x2 = x2;
	form.y2 = y2;
	form.rx = rx;
	form.ry = ry;

	form.clicked = false;
	memset(form.input, '\0', sizeof(form.input));

	return form;
}

void draw_form(Form form) {
	ALLEGRO_COLOR color = (form.clicked) ? form.clicked_color : form.default_color;
	al_draw_rounded_rectangle(
		form.x, form.y,
		form.x2, form.y2,
		form.rx, form.ry,
		color,
		5
	);
	al_draw_text(
		P2_FONT,
		al_map_rgb(225, 225, 225),
		form.x + 30,
		form.y + ((form.y2 - form.y) / 4) + 3,
		ALLEGRO_ALIGN_LEFT,
		form.input
	);

	int text_width = al_get_text_width(P2_FONT, form.input);

	if (form.clicked) {
		int offset = (cursor_animation / 25);
		if (offset == 0) {
			al_draw_filled_rectangle(
				form.x + text_width + 30, form.y + 15,
				form.x + text_width + 30 + 5, form.y2 - 15,
				al_map_rgb(225, 225, 225)
			);
		}
	}

}

void update_form(Form *form) {
	Point mouse = { mouseState.x, mouseState.y };
	RecArea rect = { form->x, form->y, form->x2 - form->x, form->y2 - form->y };

	if (mouse_in_rect(mouse, rect) && mouseState.buttons) {
		if (form->clicked == false) {
			form->clicked = true;
		}
		else {
			form->clicked = false;
		}
		al_rest(0.20);
	}
	else if (mouseState.buttons && form->clicked) {
		form->clicked = false;
	}

	if (form->clicked) {
		cursor_animation = (cursor_animation + 1) % 50;
		int inputSize = strlen(form->input);
		if (keyState[ALLEGRO_KEY_BACKSPACE]) {
			if (inputSize != 0) {
				form->input[inputSize - 1] = '\0';
				inputSize--;
				draw_form(*form);
				al_rest(0.15);
			}
		}
		if (inputSize < 10) {
			for (int i = ALLEGRO_KEY_A; i <= ALLEGRO_KEY_Z; i++) {
				if (keyState[i]) {
					if (keyState[ALLEGRO_KEY_LSHIFT] || keyState[ALLEGRO_KEY_RSHIFT]) {
						form->input[inputSize] = 'A' + (i - ALLEGRO_KEY_A);
					}
					else {
						form->input[inputSize] = 'a' + (i - ALLEGRO_KEY_A);
					}
					inputSize++;
					draw_form(*form);
					al_rest(0.15);
				}
			}
		}
		
	}
}

void destroy_form(Form* form) {
	
}

Button button_create(int x, int y, int w, int h, const char* default_image_path, const char* hovered_image_path) {

	Button button;
	memset(&button, 0, sizeof(Button));

	button.default_img = al_load_bitmap(default_image_path);
	button.hovered_img = al_load_bitmap(hovered_image_path);

	if (!button.default_img) {
        game_log("failed loading button image %s", default_image_path);
	}
    if (!button.hovered_img) {
        game_log("failed loading button image %s", hovered_image_path);
    }

	button.x = x;
	button.y = y;
	button.w = w;
	button.h = h;

	button.hovered = false;

	return button;
}

void draw_button(Button button) {
	ALLEGRO_BITMAP* _img = button.hovered ? button.hovered_img : button.default_img;
	al_draw_scaled_bitmap(
		_img,
		0, 0,
		al_get_bitmap_width(_img), al_get_bitmap_height(_img),
		button.x, button.y,
		button.w, button.h, 0
	);
}

void update_button(Button* button) {
	Point mouse = { mouseState.x, mouseState.y };
	RecArea rect = { button->x, button->y, button->w, button->h };
	/*
		[TODO Hackathon 4-2] 
		
		Using function you completed before,
		determine the button if it's hovered or not (button->hovered)
	*/
	if (mouse_in_rect(mouse, rect)) {
		button->hovered = true;
	}
	else {
		button->hovered = false;
	}

}

void destroy_button(Button* button) {
	al_destroy_bitmap(button->default_img);
	al_destroy_bitmap(button->hovered_img);
}

static bool mouse_in_rect(Point mouse, RecArea rect) {
	/*
		[TODO Hackathon 4-1] 
		
		Return true if mouse is inside the rectangle
	*/
	if (mouse.x > rect.x && mouse.y > rect.y &&
		mouse.x < rect.x + rect.w && mouse.y < rect.y + rect.h) {
		return true;
	}


	return false;
}

