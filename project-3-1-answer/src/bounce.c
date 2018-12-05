//----------------------------------------------------------------------------------
//
// Project 3.1 from "Programming Pebble in C"
// 
// Mike Jipping, December 2015

#include <pebble.h>

#define ACCEL_STEP_MS 50

// These declarations are necessary to run on the watch
static Window *window;
static Layer *bouncy_layer;
static GRect window_frame;

// These are the declarations that configure the ball!
static int position_x, position_y;
static int x_velocity, y_velocity;
static int ball_radius = 20;

// The select button starts the ball rolling (so to speak)
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
    position_x = 72;
    position_y = 25;
    x_velocity = y_velocity = 6;
} 

// The up button increases the ball size and speed.
static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
    ball_radius ++;
    x_velocity ++;
    y_velocity ++;
}

// The down button decreases the ball's size and speed
static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
    ball_radius --;
    x_velocity --;
    y_velocity --;
}

// This function moves the ball every time the timer goes off
static void bounce_the_ball() {
    if ((position_x - ball_radius < 0)
        || (position_x + ball_radius > window_frame.size.w)) {
        x_velocity = x_velocity * -1;
    }
    if ((position_y - ball_radius < 0)
        || (position_y + ball_radius > window_frame.size.h)) {
        y_velocity = y_velocity * -1;
    }
    position_y = position_y + y_velocity;
    position_x = position_x + x_velocity;
}
  
//-------------------------------------------------------------------------------

static void bouncy_layer_update_callback(Layer *me, GContext *ctx) { 
    if (position_x == -1) return;
    
#ifdef PBL_COLOR
  graphics_context_set_fill_color(ctx, GColorCobaltBlue);
#else
  graphics_context_set_fill_color(ctx, GColorWhite);
#endif
  graphics_fill_circle(ctx, GPoint(position_x, position_y), ball_radius);
}

static void timer_callback(void *data) {

    bounce_the_ball();    
    layer_mark_dirty(bouncy_layer);
    app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect frame = window_frame = layer_get_frame(window_layer);

  bouncy_layer = layer_create(frame);
  layer_set_update_proc(bouncy_layer, bouncy_layer_update_callback);
  layer_add_child(window_layer, bouncy_layer);
}

static void window_unload(Window *window) {
  layer_destroy(bouncy_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
	.load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
    
   position_x = position_y = -1;
    
  app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}