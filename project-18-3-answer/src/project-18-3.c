//----------------------------------------------------------------------------------
//
// Project 18.3 from "Learning C with Pebble"
//
// Johannes Neubrand, July 2016

#include <pebble.h>
#include "headlines.h"

static Window *s_window;
static TextLayer *s_text_layer;
static TextLayer *s_text_layer_important;
// Different char*'s are used to prevent null pointer exceptions (since the
// hidden layer would keep a reference to freed memory)
static char *headline;
static char *important_headline;

static void prv_select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (headline) {
    free(headline);
  }
  headline = get_headline();
  text_layer_set_text(s_text_layer, headline);

  layer_set_hidden(text_layer_get_layer(s_text_layer_important), true);
  layer_set_hidden(text_layer_get_layer(s_text_layer), false);
}

static void prv_select_hold_handler(ClickRecognizerRef recognizer, void *context) {
  if (important_headline) {
    free(important_headline);
  }
  important_headline = get_headline();
  text_layer_set_text(s_text_layer_important, important_headline);

  layer_set_hidden(text_layer_get_layer(s_text_layer_important), false);
  layer_set_hidden(text_layer_get_layer(s_text_layer), true);
}

static void prv_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, prv_select_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 400, prv_select_hold_handler,
    NULL);
}

static void prv_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_text_layer = text_layer_create(GRect(20, bounds.size.h / 2 - 30,
    bounds.size.w - 40, 60));
  text_layer_set_text(s_text_layer, "Press Select");
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));

  s_text_layer_important = text_layer_create(GRect(20, bounds.size.h / 2 - 30,
    bounds.size.w - 40, 60));
  text_layer_set_text(s_text_layer_important, "---");
  text_layer_set_text_alignment(s_text_layer_important, GTextAlignmentCenter);
  text_layer_set_background_color(s_text_layer_important, GColorRed);
  text_layer_set_text_color(s_text_layer_important, GColorYellow);
  layer_set_hidden(text_layer_get_layer(s_text_layer_important), true);
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer_important));
}

static void prv_window_unload(Window *window) {
  text_layer_destroy(s_text_layer);
}

static void prv_init(void) {
  s_window = window_create();
  window_set_click_config_provider(s_window, prv_click_config_provider);
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
  app_event_loop();
  prv_deinit();
}
