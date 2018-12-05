//----------------------------------------------------------------------------------
//
// Project 18.4 from "Learning C with Pebble"
// 
// Rob Spiess, August 2016

#include <pebble.h>


// ------------------------------------------------------------------------------ //
//  Global Variables
// ------------------------------------------------------------------------------ //

// Needed for the menu layer window
static Window *menu_window;
MenuLayer *menu_layer;

// Needed for the article window
static Window *article_window;
static ScrollLayer *scroll_layer;
static TextLayer *article_text_layer;

// Buffers to hold our text
#define BUFFER_SIZE 500
static char buffer[BUFFER_SIZE + 1];
static char article_text_buffer[BUFFER_SIZE + 1];
static char headlines[4][BUFFER_SIZE + 1];

int number_of_articles = 0;



// ------------------------------------------------------------------------------ //
//  File Handling
// ------------------------------------------------------------------------------ //

// File handling variables
static ResHandle article_file_handle;
static size_t article_file_size;


// Used by findlinefeed() to choose direction to search
#define FORWARD 1
#define BACKWARD -1


// Find a line feed in the file opened with handle.
// Work in the direction (FORWARD or BACKWARD) until a line feed is found.
int findlinefeed(ResHandle handle, int startpos, int filesize, int direction) {
  int position = startpos;
  char c = 0;
  uint8_t character[1];

  while (position >= 0 && position < filesize) {
    resource_load_byte_range(handle, position, character, 1);
    c = character[0];
    if (c == '\n') break;
    position += direction;
  }

  return position;
}



static char* next_headline() {
  // Static headline_position so it keeps its value next time the function is called
  static int headline_position = 0;

  // Find the pointer for the end of the headline
  int end = findlinefeed(article_file_handle, headline_position, article_file_size, FORWARD);

  // Get the length of the headline (in bytes)
  int length = end - headline_position;

  // Make sure the length isn't longer than the buffer we have
  if (length > BUFFER_SIZE)
    length = BUFFER_SIZE;

  // Copy the headline into our buffer
  resource_load_byte_range(article_file_handle, headline_position, (uint8_t *)buffer, length);

  // Make sure the string is zero-terminated
  buffer[length] = '\0';

  // get headline_position ready for next call by skipping over article
  headline_position = findlinefeed(article_file_handle, end + 1, article_file_size, FORWARD) + 1;

  return buffer;
}


static char* get_article(int index) {
  // Pointer to byte where the article starts
  int start = 0;

  // Skip over "index" number of headlines and articles, plus skip over one headline
  for (int i = 0; i < index * 2 + 1; i++) {
    start = findlinefeed(article_file_handle, start, article_file_size, FORWARD) + 1;
  }

  // Find pointer to the end of the article
  int end = findlinefeed(article_file_handle, start, article_file_size, FORWARD);

  // Get the length of the article (in bytes)
  int length = end - start;

  // Make sure the length isn't longer than the buffer we have
  if (length > BUFFER_SIZE)
    length = BUFFER_SIZE;

  // Copy the article into our buffer
  resource_load_byte_range(article_file_handle, start, (uint8_t *)buffer, length);

  // Make sure the string is zero-terminated
  buffer[length] = '\0';

  return buffer;
}


static void load_headlines() {
  number_of_articles = 0;
  strcpy(headlines[number_of_articles++], next_headline());
  strcpy(headlines[number_of_articles++], next_headline());
  strcpy(headlines[number_of_articles++], next_headline());
  strcpy(headlines[number_of_articles++], next_headline());
}


static void open_articles_file() {
  article_file_handle = resource_get_handle(RESOURCE_ID_ARTICLES);
  article_file_size = resource_size(article_file_handle);
}



// ------------------------------------------------------------------------------ //
//  Headline Menu Functions
// ------------------------------------------------------------------------------ //

static void menu_select_click(MenuLayer *menu_layer, MenuIndex *index, void *data) {
  // Get the article and copy it to our article_text_buffer
  strcpy(article_text_buffer, get_article(index->row));

  // Show the article window (which has our text)
  const bool animated = true;
  window_stack_push(article_window, animated);
}


static uint16_t get_num_sections(struct MenuLayer *menu_layer, void *callback_context) {
  return 1;
}


static uint16_t get_num_rows(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return number_of_articles;
}


