//----------------------------------------------------------------------------------
//
// Project 12.3 from "Programming Pebble in C"
// 
// Rob Spiess, May 2016
//
#include <pebble.h>

// Needed for Pebble graphics
static Window *main_window;        // Our program's window
static Layer *fill_layer;          // The layer that displays the color

// Declarations for the project
int red = 0, green = 0, blue = 0;  // The individual colors to manipulate
GColor fill_color;                 // The resultant color created by combining our individual colors


// ------------------------------------------------------------------------ //
//  Drawing Functions
// ------------------------------------------------------------------------ //
void fill_layer_update(Layer *me, GContext *ctx) {
  
  // Get the size of our layer (which should fill the whole screen)
  GRect frame = layer_get_frame(me);

  // The fill_color code
  fill_color = (GColor8){ .a = 3, .r = red, .g = green, .b = blue };
  
  // Fill the layer with our fill_color
  graphics_context_set_fill_color(ctx, fill_color);
  graphics_fill_rect(ctx, frame, 0, GCornerNone);
}


// ------------------------------------------------------------------------ //
//  Button Functions
// ------------------------------------------------------------------------ //
void up_single_click_handler(ClickRecognizerRef recognizer, void *context) {

  // The Red Manipulation Code
  red = (red + 1) & 3;
  
  layer_mark_dirty(fill_layer);
}


void select_single_click_handler(ClickRecognizerRef recognizer, void *context) {

  // The Green Manipulation Code
  green = (green + 1) & 3;

  layer_mark_dirty(fill_layer);
}


void down_single_click_handler(ClickRecognizerRef recognizer, void *context) {

  // The Blue Manipulation Code
  blue = (blue + 1) & 3;
  
  layer_mark_dirty(fill_layer);
}


void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_single_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_single_click_handler);
}


// ------------------------------------------------------------------------ //
//  Main Functions
// ------------------------------------------------------------------------ //
static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  
  // Get screen dimensions to know how big to create our layer
  GRect window_frame = layer_get_frame(window_layer);

  // Create the layer to fill with color
  fill_layer = layer_create(window_frame);
  layer_set_update_proc(fill_layer, fill_layer_update);
  layer_add_child(window_layer, fill_layer);
  
  // Set the initial color for our fill layer
  fill_color = GColorBlack;
  
  // Enable button pressess
  window_set_click_config_provider(window, click_config_provider);
}


static void window_unload(Window *window) {
  // Window is unloading, destroy our fill layer
  layer_destroy(fill_layer);
}


static void init(void) {
  // App is starting, create our window
  main_window = window_create();
  window_set_window_handlers(main_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
  window_stack_push(main_window, true /* Animated */);
}


static void deinit(void) {
  // App is closing, destroy our window
  window_destroy(main_window);
}


int main(void) {
  init();
  app_event_loop();
  deinit();
}
