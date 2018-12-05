#include <pebble.h>

#define BLOCK_WIDTH		20
#define BLOCK_HEIGHT	10

Window *window;

#define USE_METHOD_ONE

void update_display(struct Layer *layer, GContext *ctx)	{
	GColor color;
	
	#if defined(USE_METHOD_ONE)
		for (int y = 0; y < 64; ++y) {
			color.argb = 0b11000000 | y;
			graphics_context_set_fill_color(ctx, color);
			graphics_context_set_stroke_color(ctx, color);
			graphics_fill_rect(ctx, (GRect){ .origin = {.x = 0, .y = y * 2}, .size = {.w = 144, .h = 2}}, 0, GCornerNone);
		}
	#else
		int y = 0;
		int red = 0;
		int blue = 0;
		int green = 0;
		while (y < 64) {
			color.argb = 0b11000000 | (red << 4) | (green << 2) | blue;
			
			graphics_context_set_fill_color(ctx, color);
			graphics_context_set_stroke_color(ctx, color);
			graphics_fill_rect(ctx, (GRect){ .origin = {.x = 0, .y = y * 2}, .size = {.w = 144, .h = 2}}, 0, GCornerNone);
			
			++blue;
			if (blue > 3) {
				blue = 0;
				++green;
				if (green > 3) {
					green = 0;
					++red;
					if (red > 3)
						red = 0;
				}
			}
			
			++y;
		}
	#endif
}

void handle_init(void) {
	srand(time(NULL));
	
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