static void draw_row(GContext *ctx, const Layer *cell_layer, MenuIndex *index, void *data) {
  GRect bounds = layer_get_bounds(cell_layer);

  // Draw cell background, depending if highlighted or not
  if (menu_cell_layer_is_highlighted(cell_layer)) {
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_context_set_text_color(ctx, GColorWhite);
  } else {
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_context_set_text_color(ctx, GColorBlack);
  }
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);  

  // Draw the headline text
  graphics_draw_text(ctx, headlines[index->row],
                     fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
                     bounds,
                     GTextOverflowModeWordWrap,
                     GTextAlignmentCenter,
                     NULL);
}


static void menu_window_load(Window *window) {
  window_set_background_color(window, GColorWhite);

  Layer *window_layer = window_get_root_layer(window);
  GRect frame = layer_get_frame(window_layer);

  // Bring menu in from the screen edge on round displays
  #ifdef PBL_ROUND
  int inset = 10;
  frame = grect_inset(frame, GEdgeInsets(inset));
  #endif

  menu_layer = menu_layer_create(frame);
  // Set all of our callbacks.
  menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks) {
    .get_num_sections = get_num_sections,
    .get_num_rows = get_num_rows,
    .draw_row = draw_row,
    .select_click = menu_select_click,
  });

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(menu_layer, window);

  // Add it to the window for display
  layer_add_child(window_layer, menu_layer_get_layer(menu_layer));
}


void menu_window_unload(Window *window) {
  menu_layer_destroy(menu_layer);
}



// ------------------------------------------------------------------------------ //
//  Article Handling
// ------------------------------------------------------------------------------ //

void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  // Pop the article window off the stack when SELECT is pressed
  window_stack_pop(true);
}

// Add SELECT button handling
// The scroll layer already handles UP and DOWN
void article_click_config(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}


static void article_window_load(Window *window) {
  window_set_background_color(window, GColorBlack);

  Layer *window_layer = window_get_root_layer(window);
  GRect frame = layer_get_frame(window_layer);
  GRect max_text_bounds = GRect(0, 0, frame.size.w, 2000);

  // Initialize the scroll layer and add it to the window
  scroll_layer = scroll_layer_create(frame);
  layer_add_child(window_layer, scroll_layer_get_layer(scroll_layer));

  // Bind the menu layer's click config provider to the window for interactivity
  scroll_layer_set_click_config_onto_window(scroll_layer, window);
  scroll_layer_set_callbacks(scroll_layer, (ScrollLayerCallbacks){.click_config_provider=article_click_config});

  // Scrolls through text on a round screen by page (scroll offset is changed by frame's height)
  #ifdef PBL_ROUND
  scroll_layer_set_paging(scroll_layer, true);
  #endif

  // Create the text layer and add it to the scroll layer
  article_text_layer = text_layer_create(max_text_bounds);
  scroll_layer_add_child(scroll_layer, text_layer_get_layer(article_text_layer));

  // Configure the text layer
  text_layer_set_text(article_text_layer, article_text_buffer);
  text_layer_set_font(article_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_background_color(article_text_layer, GColorBlack);
  text_layer_set_text_color(article_text_layer, GColorWhite);
  text_layer_set_text_alignment(article_text_layer, GTextAlignmentCenter);

  // Fit the text to the round screen with inset around edge of screen
  #ifdef PBL_ROUND
  int inset = 4;
  text_layer_enable_screen_text_flow_and_paging(article_text_layer, inset);
  #endif

  // Trim text layer and scroll content to fit text box
  GSize max_size = text_layer_get_content_size(article_text_layer);
  text_layer_set_size(article_text_layer, max_size);
  scroll_layer_set_content_size(scroll_layer, GSize(frame.size.w, max_size.h + 4));
}


static void article_window_unload(Window *window) {
  text_layer_destroy(article_text_layer);
}



// ------------------------------------------------------------------------------ //
//  Main and Init / Deinit Functions
// ------------------------------------------------------------------------------ //
static void init(void) {
  // Open and read the file
  open_articles_file();
  load_headlines();

  // Create the headline list window
  menu_window = window_create();
  window_set_window_handlers(menu_window, (WindowHandlers) {
    .load = menu_window_load,
    .unload = menu_window_unload
  });

  // Show the headline list window
  const bool animated = true;
  window_stack_push(menu_window, animated);

  // Create the article viewing window (don't show it yet)
  article_window = window_create();
  window_set_window_handlers(article_window, (WindowHandlers) {
    .load = article_window_load,
    .unload = article_window_unload
  });
}


static void deinit() {
  // Program is closing, destroy our windows
  window_destroy(article_window);
  window_destroy(menu_window);
}


int main(void) {
  init();
  app_event_loop();
  deinit();
}