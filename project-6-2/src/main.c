#include <pebble.h>

// pointer to main window
static Window *s_main_window;

// pointer to main window layer
static Layer *s_main_window_layer;

// pointer to the layer we will draw on
static Layer *s_canvas_layer;

// string that represents the 3x5 character we want to draw - set to number 8
char *digit = "111101111101111";


static void canvas_update_proc(Layer *this_layer, GContext *ctx) {

  // set the fill colour
  graphics_context_set_fill_color(ctx, GColorWhite);

  // loop through the rows
  for (int i=0; i<5; i++){
    // loop through the columns
    for (int j=0; j<3; j++){
      
       // if the digit character is a 1 then draw a square
       if (digit[(i*3)+j] == '1'){
         graphics_fill_rect(ctx, GRect((j*15), (i*15), 14, 14), 0, GCornerNone);
       }   
      
    }
  }

  
  
}




static void main_window_load(Window *window) {
  
  // get the main window layer
  s_main_window_layer = window_get_root_layer(s_main_window);
  
  // Get the boundaries of the main layer
  GRect window_bounds = layer_get_bounds(s_main_window_layer);
  
  // Create the layer we will draw on
  s_canvas_layer = layer_create(GRect(0, 0, window_bounds.size.w, window_bounds.size.h));
  
  // Add the layer to our main window layer
  layer_add_child(s_main_window_layer, s_canvas_layer);

  // Set the update procedure for our layer
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
    
}


static void main_window_unload(Window *window) {
  
  // Unsubscribe from event services
  tick_timer_service_unsubscribe();
  
  // Destroy Layer
  layer_destroy(s_canvas_layer);
  
}


static void init(void) {
  
  // Create the main window
  s_main_window = window_create();
  
  // set the background colour
  window_set_background_color(s_main_window, GColorBlack);
  
  // set the window load and unload handlers
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  
  // show the window on screen
  window_stack_push(s_main_window, true);
  
}


static void deinit(void) {
  
  // Destroy the main window
  window_destroy(s_main_window);
  
}


int main(void) {
  
  init();
  app_event_loop();
  deinit();
  
}