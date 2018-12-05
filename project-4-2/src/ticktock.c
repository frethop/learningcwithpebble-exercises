#include <pebble.h>

static Window *window;
static TextLayer *text_layer;

static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
    int hour = tick_time->tm_hour;
    int minute = tick_time->tm_min;
    int second = tick_time->tm_sec;
    static char time_string[] = "00:00:00";

    strftime(time_string, sizeof(time_string), "%T", tick_time);
    text_layer_set_text(text_layer, time_string);
}

static void window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect frame = layer_get_frame(window_layer);

    text_layer = text_layer_create(GRect(0, frame.size.h/2-30, frame.size.w, frame.size.h));
    text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
    text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
    text_layer_destroy(text_layer);
}

void init() {
    window = window_create();
    window_set_window_handlers(window, (WindowHandlers) {
	    .load = window_load,
        .unload = window_unload,
    });
    window_stack_push(window, true);
    
    tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
}

void deinit() {
    tick_timer_service_unsubscribe();
    text_layer_destroy(text_layer);
    window_destroy(window);
}

int main() {
    init();
    app_event_loop();
    deinit();
}