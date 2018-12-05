//----------------------------------------------------------------------------------
//
// Project 9.3 from "Learning C with Pebble"
// 
// Mike Jipping, June 2016

#include <pebble.h>

#define FORWARD 1
#define BACKWARD -1

static Window *window;
static TextLayer *text_layer;

static ResHandle madlib_handle;
static size_t madlibsize;
static ResHandle noun_handle;
static size_t nounsize;
static ResHandle verb_handle;
static size_t verbsize;
static ResHandle adjective_handle;
static size_t adjectivesize;

enum Grammar {NOUN, VERB, ADJECTIVE, MADLIB}; 

char madlibstr[100]; 

// Find the first occurence of pattern after the start position in string
int index_of(char *string, char *pattern, int start) {
    int position = -1;
    int plength = strlen(pattern);
    int slength = strlen(string);
    for (int i=start; i<slength; i++) {
        if (string[i] == pattern[0]) {
            int j=0;
            position = i;
            for (j=0; j<plength; j++) {
                if (string[i+j] != pattern[j]) {
                    position = -1;
                    break;
                }
            }
            if (j == plength) break;
        }
    }
    return position;
}

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

// Find a random THING (a Grammar item).  Look in the appropriate file, start with a 
// random position, move backward and forward to find the line feeds.  THINGs are in the
// files one per line.
char *random_thing(enum Grammar item) { 
    ResHandle handle;
    uint8_t *ubuffer;
    int position, size;
    
    switch (item) {
        case NOUN:
            position = rand()%nounsize;
            handle = noun_handle;
            size = nounsize;
            break;
        case VERB:
            position = rand()%verbsize;
            handle = verb_handle;
            size = verbsize;
            break;
        case ADJECTIVE:
            position = rand()%adjectivesize;
            handle = adjective_handle;
            size = adjectivesize;
            break;
        case MADLIB:
            position = rand()%madlibsize;
            handle = madlib_handle;
            size = madlibsize;
            break;
        default:
            position = rand()%madlibsize;
            handle = madlib_handle;
            size = madlibsize;
            break;     
    }
    
    // Look for the line feeds that start and end a line.
    int start = findlinefeed(handle, position, size, BACKWARD);
    if (start != 0) start++;
    int end = findlinefeed(handle, position, size, FORWARD); 
    if (end <= start) end = findlinefeed(handle, position+1, size, FORWARD); 
    int length = end-start;

    // Get the line from the file
    ubuffer = (uint8_t *)malloc((length+1)*sizeof(uint8_t));
    resource_load_byte_range(handle, start, ubuffer, length);
    ubuffer[length] = '\0';

    // Copy the line and return it.
    char *buf = (char *)malloc(sizeof(char)*(length+1));
    strcpy(buf, (char *)ubuffer);
    free(ubuffer);
    return buf;
}

// Open the resources / files.
void open_madlibs_files() {
   // Madlib resource handle
   madlib_handle = resource_get_handle(RESOURCE_ID_MADLIBS);
   madlibsize = resource_size(madlib_handle);
        
   // Noun file resource handle
   noun_handle = resource_get_handle(RESOURCE_ID_NOUNS);
   nounsize = resource_size(noun_handle);
    
   // Verb file resource handle
   verb_handle = resource_get_handle(RESOURCE_ID_VERBS);
   verbsize = resource_size(verb_handle);

   // Adjective file resource handle
   adjective_handle = resource_get_handle(RESOURCE_ID_ADJECTIVES);
   adjectivesize = resource_size(adjective_handle);
    
}

// Replace the original element with the replacement as given in the parameters.
void replace_string(char *string, char *original, char *replacement) {
    int slength = strlen(string);
    int olength = strlen(original);
    int rlength = strlen(replacement);
    int index;
    
    index = index_of(string, original, 0);
    if (index < 0) return;
    
    char *newstring = (char *)malloc(sizeof(char)*(slength+rlength-olength+1));
    int newpos = 0;   
    int srcpos = 0;

    while (index >= 0) {
        // Copy up to the index
        for (; srcpos<index; srcpos++,newpos++) {
            newstring[newpos] = string[srcpos];
        }
        
        // Copy from the replacement
        for (int i=0; i<rlength; i++,newpos++) {
           newstring[newpos] = replacement[i];
        }
        
        // Adjust the old position and look again
        srcpos += olength;
        index = index_of(string, original, srcpos);
    } 
    
    // Copy the rest of the string
    for (; srcpos<slength; newpos++,srcpos++)
        newstring[newpos] = string[srcpos];
    newstring[newpos] = '\0';    

    strcpy(string, newstring);
    free(newstring);
}

// Replace all the elements in the madlib string and display it by marking
// the window dirty
void display_madlib() {
    char *thing;

    strcpy(madlibstr,random_thing(MADLIB));

    thing = random_thing(NOUN);
    replace_string(madlibstr, "<NOUN>", thing);
    free(thing);

    thing = random_thing(NOUN);
    replace_string(madlibstr, "<NOUN2>", thing);
    free(thing);
    
    thing = random_thing(VERB);
    replace_string(madlibstr, "<VERB>", thing);
    free(thing);

    thing = random_thing(ADJECTIVE);
    replace_string(madlibstr, "<ADJECTIVE>", thing);
    free(thing);

    layer_mark_dirty(window_get_root_layer(window));
}

void update_display(struct Layer *layer, GContext *ctx) {
    
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
    graphics_draw_text(ctx, madlibstr, font, bounds, GTextOverflowModeWordWrap, 
                                                     GTextAlignmentCenter, NULL);

}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  display_madlib();
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
  srand(time(NULL));
  open_madlibs_files();
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}