//----------------------------------------------------------------------------------
//
// Project 19.1 from "Programming Pebble in C"
// 
// ishotjr, August 2016
// Mike Jipping, August 2016
//
// inspired by http://www.how-to-draw-funny-cartoons.com/how-to-draw-a-husky.html

#include <pebble.h>

// These declarations are necessary to run on the watch
static Window *s_window;
static Layer *s_layer;

// Points for various animal paths
static const GPathInfo animal_points = {
  .num_points = 10,
  .points = (GPoint []) {
    // ears
    {40,14}, {54,38}, {144 - 54,38}, {144 - 40,14},
    // haunches
    {144 - 40,112}, {144 - 22,112}, {144 - 22,150}, {22,150}, {22,112}, {40,112}
  }
};

static const GPathInfo interior_points = {
  .num_points = 17,
  .points = (GPoint []) {
    // left paw
    {72,136}, {72,156}, {52,156}, {52,136},
    // right paw
    {144 - 72,136}, {144 - 72,156}, {144 - 52,156}, {144 - 52,136},
    {72,136}, 
    // chest/face
    {86,136}, {86,98}, {96,98}, {96,48},
    {144 - 96,48}, {144 - 96,98}, {144 - 86,98}, {144 - 86,136}
  }
};

static const GPathInfo haunch_points = {
  .num_points = 4,
  .points = (GPoint []) {
    // left haunch - will be duplicated and moved to create right
    {22,150}, {22,112}, {40,112}, {40,150}, 
  }
};

static const GPathInfo left_ear_points = {
  .num_points = 3,
  .points = (GPoint []) {
    {40,22}, {50,40}, {40,46}
  }
};
static const GPathInfo right_ear_points = {
  .num_points = 3,
  .points = (GPoint []) {
    {144 - 40,22}, {144 - 50,40}, {144 - 40,46}
  }
};

static const GPathInfo nose_points = {
  .num_points = 12,
  .points = (GPoint []) {
    {72,74}, {66,80}, {72,86}, {72,92}, {62,90}, {72,92},
    {144 - 72,92}, {144 - 62,90}, {144 - 72,92}, {144 - 72,86}, {144 - 66,80}, {144 - 72,74}
  }
};

static const GPathInfo left_eye_points = {
  .num_points = 4,
  .points = (GPoint []) {
    {58,64}, {64,60}, {68,66}, {62,68}
  }
};
static const GPathInfo right_eye_points = {
  .num_points = 4,
  .points = (GPoint []) {
    {144 - 58,64}, {144 - 64,60}, {144 - 68,66}, {144 - 62,68}
  }
};
// can you tell what it is yet? :D


// draw animal path along prescribed points
static void prv_draw_animal(GContext *ctx) {

  GPath *animal_path = gpath_create(&animal_points);
  GPath *interior_path = gpath_create(&interior_points);
  GPath *left_haunch_path = gpath_create(&haunch_points);
  GPath *right_haunch_path = gpath_create(&haunch_points);
  GPath *left_ear_path = gpath_create(&left_ear_points);
  GPath *right_ear_path = gpath_create(&right_ear_points);
  GPath *nose_path = gpath_create(&nose_points);
  GPath *left_eye_path = gpath_create(&left_eye_points);
  GPath *right_eye_path = gpath_create(&right_eye_points);

  // duplicate and move the left haunch to create the right
  // can rotate, but not mirror, so ears and eyes can't use this trick  
  gpath_move_to(right_haunch_path, GPoint(144 - 22 - 40, 0));

  graphics_context_set_stroke_color(ctx, GColorBlack);

  graphics_context_set_fill_color(ctx, GColorLightGray);
  gpath_draw_filled(ctx, animal_path);
  gpath_draw_outline(ctx, animal_path);

  graphics_context_set_fill_color(ctx, GColorWhite);
  gpath_draw_filled(ctx, interior_path);
  gpath_draw_outline(ctx, interior_path);

  gpath_draw_filled(ctx, left_haunch_path);
  gpath_draw_outline(ctx, left_haunch_path);
  gpath_draw_filled(ctx, right_haunch_path);
  gpath_draw_outline(ctx, right_haunch_path);

  gpath_draw_filled(ctx, left_ear_path);
  gpath_draw_outline(ctx, left_ear_path);
  gpath_draw_filled(ctx, right_ear_path);
  gpath_draw_outline(ctx, right_ear_path);

  graphics_context_set_fill_color(ctx, GColorBlack);
  gpath_draw_filled(ctx, nose_path);
  gpath_draw_outline(ctx, nose_path);

  // increase stroke for eyes
  graphics_context_set_stroke_width(ctx, 2);
  graphics_context_set_stroke_color(ctx, GColorVividCerulean);
  gpath_draw_filled(ctx, left_eye_path);
  gpath_draw_outline(ctx, left_eye_path);
  gpath_draw_filled(ctx, right_eye_path);
  gpath_draw_outline(ctx, right_eye_path);
}

// layer is never marked dirty again, so only called once
static void layer_update_callback(Layer *layer, GContext *ctx) {
  prv_draw_animal(ctx);
}


static void prv_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  // let's give our pup a colorful home
  window_set_background_color(window, GColorOrange);  

  GRect frame = layer_get_frame(window_layer);

  s_layer = layer_create(frame);
  layer_set_update_proc(s_layer, layer_update_callback);
  layer_add_child(window_layer, s_layer);
}

static void prv_window_unload(Window *window) {
  layer_destroy(s_layer);
}

static void prv_init(void) {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  const bool animated = true;
  window_stack_push(s_window, animated);
}

static void prv_deinit(void) {
  window_destroy(s_window);
}

int main(void) {
  prv_init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", s_window);

  app_event_loop();
  prv_deinit();
}
