#include <pebble.h>

static Window *window;
static TextLayer *text_layer;

typedef enum {
	PLUS,
	MINUS,
	TIMES,
	DIVIDE
} Operation;

static int changing = 0;
static int left_number = 1;
static int right_number = 1;
static Operation operation = PLUS;

static void set_display_text() {
	static char buffer[32];
	char *op = "";
	int result = 0;
	
	switch (operation) {
		case PLUS: op = "+";	result = left_number + right_number;	break;
		case MINUS: op = "-";	result = left_number - right_number;	break;
		case TIMES: op = "*";	result = left_number * right_number;	break;
		case DIVIDE: op = "/";	result = left_number / right_number;	break;
	}
	
	char *format = "";
	switch (changing) {
		case 0: format = "[%d] %s %d = %d"; break;
		case 1: format = "%d [%s] %d = %d"; break;
		case 2: format = "%d %s [%d] = %d"; break;
	}
	snprintf(buffer, 32, format, left_number, op, right_number, result);
	text_layer_set_text(text_layer, buffer);
}

static void change(int direction) {
	switch (changing) {
		case 0:
			left_number += direction;
			if (left_number > 9)
				left_number = 9;
			else if (left_number < 1)
				left_number = 1;
			break;
		
		case 1:
			if (direction > 0) {
				if (operation == DIVIDE)
					operation = PLUS;
				else
					++operation;
			}
			else {
				if (operation == PLUS)
					operation = DIVIDE;
				else
					--operation;				
			}
			break;
		
		case 2:
			right_number += direction;
			if (right_number > 9)
				right_number = 9;
			else if (right_number < 1)
				right_number = 1;
			break;
	}
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
	++changing;
	if (changing > 2)
		changing = 0;
	set_display_text();
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
	change(1);
	set_display_text();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
	change(-1);
	set_display_text();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create(GRect(0, 32, bounds.size.w, 20));
  text_layer_set_text(text_layer, "Press a button");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}