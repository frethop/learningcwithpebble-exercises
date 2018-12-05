//----------------------------------------------------------------------------------
//
// Project 8.3 from "Programming Pebble in C"
// 
// Allan Findlay, April 2016

#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
static TextLayer *words_layer;

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

// Here's the main function: num2words.  Takes an integer
// and produces a string that is the wordy-equivalent of the
// integer.
//
// Note that this answer uses "snprintf" to construct the
// word.

static char *num2words(int number) {
	char *sign = "";
	if (number < 0) {
		sign = "minus";
		number = -number;
	}
	
	char *first_twenty[] = {
		"zero", "one", "two", "three", "four", "five",
		"six", "seven", "eight", "nine", "ten", "eleven",
		"twelve", "thirteen", "fourteen", "fifteen", "sixteen",
		"seventeen", "eighteen", "nineteen"
	};

	char *units[] = {"", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};
	
	static char words[32];
	
	switch (number / 10) {
		case 0:
		case 1:
			snprintf(words, 32, "%s%s", sign, first_twenty[number % 20]);
			break;
		
		case 2:	snprintf(words, 32, "%stwenty %s",  sign, units[number % 10]);	break;
		case 3:	snprintf(words, 32, "%sthirty %s",  sign, units[number % 10]);	break;
		case 4:	snprintf(words, 32, "%sforty %s",   sign, units[number % 10]);	break;
		case 5:	snprintf(words, 32, "%sfifty %s",   sign, units[number % 10]);	break;
		case 6:	snprintf(words, 32, "%ssixty %s",   sign, units[number % 10]);	break;
		case 7:	snprintf(words, 32, "%sseventy %s", sign, units[number % 10]);	break;
		case 8:	snprintf(words, 32, "%seighty %s",  sign, units[number % 10]);	break;
		case 9:	snprintf(words, 32, "%sninety %s",  sign, units[number % 10]);	break;
	}
	
	return words;
}

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

	// build up the sentence
	static char sentence[128];
	sentence[0] = '\0'; // make the string empty
	char *new_word = num2words(left_number);
	strncat(sentence, new_word, 128);
	strncat(sentence, " ", 128);
	strncat(sentence, op, 128);
	strncat(sentence, " ", 128);
	new_word = num2words(right_number);
	strncat(sentence, new_word, 128);
	strncat(sentence, " = ", 128);
	new_word = num2words(result);
	strncat(sentence, new_word, 128);
	text_layer_set_text(words_layer, sentence);
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
	text_layer_set_text(text_layer, "");
	text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(text_layer));
	
	words_layer = text_layer_create(GRect(0, 80, bounds.size.w, 60));
	text_layer_set_text(words_layer, "");
	text_layer_set_text_alignment(words_layer, GTextAlignmentCenter);
	text_layer_set_overflow_mode(words_layer, GTextOverflowModeWordWrap);
	layer_add_child(window_layer, text_layer_get_layer(words_layer));

	set_display_text();
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
	app_event_loop();
	deinit();
}