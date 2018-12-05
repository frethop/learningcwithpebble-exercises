//----------------------------------------------------------------------------------
//
// Project 18.2.2 from "Learning C with Pebble"
//
// Mike Jipping, August 2016

#include <pebble.h>

// Needed for Pebble graphics
static Window *window;
static Layer *drawing_layer;
static Layer *window_layer;
static GRect window_frame;

#define POSSIBLE_LAYERS 8
#define FALSE 0
#define TRUE 1

// Declarations for the project
static int screen_width, screen_height;
static GPoint center;
static Layer *table[POSSIBLE_LAYERS];

// This is based on either creating a new layer or drawing in an existing layer.
static void drawing_layer_update_callback(Layer *me, GContext *ctx) {
   int i, radius;
   bool found = FALSE;
    
   // Look for "me" in the "created" table
   // If found, just draw circle.
   for (i=0; i<POSSIBLE_LAYERS; i++) {
       if (table[i] == me) {
           APP_LOG(APP_LOG_LEVEL_INFO, "Found layer: %d", i);
          radius = screen_width / 2 - (10*i);
          graphics_context_set_stroke_color(ctx, GColorBlack);
          GColor color = GColorBlack;
          color.argb += 5 * i;
          graphics_context_set_fill_color(ctx, color);      
          graphics_fill_circle(ctx, center, radius);
          graphics_draw_circle(ctx, center, radius);
          found = TRUE;
          break;
       }
   }
    
   // If not found, put this one in the table and create another.
   if (! found) {
      for (i=0; i<POSSIBLE_LAYERS; i++) if (table[i] == NULL) break;
      if (i < POSSIBLE_LAYERS) 
          table[i] = me;
      else
          // DONE!  Table is full!
          return;

      Layer *window_layer = window_get_root_layer(window);
      GRect frame = layer_get_frame(window_layer);

      drawing_layer = layer_create(frame);
      layer_set_update_proc(drawing_layer, drawing_layer_update_callback);
      layer_add_child(window_layer, drawing_layer);
   }
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect frame = layer_get_frame(window_layer);

  drawing_layer = layer_create(frame);
  layer_set_update_proc(drawing_layer, drawing_layer_update_callback);
  layer_add_child(window_layer, drawing_layer);
    
  for (int i=0; i<POSSIBLE_LAYERS; i++) table[i] = NULL;
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
  
  window_layer = window_get_root_layer(window);
  window_frame = layer_get_frame(window_layer);

  screen_width = window_frame.size.w;
  screen_height = window_frame.size.h;
  
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
