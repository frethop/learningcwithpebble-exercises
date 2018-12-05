//----------------------------------------------------------------------------------
//
// Project 19.2 from "Learning C with Pebble"
//
// Mike Jipping, August 2016

#include <pebble.h>

// Needed for Pebble graphics
static Window *window;
static Layer *drawing_layer;

// Declarations for the project
static int screen_width, screen_height;
static GPoint center;

static const GPathInfo lizard_points = {
  .num_points = 43,
  .points = (GPoint []) {
      // head
      {0,72}, {58,72}, {42,88}, {12,72}, {52,14}, {54,29}, {72,9},
      // body
      {110,23}, {126,43}, {133,73}, 
      // tail to tip
      {115,118}, {92,124}, {74,108}, {84,80}, {105,88}, {111,102},
      {98,113}, {84, 104}, {91,96}, {100,101}, 
      // tip to foot
      {93,101}, {90,105}, {97,107}, {104,100},
      {92,91}, {79,108}, {96,116}, {113,104}, {110,80},
      // foot
      {104,74}, {93,71}, {100,68}, {96,58},
      // other foot
      {72,72}, {72,91}, {65,101}, {49,101}, {52,97}, {65,90}, 
      // the rest
      {58,45}, {58,72}, {51,14}, {58, 72}
  }
};

// the bug!
static const GPathInfo bug_points = {
  .num_points = 7,
  .points = (GPoint []) {
      {0,73}, {2,70}, {4,70}, {6,73}, {4,76}, {2,76}, {0,73}
  }
};

// Draw the lizard by calling from the layer redraw proc
void draw_lizard(GContext* ctx) {
  GPath *lizard_path = gpath_create(&lizard_points);
  GPath *bug_path = gpath_create(&bug_points);

  //gpath_rotate_to(lizard_path, TRIG_MAX_ANGLE / 360 * 20);
  gpath_move_to(lizard_path, GPoint(10, 0));
    
  graphics_context_set_fill_color(ctx, GColorGreen);
  gpath_draw_filled(ctx, lizard_path);
  graphics_context_set_stroke_color(ctx, GColorBlack);
  gpath_draw_outline(ctx, lizard_path);
  graphics_context_set_fill_color(ctx, GColorBlack);
  gpath_draw_filled(ctx, bug_path);
    
  GPoint center = {
      .x = 41,
      .y = 59
  };
  graphics_fill_circle(ctx, center, 3);
  graphics_draw_circle(ctx, center, 8);
  graphics_draw_line(ctx, GPoint(6, 73), GPoint(16, 72));
    
  graphics_context_set_text_color(ctx, GColorRed);
  graphics_draw_text(ctx,
	    		"Lizzy",
	    		fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD),
	    		GRect(0,120,144,140),
	    		GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
    
}

// Only need to draw the lizard
static void drawing_layer_update_callback(Layer *me, GContext *ctx) { 
    draw_lizard(ctx);
}

// Setup the drawing layer to be drawn when the layer is created
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
