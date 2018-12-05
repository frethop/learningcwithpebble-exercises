//----------------------------------------------------------------------------------
//
// Project 19.4 from "Programming Pebble in C"
// 
// Johannes Neubrand, August 2016

#include <pebble.h>

static Window *s_window;
static GBitmap *s_result_bitmap;
static BitmapLayer *s_bitmap_layer;

typedef enum {
  UseImageWater, UseImageStreet
} UseImage;
static UseImage s_use_image;

static void reload_images() {
  gbitmap_destroy(s_result_bitmap);
  GBitmap *back_bitmap;
  back_bitmap = gbitmap_create_with_resource((s_use_image == UseImageWater ?
    RESOURCE_ID_WATER : RESOURCE_ID_STREET));
  s_result_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CHAMELEON);
  GRect bitmap_bounds = gbitmap_get_bounds(s_result_bitmap);
  // We can do this because we know that BOTH images use an 8bit format,
  // neither of them are palletized, and they have exactly the same size.
  // This isn't always the case.
  for (int y = 0; y < bitmap_bounds.size.h; y++) {
    GBitmapDataRowInfo row = gbitmap_get_data_row_info(s_result_bitmap, y);
    GBitmapDataRowInfo row_back = gbitmap_get_data_row_info(back_bitmap, y);
    for (int x = row.min_x; x <= row.max_x; x++)
      if (row.data[x] == 0b11000011)
        row.data[x] = row_back.data[x];
  }
  gbitmap_destroy(back_bitmap);
}

static void prv_select_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_use_image =
    (s_use_image == UseImageStreet ? UseImageWater : UseImageStreet);
  reload_images();
  bitmap_layer_set_bitmap(s_bitmap_layer, s_result_bitmap);
}

static void prv_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, prv_select_click_handler);
}

static void prv_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_bitmap_layer = bitmap_layer_create(bounds);
  reload_images();
  bitmap_layer_set_bitmap(s_bitmap_layer, s_result_bitmap);
  layer_add_child(window_get_root_layer(window),
    bitmap_layer_get_layer(s_bitmap_layer));
}

static void prv_window_unload(Window *window) {
  bitmap_layer_destroy(s_bitmap_layer);
  gbitmap_destroy(s_result_bitmap);
}

static void prv_init(void) {
  s_window = window_create();
  window_set_click_config_provider(s_window, prv_click_config_provider);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  window_stack_push(s_window, true);
}

static void prv_deinit(void) {
  window_destroy(s_window);
}

int main(void) {
  prv_init();
  app_event_loop();
  prv_deinit();
}
