#include <pebble.h>

Layer *layer;
Window *window;
GBitmap *old_image, *image;

uint8_t *bitmap_data;
int bytes_per_row;

int HEIGHT = PBL_IF_RECT_ELSE(168,180);
int WIDTH = PBL_IF_RECT_ELSE(144,180);

int IMAGE_HEIGHT = 76;
int IMAGE_WIDTH = 56;

GColor get_pixel_color(int x, int y){
	return (GColor){ .argb = bitmap_data[y*bytes_per_row + x] };
}

void set_pixel_color(int x, int y, GColor color){
	bitmap_data[y*bytes_per_row + x] = color.argb;
}

void replace_colors(int pixel_width, int pixel_height, GColor old_color, GColor new_color){

}

void draw(Layer *layer, GContext *ctx){
	graphics_context_set_compositing_mode(ctx, GCompOpSet);
	graphics_draw_bitmap_in_rect(ctx, old_image, GRect((WIDTH-IMAGE_WIDTH)/2, 4, IMAGE_WIDTH, IMAGE_HEIGHT));
	graphics_draw_bitmap_in_rect(ctx, image, GRect((WIDTH-IMAGE_WIDTH)/2, HEIGHT-4-IMAGE_HEIGHT, IMAGE_WIDTH, IMAGE_HEIGHT));
	graphics_context_set_stroke_color(ctx, GColorBlack);
	graphics_draw_line(ctx, GPoint(0,HEIGHT/2), GPoint(WIDTH,HEIGHT/2));
}

void handle_init(){
	window = window_create();
	layer = layer_create(layer_get_bounds(window_get_root_layer(window)));
	layer_set_update_proc(layer, draw);
	layer_add_child(window_get_root_layer(window), layer);
	
	image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE);
	old_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE);
	
	bitmap_data = gbitmap_get_data(image);
	bytes_per_row = gbitmap_get_bytes_per_row(image);
	
	replace_colors(56, 76, GColorRed, GColorBlue);
	replace_colors(56, 76, GColorDarkCandyAppleRed, GColorOxfordBlue);
	
	window_stack_push(window, true);
	
	//light_enable(true);
	
	app_event_loop();
}

int main(void) {handle_init();}
