//----------------------------------------------------------------------------------
//
// Project 18.1 from "Programming Pebble in C"
// (based on Project 3.1)
// 
// ishotjr, August 2016
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

// actual *types* differ between platforms!
#ifdef PBL_COLOR
  static GColor8 ball_color;
#else
  static GColor ball_color;
#endif

// Double-clicking the select button starts the ball rolling (so to speak)
static void select_double_click_handler(ClickRecognizerRef recognizer, void *context) {
    position_x = 72;
    position_y = 25;
    x_velocity = y_velocity = 6;
} 

// The up button increases the ball's size on single click.
static void up_single_click_handler(ClickRecognizerRef recognizer, void *context) {
    ball_radius ++;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "ball_radius: %d", ball_radius);
}

// The down button decreases the ball's size on single click.
static void down_single_click_handler(ClickRecognizerRef recognizer, void *context) {
    ball_radius --;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "ball_radius: %d", ball_radius);
}


// The up button increases the ball's speed when held.
static void up_long_click_handler(ClickRecognizerRef recognizer, void *context) {
    // consider sign in order to correctly increase
    x_velocity < 0 ? x_velocity-- : x_velocity++;
    y_velocity < 0 ? y_velocity-- : y_velocity++;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "x_velocity: %d", x_velocity);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "y_velocity: %d", y_velocity);
}

// The down button decreases the ball's speed when held.
static void down_long_click_handler(ClickRecognizerRef recognizer, void *context) {
    // stop at 0
    if (x_velocity != 0) {
        // consider sign in order to correctly decrease
        x_velocity < 0 ? x_velocity++ : x_velocity--;
        y_velocity < 0 ? y_velocity++ : y_velocity--;
    }
    APP_LOG(APP_LOG_LEVEL_DEBUG, "x_velocity: %d", x_velocity);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "y_velocity: %d", y_velocity);
}

// The select button changes the ball color (if supported)
static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
#ifdef PBL_COLOR
    if (gcolor_equal(ball_color, GColorCobaltBlue)) {
        ball_color = GColorRed;
    } else {
        ball_color = GColorCobaltBlue;
    }
#endif
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
    
    graphics_context_set_fill_color(ctx, ball_color);
    graphics_fill_circle(ctx, GPoint(position_x, position_y), ball_radius);
}

static void timer_callback(void *data) {

    bounce_the_ball();    
    layer_mark_dirty(bouncy_layer);
    app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_single_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_single_click_handler);

  // 18.1.1: break speed change out onto long buttons (no up handler, just down)
  window_long_click_subscribe(BUTTON_ID_UP, 0, up_long_click_handler, NULL);
  window_long_click_subscribe(BUTTON_ID_DOWN, 0, down_long_click_handler, NULL);

  // 18.1.2: restart ball on double click only
  window_multi_click_subscribe(BUTTON_ID_SELECT, 2, 0, 0, true, select_double_click_handler);

  // 18.1.3: long click select to change the color of the ball
  window_long_click_subscribe(BUTTON_ID_SELECT, 2000, select_long_click_handler, NULL);
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
  ball_color = COLOR_FALLBACK(GColorCobaltBlue, GColorWhite);

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