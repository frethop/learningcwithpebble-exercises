#include <pebble.h>

// Needed for Pebble graphics
static Window *window;
static Layer *drawing_layer;


// Declarations for the project
static int screen_width, screen_height;
static GPoint center;


// Draw concentric circles
static void draw_circles(GContext *ctx) {
  graphics_context_set_stroke_color(ctx, GColorBlack);
  for(int radius = 0; radius < screen_width / 2; radius += 10)
    graphics_draw_circle(ctx, center, radius);
}


static void drawing_layer_update_callback(Layer *me, GContext *ctx) { 
  draw_circles(ctx);
}


static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect frame = layer_get_frame(window_layer);

  drawing_layer = layer_create(frame);
  layer_set_update_proc(drawing_layer, drawing_layer_update_callback);
  layer_add_child(window_layer, drawing_layer);
}


static void window_unload(Window *window) {
  layer_destroy(drawing_layer);
}


static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
	.load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
  
  Layer *window_layer = window_get_root_layer(window);
  
  GRect frame = layer_get_frame(window_layer);

  screen_width = frame.size.w;
  screen_height = frame.size.h;
  
  // Center Point on the screen
  center.x = screen_width / 2;
  center.y = screen_height / 2;
}


static void deinit(void) {
    window_destroy(window);
}


int main(void) {
  init();
  app_event_loop();
  deinit();
}
