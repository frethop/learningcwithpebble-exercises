#include <pebble.h>


// Global Variables
static Window *main_window;        // Our program's window
TextLayer *text_layer;             // A layer to display text on the screen


// ------------------------------------------------------------------------ //
//  Accelerometer Function
// ------------------------------------------------------------------------ //

static void tap_handler(AccelAxisType axis, int32_t direction) {
  AccelData data;
  
  int err = accel_service_peek(&data);
  
  if (err==0) {
    unsigned int speed = 0;
    
    
    // Insert your speed detection code here
    
    
    APP_LOG(APP_LOG_LEVEL_INFO, "X = %d, Y = %d, and Z = %d", data.x, data.y, data.z);
    APP_LOG(APP_LOG_LEVEL_INFO, "Vibrate? %c, Speed: %d", data.did_vibrate ? 'Y' : 'N', speed);
    
    // Don't vibrate if already vibrating
    if(data.did_vibrate == false) {
      
      
      // Insert your vibration code here  
      
      
    }
  } else {
    APP_LOG(APP_LOG_LEVEL_INFO, "Error reading accelerometer data");
  }
}


// ------------------------------------------------------------------------ //
//  Main Functions
// ------------------------------------------------------------------------ //

static void init(void) {
  // App is starting, create our window
  main_window = window_create();
  
  // Get the size of our window
  Layer *window_layer = window_get_root_layer(main_window);
  GRect frame = layer_get_frame(window_layer);
  
  // Create a text layer and center it vertically and horizontally
  frame.origin.y = frame.size.h/2 - 7;
  text_layer = text_layer_create(frame);
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  
  text_layer_set_text(text_layer, "Shake to Vibrate!");
  layer_add_child(window_get_root_layer(main_window), 
                    text_layer_get_layer(text_layer));
  
  // Display our window
  window_stack_push(main_window, true /* Animated */);
  
  // Register for tap events
  accel_tap_service_subscribe(tap_handler);
}


static void deinit(void) {
  // App is closing, destroy our text layer and window
  text_layer_destroy(text_layer);
  window_destroy(main_window);
}


int main(void) {
  init();
  app_event_loop();
  deinit();
}