//----------------------------------------------------------------------------------
//
// Project 6.3 from "Programming Pebble in C"
// 
// Mike Jipping, February 2016

#include <pebble.h>

// Needed for access the screen with text and graphics.
Window *my_window;
TextLayer *text_layer;
static Layer *graphics_layer;

// Define your code below here
void fill_the_rectangle(GContext *ctx, int width, int height, char *str, GFont const font) {
    int x, y;
    
    // Draw the rectangle
    graphics_draw_rect(ctx, GRect(20,20,width, height));
    
    // Get the size of the text in the font requested.
    GSize size = graphics_text_layout_get_content_size(str, font, 
                                                       GRect(0,0,width, height), 
                                                       GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Size of the string is %d x %d", size.w, size.h);
    
    // The text is padded quite a bit.  This tightens the box around the text.
    size.w = size.w - (0.15*size.w);
    size.h = size.h - (0.3*size.h);
    
    // Set the color of the text to draw black.
    graphics_context_set_text_color(ctx, GColorBlack);
    
    // Now cram as many text pieces as will fit!
    y = 20;
    while (y < height) {
        x = 20;
        while (x < width) {
            graphics_draw_text(ctx, str, font, 
                     GRect(x, y, size.w, size.h), 
                     GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
            x += size.w;
        }
        y += size.h;
    }
    
}

//----------------------------------------------------------------------
static void graphics_layer_update_callback(Layer *me, GContext *ctx) { 
    fill_the_rectangle(ctx, 100, 80, "Fill it up!", fonts_get_system_font(FONT_KEY_GOTHIC_24));
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect frame = layer_get_frame(window_layer);

  graphics_layer = layer_create(frame);
  layer_set_update_proc(graphics_layer, graphics_layer_update_callback);
  layer_add_child(window_layer, graphics_layer);
    
}

static void window_unload(Window *window) {
  layer_destroy(graphics_layer);
}

void handle_init(void) {
  my_window = window_create();

  window_set_window_handlers(my_window, (WindowHandlers) {
	.load = window_load,
    .unload = window_unload,
  });

  text_layer = text_layer_create(GRect(0, 0, 144, 20));
  window_stack_push(my_window, true);
}

void handle_deinit(void) {
  text_layer_destroy(text_layer);
  window_destroy(my_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
