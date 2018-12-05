//----------------------------------------------------------------------------------
//
// Project 8.3 from "Programming Pebble in C"
// 
// Mathew Reiss with Mike Jipping, March 2016

#include <pebble.h>

Layer *layer;
Window *window;
GBitmap *old_image, *image;

uint8_t *bitmap_data;
int bytes_per_row;

// Set the correct screen height and width (checking for Pebble Time Round)
int HEIGHT = PBL_IF_RECT_ELSE(168,180);
int WIDTH = PBL_IF_RECT_ELSE(144,180);

// Set the height and width of the image
int IMAGE_HEIGHT = 76;
int IMAGE_WIDTH = 56;

// Get the color of the pixel at (x,y)
GColor get_pixel_color(int x, int y){
    uint8_t *bmd = bitmap_data;
    bmd += y*bytes_per_row + x;
	return (GColor){ .argb = *bmd };
}

// Set the color of the pixel at (x,y) to "color"
void set_pixel_color(int x, int y, GColor color){
    uint8_t *bmd = bitmap_data;
    bmd += y*bytes_per_row + x;
	*bmd = color.argb;
}

// Go through pixels using a single loop and replace old_color with new_color
void replace_colors(int pixel_width, int pixel_height, GColor old_color, GColor new_color){
	int max_y = pixel_height; //Use 28 for just the hat
	int max_x = pixel_width;
	
    uint8_t *bmd = bitmap_data;
    
    for (int d=0; d < max_x*max_y; d++) {
        GColor pixel_color = (GColor){ .argb = *bmd };
        if (gcolor_equal(pixel_color, old_color))
            *bmd = new_color.argb;
        bmd++;
    }
}

// Draw two images with a line separating them
void draw(Layer *layer, GContext *ctx){
	graphics_context_set_compositing_mode(ctx, GCompOpSet);
	graphics_draw_bitmap_in_rect(ctx, old_image, GRect((WIDTH-IMAGE_WIDTH)/2, 4, IMAGE_WIDTH, IMAGE_HEIGHT));
	graphics_draw_bitmap_in_rect(ctx, image, GRect((WIDTH-IMAGE_WIDTH)/2, HEIGHT-4-IMAGE_HEIGHT, IMAGE_WIDTH, IMAGE_HEIGHT));
	graphics_context_set_stroke_color(ctx, GColorBlack);
	graphics_draw_line(ctx, GPoint(0,HEIGHT/2), GPoint(WIDTH,HEIGHT/2));
}

//---------------------------------------------------------------------------------------------

void handle_init(){
	window = window_create();
	layer = layer_create(layer_get_bounds(window_get_root_layer(window)));
	layer_set_update_proc(layer, draw);
	layer_add_child(window_get_root_layer(window), layer);
	
	image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE);
	old_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE);
	
	bitmap_data = gbitmap_get_data(image);
	bytes_per_row = gbitmap_get_bytes_per_row(image);
	
	replace_colors(IMAGE_WIDTH, IMAGE_HEIGHT, GColorRed, GColorBlue);
	replace_colors(IMAGE_WIDTH, IMAGE_HEIGHT, GColorDarkCandyAppleRed, GColorOxfordBlue);
	
	window_stack_push(window, true);
	
	app_event_loop();
}

int main(void) {handle_init();}