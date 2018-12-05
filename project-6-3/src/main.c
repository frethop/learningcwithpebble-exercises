#include <pebble.h>

Window *my_window;
TextLayer *text_layer;
static Layer *graphics_layer;

// Define your code below here

//----------------------------------------------------------------------
static void graphics_layer_update_callback(Layer *me, GContext *ctx) { 
    fill_the_rectangle(100, 80);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect frame = layer_get_frame(window_layer);

  graphics_layer = layer_create(frame);
  layer_set_update_proc(graphics_layer, graphics_layer_update_callback);
  layer_add_child(window_layer, graphics_layer);
    
}

static void window_unload(Window *window) {
  layer_destroy(graphics_layer);
}

void handle_init(void) {
  my_window = window_create();

  window_set_window_handlers(my_window, (WindowHandlers) {
	.load = window_load,
    .unload = window_unload,
  });

  text_layer = text_layer_create(GRect(0, 0, 144, 20));
  window_stack_push(my_window, true);
}

void handle_deinit(void) {
  text_layer_destroy(text_layer);
  window_destroy(my_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
