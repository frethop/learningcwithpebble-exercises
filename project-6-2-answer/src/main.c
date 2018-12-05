//----------------------------------------------------------------------------------
//
// Project 6.2 from "Programming Pebble in C"
// 
// Paul Niven, February 2016

#include <pebble.h>

// pointer to main window
static Window *s_main_window;

// pointer to main window layer
static Layer *s_main_window_layer;

// GRect to hold the boundaries of the main window
static GRect s_main_window_bounds;

// GPoint to hold the center of the screen
static GPoint s_main_window_center;

// pointer to the layer we will draw on
static Layer *s_canvas_layer;

// string that represents the 3x5 character we want to draw - set to number 8
char *digit = "111101111101111";

// integer to hold the size of the squares to draw
static int tile_size = 14;

static void draw_digit(GContext *ctx, char * digit, int x, int y){
  
  // loop through the rows
  for (int i=0; i<5; i++){
    // loop through the columns
    for (int j=0; j<3; j++){
      
       // if the digit character is a 1 then draw a square
       if (digit[(i*3)+j] == '1'){
         graphics_fill_rect(ctx, GRect(x + (j*(tile_size+1)), y + (i*(tile_size+1)), tile_size, tile_size), 0, GCornerNone);
       }   
      
    }
  }
  
}


static void canvas_update_proc(Layer *this_layer, GContext *ctx) {

  // calculate the width of the digit - 3 tile columns plus 2 one pixel gaps
  int digit_width = (3 * tile_size) + 2;
  
  // calculate the height of the digit - 5 tile rows plus 4 one pixel gaps
  int digit_height = (5 * tile_size) + 4;
      
    // set the fill colour
    graphics_context_set_fill_color(ctx, GColorWhite);
  
    // draw four digits, grouped together in the center
  
    // top left
    draw_digit(ctx, digit, s_main_window_center.x-digit_width-(tile_size/2), s_main_window_center.y-digit_height-(tile_size/2));  
  
    // top right
    draw_digit(ctx, digit, s_main_window_center.x+(tile_size/2), s_main_window_center.y-digit_height-(tile_size/2));  
    
    // bottom left
    draw_digit(ctx, digit, s_main_window_center.x-digit_width-(tile_size/2), s_main_window_center.y+(tile_size/2));  
  
    // bottom right
    draw_digit(ctx, digit, s_main_window_center.x+(tile_size/2), s_main_window_center.y+(tile_size/2));  
  
  
}




static void main_window_load(Window *window) {
  
  // get the main window layer
  s_main_window_layer = window_get_root_layer(s_main_window);
  
  // Get the boundaries of the main layer
  s_main_window_bounds = layer_get_bounds(s_main_window_layer);
  
  // Get the center of the main layer
  s_main_window_center = grect_center_point(&s_main_window_bounds);
  
  // Create the layer we will draw on
  s_canvas_layer = layer_create(GRect(0, 0, s_main_window_bounds.size.w, s_main_window_bounds.size.h));
  
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