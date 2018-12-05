#include <pebble.h>

#define BLOCK_WIDTH		20
#define BLOCK_HEIGHT	10

Window *window;

void update_display(struct Layer *layer, GContext *ctx)	{
	GColor color;
	color.argb = GColorCyanARGB8;
	graphics_context_set_fill_color(ctx, color);
	graphics_context_set_stroke_color(ctx, color);
	
	for (int y = 0; y < 64; ++y) {
		graphics_fill_rect(ctx, (GRect){ .origin = {.x = 0, .y = y * 2}, .size = {.w = 144, .h = 2}}, 0, GCornerNone);
	}
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
