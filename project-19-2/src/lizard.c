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

// .update_proc of my_layer:
void draw_lizard(GContext* ctx) {
  GPath *lizard_path = gpath_create(&lizard_points);

  //gpath_rotate_to(lizard_path, TRIG_MAX_ANGLE / 360 * 20);
  //gpath_move_to(lizard_path, GPoint(20, 10));
    
  graphics_context_set_fill_color(ctx, GColorGreen);
  gpath_draw_filled(ctx, lizard_path);
  graphics_context_set_stroke_color(ctx, GColorBlack);
  gpath_draw_outline(ctx, lizard_path);
}

static void drawing_layer_update_callback(Layer *me, GContext *ctx) { 
    draw_lizard(ctx);
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
