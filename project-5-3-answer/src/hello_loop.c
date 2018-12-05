//----------------------------------------------------------------------------------
//
// Project 5.3 from "Programming Pebble in C"
// 
// Rob Spiess, January 2016

#include <pebble.h>

// Needed for Pebble graphics
static Window *window;
static Layer *drawing_layer;


// Declarations for the project
static int screen_width, screen_height;
static GColor text_color;


// Answer 2, using one WHILE loop
static void fill_screen(GContext *ctx) {
  int position_x, position_y;
  int text_width, text_height;
  char text_string[] = "Hello, Pebble!";
  
  text_width = 75;
  text_height = 15;
  position_x = 0;
  position_y = 0;
  
  graphics_context_set_text_color(ctx, text_color);
  while(position_y < screen_height) {
    graphics_draw_text(ctx, text_string, fonts_get_system_font(FONT_KEY_GOTHIC_14), 
                       GRect(position_x, position_y, text_width, text_height), 
                       GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
    position_x = position_x + text_width;
    if(position_x >= screen_width) {
      position_x = 0;
      position_y = position_y + text_height;
    }
  }
}


/*
// Alternate Answer 2, using a Boolean with one WHILE loop
static void fill_screen(GContext *ctx) {
  int position_x, position_y;
  int text_width, text_height;
  char text_string[] = "Hello, Pebble!";
  bool done;
  
  text_width = 75;
  text_height = 15;
  position_x = 0;
  position_y = 0;
  done = false;
  
  graphics_context_set_text_color(ctx, text_color);
  while(!done) {
    graphics_draw_text(ctx, text_string, fonts_get_system_font(FONT_KEY_GOTHIC_14), 
                       GRect(position_x, position_y, text_width, text_height), 
                       GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
    position_x = position_x + text_width;
    if(position_x >= screen_width) {
      position_x = 0;
      position_y = position_y + text_height;
      if(position_y >= screen_height) {
        done = true;
      }
    }
  }
}
*/


static void drawing_layer_update_callback(Layer *me, GContext *ctx) { 
  fill_screen(ctx);
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
    text_color = GColorBlack;
}


static void deinit(void) {
    window_destroy(window);
}


int main(void) {
  init();
  app_event_loop();
  deinit();
}