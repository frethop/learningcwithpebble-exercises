//----------------------------------------------------------------------------------
//
// Project 5.1 from "Programming Pebble in C"
// 
// Allan Findlay with Mike Jipping, February 2016

#include <pebble.h>

Window *window;
int16_t random_number;

// Display a decimal number at (x,y) using the graphics context ctx
void display_number(int number, int x, int y, GContext *ctx) {
 	GRect decimal_box = (GRect){ .origin = {.x = x, .y = y}, .size = {.w = 144, .h = 30}};
	char decimal_string[32];
	snprintf(decimal_string, 32, "%d", number);
	graphics_draw_text(ctx, decimal_string, 
                       fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), 
                       decimal_box, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
}

// Display a bit at (x,y) using the graphics context ctx
void display_bit(int bit, int x, int y, GContext *ctx) {
 	GRect bit_box = (GRect){ .origin = {.x = x, .y = y}, .size = {.w = 8, .h = 30}};
	char bit_string[2];
	snprintf(bit_string, 2, "%d", bit);
    graphics_draw_text(ctx, bit_string, 
                       fonts_get_system_font(FONT_KEY_GOTHIC_18), 
                       bit_box, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
}

// This function displays a random number, then its binary equivalent
void update_display(struct Layer *layer, GContext *ctx)	{
	graphics_context_set_text_color(ctx, GColorBlack);

    display_number(random_number, 0, 20, ctx);
    
    // Start with the random_number and with X coordinate on the right of the screen
	int number = random_number;
	int x = 124;
    
    // For 16 bits, extract the rightmost bit with an AND and shift the number
    // Be sure to decrement the X coordinate
	for (int bit = 0; bit < 16; ++bit) {
		if (number & 1)
            display_bit(1,x,60,ctx);
		else
            display_bit(0,x,60,ctx);

		x -= 8;
		number >>= 1;
    }
}

//----------------------------------------------------------------------

void handle_init(void) {
	srand(time(NULL));
	random_number = rand() % 1000;
	
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
