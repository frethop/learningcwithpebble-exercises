//----------------------------------------------------------------------------------
//
// Project 9.1 from "Programming Pebble in C"
// 
// Rob Spiess, May 2016
//
// Monospace Font from:
//   http://www.1001fonts.com/source-code-pro-font.html
//   Licensed under the SIL Open Font License (OFL)

#include <pebble.h>

// Hardcoded constants for the project
#define STRING_LENGTH 12                                  // Length of the word (in characters, includes the null terminator)
#define CHARACTER_WIDTH 11                                // Width of a single character (in pixels)
#define CHARACTER_HEIGHT 22                               // Height of a single character (in pixels)
#define TEXT_WIDTH (STRING_LENGTH - 1) * CHARACTER_WIDTH  // Width of the whole string (in pixels)
#define CURSOR_THICKNESS 2                                // Thickness of our cursor (in pixels)
#define REPEAT_RATE 100                                   // Rate to repeat button press (in milliseconds)

// Needed for Pebble graphics
static Window *window;          // Our program's window
static TextLayer *string_layer; // The layer that displays the string
static TextLayer *cursor_layer; // The layer that displays the cursor
static GFont font;              // Our custom monospaced font
static int left_edge, top_edge; // Position of our string_layer

// Declarations for the project
static int cursor_position = 0;                            // Cursor position.  We'll start at 0 (far left)
static char text_string[STRING_LENGTH] = "Hello World";    // Our string


// ------------------------------------------------------------------------ //
//  Display Functions
// ------------------------------------------------------------------------ //
static void refresh_screen() {
  layer_mark_dirty(text_layer_get_layer(string_layer));
  layer_mark_dirty(text_layer_get_layer(cursor_layer));
}


static void set_cursor_layer_position() {
  layer_set_frame(text_layer_get_layer(cursor_layer), GRect(left_edge + (cursor_position * CHARACTER_WIDTH), top_edge + CHARACTER_HEIGHT, CHARACTER_WIDTH, CURSOR_THICKNESS));
}


// ------------------------------------------------------------------------ //
//  Button Functions
// ------------------------------------------------------------------------ //
static void select_short_click_handler(ClickRecognizerRef recognizer, void *context) {
  cursor_position++;
  if(cursor_position >= (STRING_LENGTH - 1))
    cursor_position = 0;
  set_cursor_layer_position();
  refresh_screen();
}


static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  cursor_position--;
  if(cursor_position < 0)
    cursor_position = STRING_LENGTH - 2;
  set_cursor_layer_position();
  refresh_screen();
}


static void up_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_string[cursor_position]--;
  if(text_string[cursor_position] < ' ')
    text_string[cursor_position] = 'z';
  refresh_screen();
}


static void down_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_string[cursor_position]++;
  if(text_string[cursor_position] > 'z')
    text_string[cursor_position] = ' ';
  refresh_screen();
}


static void click_config_provider(void *context) {
  window_single_repeating_click_subscribe(BUTTON_ID_UP, REPEAT_RATE, up_single_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_short_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 0, select_long_click_handler, NULL);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, REPEAT_RATE, down_single_click_handler);
}


// ------------------------------------------------------------------------ //
//  Main Functions
// ------------------------------------------------------------------------ //
static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  
  // Calculate where to put our layers
  GRect window_frame = layer_get_frame(window_layer);
  left_edge = (window_frame.size.w - TEXT_WIDTH) / 2;
  top_edge = (window_frame.size.h - CHARACTER_HEIGHT) / 2;

  // Create the layer to show our cursor
  cursor_layer = text_layer_create(GRect(left_edge, top_edge + CHARACTER_HEIGHT, CHARACTER_WIDTH, CURSOR_THICKNESS));
  text_layer_set_background_color(cursor_layer, GColorWhite);
  layer_add_child(window_layer, text_layer_get_layer(cursor_layer));
  
  // Create the layer to display our string
  string_layer = text_layer_create(GRect(left_edge, top_edge, TEXT_WIDTH, CHARACTER_HEIGHT));
  text_layer_set_text_color(string_layer, GColorWhite);
  text_layer_set_background_color(string_layer, GColorClear);
  text_layer_set_font(string_layer, font);
  text_layer_set_text(string_layer, text_string);
  text_layer_set_text_alignment(string_layer, GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(string_layer));
}


static void window_unload(Window *window) {
  text_layer_destroy(cursor_layer);
  text_layer_destroy(string_layer);
}


static void init(void) {
  // Load our monospace font
  font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_SOURCE_18));

  // Create our window
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
  window_stack_push(window, true /* Animated */);
  window_set_background_color(window, GColorBlack);
  window_set_click_config_provider(window, click_config_provider);
}


static void deinit(void) {
  fonts_unload_custom_font(font);
  window_destroy(window);
}


int main(void) {
  init();
  app_event_loop();
  deinit();
}
