//----------------------------------------------------------------------------
//
//  Pebble smartwatch side of Project 20.1 for "Learning C with Pebble"
//
//  Mike Jipping, August, 2016

#include <pebble.h>

static Window *window;	
static TextLayer *debt_text_layer;
static AppTimer *debt_timer;
	
// Called when a message is received from the JavaScript side
static void in_received_handler(DictionaryIterator *received, void *context) {
	Tuple *tuple;
    
	tuple = dict_find(received, MESSAGE_KEY_DEBT);
	if(tuple) {
        text_layer_set_text(debt_text_layer, tuple->value->cstring);
	} else {
        text_layer_set_text(debt_text_layer, "ERROR!");
    }
}

// Make a request for the national debt number
static void ask_for_debt(void *context) {
    DictionaryIterator *iter;
	
	uint32_t result = app_message_outbox_begin(&iter);
    if (result == APP_MSG_OK) {
	    dict_write_int8(iter, MESSAGE_KEY_ASK, 1);  // Gotta have a payload
	    dict_write_end(iter);
        app_message_outbox_send();
    }
    debt_timer = app_timer_register(5000, ask_for_debt, NULL);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
    app_timer_cancel(debt_timer);
    ask_for_debt(NULL);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
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
    debt_text_layer = text_layer_create(GRect(10,60, frame.size.w-20, frame.size.h-120));
    text_layer_set_font(debt_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
    text_layer_set_background_color(debt_text_layer, GColorWhite);
    text_layer_set_text_color(debt_text_layer, GColorRed);
    text_layer_set_text_alignment(debt_text_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(debt_text_layer));
    
    // Set the timer
    debt_timer = app_timer_register(5000, ask_for_debt, NULL);
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