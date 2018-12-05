#include <pebble.h>

Window *window;

#define NUMBERS_MAX		16
int numbers[NUMBERS_MAX];
int numbers_sorted[NUMBERS_MAX];

void bubble_sort(int array[])
{	
 	for (int i=0; i < NUMBERS_MAX - 1; i++) {
    	for (int j=0; j < NUMBERS_MAX - 1; j++) {
        	if (array[j] > array[j+1]) {
            	int temp = array[j];
            	array[j] = array[j+1];
            	array[j+1] = temp;
        	}
    	}
	}
}


void update_display(struct Layer *layer, GContext *ctx)	{
	graphics_context_set_text_color(ctx, GColorBlack);

	GRect header_box = (GRect){ .origin = {.x = 0, .y = 0}, .size = {.w = 144, .h = 10}};
	graphics_draw_text(ctx, "          Unsorted                 Sorted", fonts_get_system_font(FONT_KEY_GOTHIC_09), header_box, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
	
	for (int n = 0; n < NUMBERS_MAX; ++n) {
		GRect number_box = (GRect){ .origin = {.x = 0, .y = n * 10 + 9}, .size = {.w = 72, .h = 10}};
		char number_string[32];
		snprintf(number_string, 32, "%d", numbers[n]);
		graphics_draw_text(ctx, number_string, fonts_get_system_font(FONT_KEY_GOTHIC_09), number_box, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

		GRect sorted_box = (GRect){ .origin = {.x = 72, .y = n * 10 + 9}, .size = {.w = 72, .h = 10}};
		char sorted_string[32];
		snprintf(sorted_string, 32, "%d", numbers_sorted[n]);
		graphics_draw_text(ctx, sorted_string, fonts_get_system_font(FONT_KEY_GOTHIC_09), sorted_box, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
	}
}

void handle_init(void) {
	srand(time(NULL));
	
	for (int n = 0; n < NUMBERS_MAX; ++n) {
		numbers[n] = rand() % 1000;
		numbers_sorted[n] = numbers[n];
	}
	
	bubble_sort(numbers_sorted);
	
	window = window_create();

	layer_set_update_proc(window_get_root_layer(window), update_display);
	
	window_stack_push(window, true);
	
	layer_mark_dirty(window_get_root_layer(window));
}

void handle_deinit(void) {
	window_destroy(window);
}

int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}
