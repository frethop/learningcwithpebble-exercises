//----------------------------------------------------------------------------------
//
// Project 12.1 from "Learning C with Pebble"
// 
// Mike Jipping, June 2016
//

#include <pebble.h>

#define FORWARD 1
#define BACKWARD -1

static Window *window;

static ResHandle codefile_handle;
static int codefilesize;

// Find a line feed in the file opened with handle.  Work in the direction (FORWARD or BACKWARD)
// until a line feed is found.
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

// Using "findlinefeed" above, randomly pick a position in the sentence file, work back to find a 
// linefeed, then work forward to find a linefeed.  Return the result.
char *get_random_sentence()
{ 
    uint8_t *ubuffer;
    int position;
    
    position = rand()%codefilesize;
    
    // Look for the line feeds that start and end a line.
    int start = findlinefeed(codefile_handle, position, codefilesize, BACKWARD);
    if (start != 0) start++;
    int end = findlinefeed(codefile_handle, position, codefilesize, FORWARD); 
    if (end <= start) end = findlinefeed(codefile_handle, position+1, codefilesize, FORWARD); 
    int length = end-start;

    // Get the line from the file
    ubuffer = (uint8_t *)malloc((length+1)*sizeof(uint8_t));
    resource_load_byte_range(codefile_handle, start, ubuffer, length);
    ubuffer[length] = '\0';

    // Copy the line and return it.
    char *buf = (char *)malloc(sizeof(char)*(length+1));
    strcpy(buf, (char *)ubuffer);
    free(ubuffer);
    return buf;
}

// Given a sentence with numbers and a position, extract the number and change it to an integer.
// Then extract the rotation and the actual character, rotate backwards, and correct if we went off 
// the alphabet range.
char decode_next_letter(char *sentence, int *position) { 
   int pos = 0;
   char numstring[8];
   char actual;
    
   // Get the number
   pos = 0;
   while (sentence[*position] != ' ' && sentence[*position] != '\0') {
       numstring[pos] = sentence[*position];
       pos = pos + 1;
       *position = *position + 1;
   }
   numstring[pos] = '\0';
   position ++;
   
   // If the string is empty, return an empty string.
   if (strlen(numstring) == 0) return '\0';
    
   // Get the integer
   int coded = atoi(numstring);
    
   // Decode the integer
   int rotate = (coded & 7936) >> 8;
   int charcode = coded & 255;

   actual = (char) (charcode - rotate);
   if (actual < 'A' && actual != ' ') {
       actual = 'Z' - ('A' - actual - 1);
   } else if (charcode >= 'a' && actual < 'a') {
       actual = 'z' - ('a' - actual - 1);
   }
    
   return actual;
}

// Using get_random_sentence, get a sentance and process the characters.
char *get_decoded_line() {
    char *buffer = get_random_sentence();
    char c = ' ';
    int position = 0;
    int newpos = 0;

    while (c != '\0') {
        c = decode_next_letter(buffer, &position);
        if (c == '\0')
            buffer[newpos] = '\0';
        else
            buffer[newpos] = c;
        newpos++;
        position ++;
    }
    
    return buffer;
}

// Open the resources / files.
void open_code_file() {
   codefile_handle = resource_get_handle(RESOURCE_ID_CEASAR); 
   codefilesize = resource_size(codefile_handle);
}

void update_display(struct Layer *layer, GContext *ctx) {
    char *code = get_decoded_line();
    
    // Load the font
    GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
    // Set the color
    graphics_context_set_text_color(ctx, GColorBlack);
    
    // Determine a reduced bounding box
    GRect layer_bounds = layer_get_bounds(window_get_root_layer(window));
    GRect bounds = GRect(layer_bounds.origin.x+5, layer_bounds.origin.y+5,
                         layer_bounds.size.w-10, layer_bounds.size.h-10);

    // Clear the window
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_fill_rect(ctx, layer_bounds, 0, GCornerNone);

    // Draw the text
    graphics_draw_text(ctx, code, font, bounds, GTextOverflowModeWordWrap, 
                                                     GTextAlignmentCenter, NULL);

}

//----------------------------------------------------------------------------------

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  layer_mark_dirty(window_get_root_layer(window));
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

static void window_load(Window *window) {
}

static void window_unload(Window *window) {
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
    
  layer_set_update_proc(window_get_root_layer(window), update_display);
  open_code_file();
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}