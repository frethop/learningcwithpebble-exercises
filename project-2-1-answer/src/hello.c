// Project 2.1 from Chapter 2 of "Programming Pebble in C"
//
// Very simple "Hello World" style app to get started.
//
// Your Name, November 2015

#include <pebble.h>

Window *window;
TextLayer *text_layer;

void init() {
  window = window_create();
  text_layer = text_layer_create(GRect(0, 0, 144, 40));
  text_layer_set_text(text_layer, "Love ya, Pebble!");
  layer_add_child(window_get_root_layer(window), 
                    text_layer_get_layer(text_layer));
  window_stack_push(window, true);
}

void deinit() {
  text_layer_destroy(text_layer);
  window_destroy(window);
}

int main() {
  init();
  app_event_loop();
  deinit();
  return 0;
}