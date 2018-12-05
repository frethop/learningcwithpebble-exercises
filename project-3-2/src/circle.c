#include <pebble.h>

#define ACCEL_STEP_MS 25

static Window *window;
static Layer *bouncy_layer;
static GRect window_frame;
static int position_x, position_y;
static int x_velocity, y_velocity;
static int center_point_x, center_point_y;
static int angle, radius;
static int acceleration;
static char letter;

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {

}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {

}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {    

}

static void move_letter() {
    angle = (angle + acceleration) % 360;
    int dangle = TRIG_MAX_ANGLE * angle / 360;
    position_x = center_point_x + (radius*-1*sin_lookup(dangle)/TRIG_MAX_RATIO);
    position_y = center_point_y + (radius*cos_lookup(dangle)/TRIG_MAX_RATIO);
}
    

static void bouncy_layer_update_callback(Layer *me, GContext *ctx) { 
#ifdef PBL_COLOR
  graphics_context_set_text_color(ctx, GColorCobaltBlue);
#else
  graphics_context_set_text_color(ctx, GColorBlack);
#endif
  char text[2];
  text[0] = letter;
  text[1] = 0;
  graphics_draw_text(ctx, text, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD), 
                     GRect(position_x,position_y,30,30), 
                     GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
}

static void timer_callback(void *data) {
    move_letter();    
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
        
    Layer *window_layer = window_get_root_layer(window);
    GRect frame = layer_get_frame(window_layer);
    center_point_x = frame.size.w/2;
    center_point_y = frame.size.h/2;
    x_velocity = y_velocity = 3;
    radius = 40;
    angle = 0;
    letter = 'A';
    acceleration = 1;

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