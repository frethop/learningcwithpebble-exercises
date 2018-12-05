#include <pebble.h>

static Window *window;
static Layer *seconds_layer;
static int position_x, position_y;
static int center_point_x, center_point_y;
static int second, radius;
static int direction=1;
static GColor time_color;

static void change_colors() {
    
}

static void mark_the_second(GContext *ctx) {
    
    if (second % 5 == 0) {
        char text[3];
        int number = second / 5;
        
        switch (number) {
            case 0:
                strcpy(text, "12");
                break;
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
                text[0] = 48 + number;
                text[1] = 0;
                break;
            case 10:
                strcpy(text, "10");
                break;
            case 11:
                strcpy(text, "11");
                break;
        }
        
        graphics_context_set_text_color(ctx, time_color);
        graphics_draw_text(ctx, text, fonts_get_system_font(FONT_KEY_GOTHIC_24), 
                     GRect(position_x,position_y,30,30), 
                     GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
    }
}

static void move_second_hand() {
    second = (second + 1) % 60;
    int dangle = TRIG_MAX_ANGLE * second /  60;
    position_x = center_point_x + (radius*direction*sin_lookup(dangle)/TRIG_MAX_RATIO);
    position_y = center_point_y + (-radius*cos_lookup(dangle)/TRIG_MAX_RATIO);
}
    

static void seconds_layer_update_callback(Layer *me, GContext *ctx) { 
    graphics_draw_line(ctx, GPoint(center_point_x, center_point_y), GPoint(position_x, position_y));
    mark_the_second(ctx);
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
    move_second_hand();
    change_colors();
    layer_mark_dirty(seconds_layer);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect frame = layer_get_frame(window_layer);

  seconds_layer = layer_create(frame);
  layer_set_update_proc(seconds_layer, seconds_layer_update_callback);
  layer_add_child(window_layer, seconds_layer);
    
}

static void window_unload(Window *window) {
  layer_destroy(seconds_layer);
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
    center_point_x = frame.size.w/2;
    center_point_y = frame.size.h/2;
    radius = 40;
    second = 0;
    time_color = GColorBlack;

    tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);

}

static void deinit(void) {
    tick_timer_service_unsubscribe();
    window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}