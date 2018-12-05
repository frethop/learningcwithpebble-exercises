#include <pebble.h>

static Window *s_window;
static TextLayer *s_text_layer;

#define MY_DEBUG

#if defined(MY_DEBUG)
  #define MY_LOG(...)  printf(__VA_ARGS__)
  #define MY_ASSERT(x, ...)  if (!(x)) { printf(__VA_ARGS__); window_stack_pop_all(false); }
  #define PRINT_VAR(var, ...) \
     printf(strcat("DEBUG: %s in %s @ %d: ", __VA_ARGS__), #var, __FILE__, __LINE__, var)
#else
  #define MY_LOG(...)
  #define MY_ASSERT(x, ...)
  #define PRINT_VAR(var)
#endif

static void init(void) {
	// Create a window and get information about the window
	s_window = window_create();
  MY_ASSERT(s_window, "Failed to create window.");
  
  Layer *window_layer = window_get_root_layer(s_window);
  GRect bounds = layer_get_bounds(window_layer);
    
  MY_ASSERT(bounds.size.w > 144, "Pebble Time");
  PRINT_VAR(bounds.size.w, "%d%");
	
  // Create a text layer and set the text
	s_text_layer = text_layer_create(bounds);
  MY_ASSERT(s_text_layer, "Failed to create text layer.");
	text_layer_set_text(s_text_layer, "Hi, I'm a Pebble!");
  
  // Set the font and text alignment
	text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);

	// Add the text layer to the window
	layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_text_layer));
  
  // Enable text flow and paging on the text layer, with a slight inset of 10, for round screens
  text_layer_enable_screen_text_flow_and_paging(s_text_layer, 10);

	// Push the window, setting the window animation to 'true'
	window_stack_push(s_window, true);
  
  MY_LOG("Window and text layer created and ready to go.");
}

static void deinit(void) {
  MY_LOG("Destroying everything.");
  
	// Destroy the text layer
	text_layer_destroy(s_text_layer);
	
	// Destroy the window
	window_destroy(s_window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}
