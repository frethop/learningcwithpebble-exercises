//----------------------------------------------------------------------------------
//
// Project 7.3 from "Programming Pebble in C"
// 
// Rob Spiess, March 2016

#include <pebble.h>


// Hardcoded constants for the project
#define MAZE_HEIGHT 13   // Height of maze
#define MAZE_WIDTH  11   // Width of maze
#define CELL_SIZE   10   // Pixel size of each maze cell


// Needed for Pebble graphics
Window *window;
Layer *maze_layer, *solution_layer;


// Declarations for the project
bool maze[MAZE_WIDTH][MAZE_HEIGHT];
bool solved_maze[MAZE_WIDTH][MAZE_HEIGHT];

int start_x, start_y;     // Start of maze
int finish_x, finish_y;   // End of maze


// ------------------------------------------------------------------------ //
//  Maze Generating Functions
// ------------------------------------------------------------------------ //
void clear_maze() {
  for (int x=0; x<MAZE_WIDTH; x++) {
    for (int y=0; y<MAZE_HEIGHT; y++) {
      maze[x][y] = true; // Fill all spaces in maze with solid walls
    }
  }
}


// Returns if an x,y maze location is available for generating the maze
bool can_go(int x, int y) {
  if (x<1 || x>=MAZE_WIDTH-1 || y<1 || y>=MAZE_HEIGHT-1)  // If it's a border spot (or beyond)
    return false;    // You cannot go there
  return maze[x][y]; // If the spot has already been cleared (false), you cannot revisit it. Else (true) you can.
}


// Generate a maze (recursively)
//   Generates a maze using the Recursive Backtracker method:
//   https://en.wikipedia.org/wiki/Maze_generation_algorithm#Recursive_backtracker
void generate_maze(int x, int y) {
  maze[x][y] = false;  // clear the spot in current position

  while(can_go(x, y - 2) || can_go(x, y + 2) || can_go(x - 2, y) || can_go(x + 2, y)) {  // While there exists a new position you can go to
    switch(rand()%4) {  // Randomly choose a direction to go to
      case 0:  // Try going up
      if(can_go(x, y - 2)) {  // If you can go up
        maze[x][y-1] = false;  // remove wall above current position
        generate_maze(x, y-2);  // move up two spaces
      }
      break;
      
      case 1:  // Try going down
      if(can_go(x, y + 2)) {   // If you can go down
        maze[x][y+1] = false;   // remove wall below current position
        generate_maze(x, y + 2); // move down two spaces
      }
      break;
      
      case 2:  // Try going left
      if(can_go(x - 2, y)) {  // If you can go left
        maze[x-1][y] = false;  // remove wall left of current position
        generate_maze(x-2, y);  // move left two spaces
      }
      break;
      
      case 3:  // Try going right
      if(can_go(x + 2, y)) {  // If you can go right
        maze[x+1][y] = false;  // remove wall right of current position
        generate_maze(x+2, y);  // move right two spaces
      }
      break;
    }
  }
}


// ------------------------------------------------------------------------ //
//  Maze Solving Functions
// ------------------------------------------------------------------------ //
// Fill all solutions spaces in maze with blank "Not a solution" spaces
void clear_solution() {
  for (int x=0; x<MAZE_WIDTH; x++) {
    for (int y=0; y<MAZE_HEIGHT; y++) {
      solved_maze[x][y] = false;
    }
  }
}


// Solve the maze (recursively)
// Start at x and y, Finish at global variables finish_x and finish_y
// returns true if solved, returns false if unsolvable
bool solve_maze(int x, int y) {

  // [Put your maze solving code here]
  
  return false;               // Not solved, return false
}

// ------------------------------------------------------------------------ //
//  Button Functions
// ------------------------------------------------------------------------ //
void up_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  // Solve current maze
  clear_solution();
  solve_maze(start_x, start_y);
  layer_mark_dirty(solution_layer);
}


void select_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  // Generate and Solve a new maze
  clear_solution();
  clear_maze();
  generate_maze(start_x, start_y);
  solve_maze(start_x, start_y);
  layer_mark_dirty(maze_layer);
  layer_mark_dirty(solution_layer);
}


void down_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  // Generate new maze but don't solve it
  clear_solution();
  clear_maze();
  generate_maze(start_x, start_y);
  layer_mark_dirty(maze_layer);
  layer_mark_dirty(solution_layer);
}


void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_single_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_single_click_handler);
}

// ------------------------------------------------------------------------ //
//  Drawing Functions
// ------------------------------------------------------------------------ //
static void solution_layer_update(Layer *me, GContext *ctx) {
  // Draw the solution
  graphics_context_set_fill_color(ctx, PBL_IF_BW_ELSE(GColorLightGray, GColorRed));

  // [Put your maze solution drawing code here]
  
}


static void maze_layer_update(Layer *me, GContext *ctx) {
  GRect cell;
  // Draw the maze
  
  // [Put your maze drawing code here]
  
  // Draw the start entrance
  graphics_context_set_fill_color(ctx, PBL_IF_BW_ELSE(GColorLightGray, GColorRed));
  cell = GRect(start_x * CELL_SIZE, start_y * CELL_SIZE, CELL_SIZE, CELL_SIZE);
  graphics_fill_rect(ctx, cell, 0, GCornerNone);
  
  // Draw the finish exit
  graphics_context_set_fill_color(ctx, PBL_IF_BW_ELSE(GColorLightGray, GColorRed));
  cell = GRect(finish_x * CELL_SIZE, finish_y * CELL_SIZE, CELL_SIZE, CELL_SIZE);
  graphics_fill_rect(ctx, cell, 0, GCornerNone);
}

// ------------------------------------------------------------------------ //
//  Main Functions
// ------------------------------------------------------------------------ //
static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_frame = layer_get_frame(window_layer);
  GRect maze_frame;
  
  // Size of the maze on the screen
  maze_frame.size.w = CELL_SIZE * MAZE_WIDTH;
  maze_frame.size.h = CELL_SIZE * MAZE_HEIGHT;
  
  // Center the maze horizontally and vertically
  maze_frame.origin.x = (window_frame.size.w - maze_frame.size.w) / 2;
  maze_frame.origin.y = (window_frame.size.h - maze_frame.size.h) / 2;

  // Create Maze Drawing Layer
  maze_layer = layer_create(maze_frame);
  layer_set_update_proc(maze_layer, maze_layer_update);
  layer_add_child(window_layer, maze_layer);

  // Create Solution Drawing Layer
  solution_layer = layer_create(maze_frame);
  layer_set_update_proc(solution_layer, solution_layer_update);
  layer_add_child(window_layer, solution_layer);
}


static void window_unload(Window *window) {
  layer_destroy(maze_layer);
  layer_destroy(solution_layer);
}


static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
  window_stack_push(window, true /* Animated */);
  window_set_click_config_provider(window, click_config_provider);
  window_set_background_color(window, GColorDukeBlue);

  // Set the start of the maze: Upper Left Corner
  start_x = 1;
  start_y = 1;

  // Set the end of the maze: Bottom Right Corner
  finish_x = ((MAZE_WIDTH  + 1) / 2) * 2 - 3;
  finish_y = ((MAZE_HEIGHT + 1) / 2) * 2 - 3;

  // Blank the maze and solution and generate a starting maze
  clear_solution();
  clear_maze();
  generate_maze(start_x, start_y);
  
  srand(time(NULL));  // Seed randomizer (to make maze different every time)
}


static void deinit(void) {
  window_destroy(window);
}


int main(void) {
  init();
  app_event_loop();
  deinit();
}