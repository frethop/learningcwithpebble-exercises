//----------------------------------------------------------------------------------
//
// Project 6.4 from "Programming Pebble in C"
// 
// Rob Spiess, February 2016

#include <pebble.h>

// Needed for Pebble graphics
static Window *window;
static Layer *drawing_layer;


// Declarations for the project
static int screen_width, screen_height;
static GPoint center;


// Draw concentric circles using recursion
static void draw_circles_recursion(GContext *ctx, int radius) {
  if(radius < screen_width / 2) {
    graphics_context_set_stroke_color(ctx, GColorBlack);
    graphics_draw_circle(ctx, center, radius);
    draw_circles_recursion(ctx, radius + 10);
  }
}


static void drawing_layer_update_callback(Layer *me, GContext *ctx) { 
  draw_circles_recursion(ctx, 0);
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