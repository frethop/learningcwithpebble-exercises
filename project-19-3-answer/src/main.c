//----------------------------------------------------------------------------------
//
// Project 19.3 from "Programming Pebble in C"
// 
// Mike Jipping, August 2016

#include <pebble.h>

Layer *layer;
Window *window;
GBitmap *image;
GPoint center;

uint8_t *bitmap_data;
int bytes_per_row, images_per_row;

// Set the correct screen height and width (checking for Pebble Time Round)
int HEIGHT = PBL_IF_RECT_ELSE(168,180);
int WIDTH = PBL_IF_RECT_ELSE(144,180);

// The height and width of the image
int IMAGE_HEIGHT;
int IMAGE_WIDTH;

int numLittleGuys;

// Here is where the heart of drawing is.  Figure out how many bitmaps fit on the watch face
// and do it computationally.  
void draw(Layer *layer, GContext *ctx){
    GRect bounds = gbitmap_get_bounds(image);
    IMAGE_WIDTH = bounds.size.w;
    IMAGE_HEIGHT = bounds.size.h;
    
    int num_rows = (numLittleGuys-1) / images_per_row + 1;
    int x, y, per_row;
    
    if (num_rows * IMAGE_HEIGHT > HEIGHT) num_rows = HEIGHT / IMAGE_HEIGHT;
    
	graphics_context_set_compositing_mode(ctx, GCompOpSet);
    for (int row = 0; row < num_rows; row++) {
        per_row = row < num_rows-1?images_per_row:numLittleGuys - images_per_row*row;
        if (per_row > images_per_row) per_row = images_per_row;
        for (int little_guys = 0; little_guys < per_row; little_guys++) {
            x = center.x - (per_row * IMAGE_WIDTH / 2) + (little_guys * IMAGE_WIDTH);
            y = center.y - (num_rows * IMAGE_HEIGHT / 2) + (row * IMAGE_HEIGHT);
        	graphics_draw_bitmap_in_rect(ctx, image, GRect(x, y, IMAGE_WIDTH, IMAGE_HEIGHT));
        }
    }    
}

//---------------------------------------------------------------------------------------------

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
    numLittleGuys ++;
    layer_mark_dirty(layer);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
    numLittleGuys --;
    layer_mark_dirty(layer);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}


void handle_init(){
	window = window_create();
	layer = layer_create(layer_get_bounds(window_get_root_layer(window)));
	layer_set_update_proc(layer, draw);
	layer_add_child(window_get_root_layer(window), layer);
	
    window_set_click_config_provider(window, click_config_provider);

    image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE);
	bitmap_data = gbitmap_get_data(image);
	bytes_per_row = gbitmap_get_bytes_per_row(image);
    images_per_row = WIDTH / bytes_per_row;	
    numLittleGuys = 1;
    center.x = WIDTH / 2;
    center.y = HEIGHT / 2;
    	
	window_stack_push(window, true);
	
	app_event_loop();
}

int main(void) {handle_init();}