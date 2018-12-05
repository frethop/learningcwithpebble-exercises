#include <pebble.h>

#define COLOR_FOREGROUND GColorWhite
#define COLOR_BACKGROUND GColorBlue
#define COLOR_FRUIT GColorGreen

#define HEIGHT 18
#define WIDTH 15
#define PADDING 2
#define CELL_HEIGHT 6
#define CELL_WIDTH 6
#define BORDER_PADDING 2

#define TICK_TIME_MS 600

#define X_TOP 12
#define Y_TOP 12

#define NUM_CELLS (HEIGHT * WIDTH)

#define SNAKE_HEAD_START_X 10
#define SNAKE_HEAD_START_Y 10

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3
#define DIRECTION_COUNT (LEFT + 1)

#define CLOCKWISE -1
#define COUNTER_CLOCKWISE 1

#define SNAKE_MAX_LENGTH NUM_CELLS

typedef struct {
  unsigned short x;
  unsigned short y;
} Position;

typedef struct {
  Position body[SNAKE_MAX_LENGTH];
  unsigned short direction;
  unsigned short length;
} Snake;

static Window *window;

Snake snake;
Position fruit;
Position old_position;
unsigned short game_running;

// Get a GPoint from the struct Position
GPoint get_point_from_position(Position *pos) {
  unsigned short x_start;
  x_start = X_TOP + BORDER_PADDING + pos->x * (CELL_HEIGHT + PADDING);
  unsigned short y_start;
  y_start = Y_TOP + BORDER_PADDING + pos->y * (CELL_WIDTH + PADDING);

  GPoint pt = {.x = x_start, .y = y_start};
  return pt;
}

// Draw the fruit square on the screen
void draw_fruit(GContext *ctx) {
  GPoint pos = get_point_from_position(&fruit);
  pos.x++;
  pos.y++;
    
  GRect fruitRect = { .origin = pos,  .size = { .w = CELL_WIDTH - 2, .h = CELL_HEIGHT - 2}};

  graphics_context_set_stroke_color(ctx, COLOR_FOREGROUND);
  graphics_context_set_fill_color(ctx, COLOR_FRUIT);
  graphics_fill_rect(ctx, fruitRect, 0, GCornerNone);
}

// Draw the snake.  This will take several square PLUS a possible erase
void draw_snake(GContext *ctx) {
  GRect snakeRect;
  graphics_context_set_fill_color(ctx, COLOR_FOREGROUND);
  graphics_context_set_stroke_color(ctx, COLOR_FOREGROUND);
  for (int i = 0; i < snake.length; i++) {
    snakeRect.origin = get_point_from_position(&snake.body[i]);
    snakeRect.size.w = CELL_WIDTH;
    snakeRect.size.h = CELL_HEIGHT;
    graphics_fill_rect(ctx, snakeRect, 0, GCornerNone);
  }
  
  // Possible erase a square
  if (old_position.x != 0 && old_position.y != 0) {
     graphics_context_set_stroke_color(ctx, COLOR_BACKGROUND);
     graphics_context_set_fill_color(ctx, COLOR_BACKGROUND);
     snakeRect.origin = get_point_from_position(&old_position);
     snakeRect.size.w = CELL_WIDTH;
     snakeRect.size.h = CELL_HEIGHT;
     graphics_fill_rect(ctx, snakeRect, 0, GCornerNone);
  }
}

// Draw a border around the screen.  Cross the border and die!
void draw_border(GContext *ctx) {
  unsigned short full_width = WIDTH * (CELL_WIDTH + PADDING) + PADDING + (BORDER_PADDING * 2);
  unsigned short full_height = HEIGHT * (CELL_HEIGHT + PADDING) + PADDING + (BORDER_PADDING * 2);

  graphics_context_set_stroke_color(ctx, COLOR_FOREGROUND);
  GRect bRect = { .origin = GPoint(X_TOP - BORDER_PADDING, Y_TOP - BORDER_PADDING),  
                 .size = { .w = full_width, .h = full_height}};
  graphics_draw_rect(ctx, bRect);
}

// Generate a random coordinate for the fruit
void gen_fruit_position() {
  unsigned short cell = rand() % NUM_CELLS;
  fruit.x = cell % WIDTH;
  fruit.y = cell / WIDTH;
}

// Convenience function to copy one snake square to another
void copy_position(unsigned short to, unsigned short from) {
  snake.body[to].x = snake.body[from].x;
  snake.body[to].y = snake.body[from].y;
}

