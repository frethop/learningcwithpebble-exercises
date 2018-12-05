//----------------------------------------------------------------------------
//
//  Pebble smartwatch side of Project 20.2 for "Learning C with Pebble"
//
//  Mike Jipping, August, 2016

#include <pebble.h>

static Window *window;	
static TextLayer *geo_text_layer;

char *geo_string;

// States for tracking various messages
enum states {
    NOT_WAITING,
    WAITING_FOR_LATLONG,
    WAITING_FOR_ADDRESS1,
    WAITING_FOR_ADDRESS2
} waiting;

// Called when a message is received from the JavaScript side
static void in_received_handler(DictionaryIterator *received, void *context) {
	Tuple *tuple;
    DictionaryIterator *iter;
    int longitude, latitude, altitude, accuracy;
    char *display;
    uint8_t latlong[4];
        
    // Here we wait for the latitude and longitude data.  We pull the data from the dictionary
    // and assemble a string for the display.
    if (waiting == WAITING_FOR_LATLONG) {
    	tuple = dict_find(received, MESSAGE_KEY_LATITUDE);
        latitude = tuple->value->int32;
    	tuple = dict_find(received, MESSAGE_KEY_LONGITUDE);
        longitude = tuple->value->int32;
    	tuple = dict_find(received, MESSAGE_KEY_ALTITUDE);
        altitude = tuple->value->int32;
    	tuple = dict_find(received, MESSAGE_KEY_ACCURACY);
        accuracy = tuple->value->int32;
        display = malloc(100*sizeof(char));
        snprintf(display, 100, "Longitude is %2d.%02d Latitude is %2d.%02d Altitude is %d meters Accuracy is within %d meters", 
                 longitude / 100, abs(longitude) % 100, 
                 latitude / 100, abs(latitude) % 100,
                 altitude, accuracy);
        text_layer_set_text(geo_text_layer, display);
        waiting = NOT_WAITING;

    // We receive latitude and longitude data for part ONE of the address receipt.  We then
    // encode this data in a new message/request.
    } else if (waiting == WAITING_FOR_ADDRESS1) {
    	tuple = dict_find(received, MESSAGE_KEY_LATITUDE);
        float lat = tuple->value->int32/100.0;
    	tuple = dict_find(received, MESSAGE_KEY_LONGITUDE);
        float lon = tuple->value->int32/100.0;
        latlong[0] = (int)lat;
        if (lat < 0) lat = -(lat);
        latlong[1] = (lat - (int)lat) * 100;
        latlong[2] = (int)lon;
        if (lon < 0) lon = -(lon);
        latlong[3] = (lon - (int)lon) * 100;
        uint32_t result = app_message_outbox_begin(&iter);
        if (result == APP_MSG_OK) {
	        dict_write_data(iter, MESSAGE_KEY_ADDRESS, latlong, 4);  
	        dict_write_end(iter);
            app_message_outbox_send();
            waiting = WAITING_FOR_ADDRESS2;
        }
        
   // Here we receive the string address for display on the smartwatch.
   }  else if (waiting == WAITING_FOR_ADDRESS2) {
    	tuple = dict_find(received, MESSAGE_KEY_STREETADDRESS);
        display = malloc(100*sizeof(char));
        snprintf(display, 100, "Your street address is %s", tuple->value->cstring);
        text_layer_set_text(geo_text_layer, display);
   }
}

// Make a request for latitude and longitude.
static void ask_for_latlong(void *context) {
    DictionaryIterator *iter;
	
	uint32_t result = app_message_outbox_begin(&iter);
    if (result == APP_MSG_OK) {
	    dict_write_int8(iter, MESSAGE_KEY_LATLONG, 8);  
	    dict_write_end(iter);
        app_message_outbox_send();
        waiting = WAITING_FOR_LATLONG;
    }
}

// Initiate the sequence for the address string.
static void ask_for_address(void *context) {
    DictionaryIterator *iter;
	
	uint32_t result = app_message_outbox_begin(&iter);
    if (result == APP_MSG_OK) {
	    dict_write_int8(iter, MESSAGE_KEY_LATLONG, 8);  
	    dict_write_end(iter);
        app_message_outbox_send();
        waiting = WAITING_FOR_ADDRESS1;
    }
}

//--------------------------------------------------------------------------------------

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
    ask_for_latlong(NULL);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
    ask_for_address(NULL);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void init(void) {
    window = window_create();

    Layer *window_layer = window_get_root_layer(window);
    GRect frame = layer_get_frame(window_layer);

    window_set_click_config_provider(window, click_config_provider);
    const bool animated = true;
    window_stack_push(window, animated);

    // Register AppMessage handlers
	app_message_register_inbox_received(in_received_handler); 

    // Initialize AppMessage inbox and outbox buffers with a suitable size
    const int inbox_size = 128;
    const int outbox_size = 128;
    app_message_open(inbox_size, outbox_size);

    // Configure the debt text layer
    geo_text_layer = text_layer_create(GRect(10,30, frame.size.w-20, frame.size.h-60));
    text_layer_set_font(geo_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
    text_layer_set_background_color(geo_text_layer, GColorWhite);
    text_layer_set_text_color(geo_text_layer, GColorRed);
    text_layer_set_text_alignment(geo_text_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(geo_text_layer));
}

static void deinit(void) {
	app_message_deregister_callbacks();
	window_destroy(window);
}

int main( void ) {
	init();
	app_event_loop();
	deinit();
}