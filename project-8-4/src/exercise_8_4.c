#include <pebble.h>

#define BLOCK_WIDTH		20
#define BLOCK_HEIGHT	10

Window *window;
AppTimer* refresh = NULL;

void refresh_timer_callback(void *data) {
	layer_mark_dirty(window_get_root_layer(window)); // Tell the layer it needs to redraw.
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
}


void update_block(int *x, int *y, int* dx, int* dy) {
	*x += *dx;
	if (*x + BLOCK_WIDTH >= 143 || *x <= 0)
		*dx = -(*dx);
	
	*y += *dy;
	if (*y + BLOCK_HEIGHT >= 167 || *y <= 0)
		*dy = -(*dy);
}

void update_display(struct Layer *layer, GContext *ctx)	{
	static int x = 100;
	static int y = 100;
	static int dx = 1;
	static int dy = 1;
	
	// Clear the screen.
	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_fill_rect(ctx, (GRect) { .origin = { .x = 0, .y = 0}, .size = { .w = 144, .h = 168}}, 0, GCornerNone);
	
	graphics_context_set_stroke_color(ctx, GColorBlack);
	update_block(&x, &y, &dx, &dy); // Passing pointers to the variables so their values will be updated when changed in the function.
	graphics_draw_rect(ctx, (GRect){ .origin = {.x = x, .y = y}, .size = {.w = BLOCK_WIDTH, .h = BLOCK_HEIGHT}});

	refresh = app_timer_register(16, refresh_timer_callback, NULL);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

void handle_init(void) {
	srand(time(NULL));
	
	window = window_create();
    window_set_click_config_provider(window, click_config_provider);

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