// More explanatory convenience function
unsigned short is_same_position(Position *a, Position *b) {
  return (a->x == b->x && a->y == b->y);
}

// Explanatory convenience function
unsigned short is_out_of_bounds(Position *pos) {
  return (pos->x >= WIDTH || pos->y >= HEIGHT);
}

// Is the game over?  Return an integer to represent a boolean
unsigned short is_game_over() {
  
  // First, are we out of bounds.  Check the head.
  if (is_out_of_bounds(&snake.body[0])) {
    return true;
  }

  // Check each square of the snake.  Did any square CROSS another square
  // Use the field array to check if we have a square at that spot.
  unsigned short field[NUM_CELLS] = {0};
  for (int i = 0; i < snake.length; i++) {
    unsigned short pos = snake.body[i].x * WIDTH + snake.body[i].y;

    if (field[pos]) {  // that square is occupied
      return true;
    }

    field[pos] = true;
  }

  // Finally, if we are at maximum length (i.e., the screen is full of snake)
  if (snake.length == SNAKE_MAX_LENGTH) {
    return true;
  }

  return false;
}

// Move the game forward one move
void tick_game() {
    
  // Stop if we are not running at all
  if (!game_running) {
    return;
  }

  // Record the old tail
  old_position.x = snake.body[snake.length-1].x;
  old_position.y = snake.body[snake.length-1].y;

  // Copy each square to the one closer to the tail.
  for (unsigned short i = snake.length - 1; i > 0; i--) {
    copy_position(i, i - 1);
  }

  // And ... reset the head.
  if (snake.direction == UP) {
    snake.body[0].y--;
  } else if (snake.direction == RIGHT) {
    snake.body[0].x++;
  } else if (snake.direction == DOWN) {
    snake.body[0].y++;
  } else if (snake.direction == LEFT) {
    snake.body[0].x--;
  } else {
    // Can't touch this.
  }

  // Let the user know the game is done. 
  if (is_game_over()) {
    game_running = false;
    vibes_double_pulse();
  }

  // Is the snake head on top of the fruit?
  if (is_same_position(&snake.body[0], &fruit)) {
    // Eat it.
    vibes_short_pulse();
    gen_fruit_position();
    // And grow
    snake.length++;
    snake.body[snake.length-1] = old_position;
  }
}

// Suit up and get him ready to go.
void init_snake() {
  snake.direction = RIGHT;

  snake.body[0].x = SNAKE_HEAD_START_X;
  snake.body[0].y = SNAKE_HEAD_START_Y;

  snake.body[1].x = SNAKE_HEAD_START_X - 1;
  snake.body[1].y = SNAKE_HEAD_START_Y;

  snake.body[2].x = SNAKE_HEAD_START_X - 2;
  snake.body[2].y = SNAKE_HEAD_START_Y;

  snake.length = 3;

  old_position.x = 0;
  old_position.y = 0;
}

// Reset snake and fruit...
void reset_game() {
  init_snake();
  gen_fruit_position();
  game_running = true;
}

// When the timer goes off, redraw the window
void refresh_timer_callback(void *data) {
	layer_mark_dirty(window_get_root_layer(window)); // Tell the layer it needs to redraw.
}

// Redraw the window: move the game, fill the window, then draw the border+fruit+snake
void update_display(struct Layer *layer, GContext *ctx) {
  tick_game();
    
  graphics_context_set_fill_color(ctx, COLOR_BACKGROUND);
  graphics_fill_rect(ctx, (GRect) { .origin = { .x = 0, .y = 0}, .size = { .w = 144, .h = 168}}, 0, GCornerNone);

  draw_border(ctx);
  draw_fruit(ctx);
  draw_snake(ctx);
    
  app_timer_register(TICK_TIME_MS, refresh_timer_callback, NULL);
}

//----------------------------------------------------------------------------------

void change_snake_direction(short direction) {
  snake.direction = (snake.direction + DIRECTION_COUNT - direction) % DIRECTION_COUNT;
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
    // switch to autopilot mode
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
    change_snake_direction(COUNTER_CLOCKWISE);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
    change_snake_direction(CLOCKWISE);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  layer_set_update_proc(window_get_root_layer(window), update_display);
  const bool animated = true;
  window_stack_push(window, animated);
    
  time_t now = time(NULL);
  srand(now);
    
  reset_game();
    
  layer_mark_dirty(window_get_root_layer(window));
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}