#ifndef TYPES_H
#define TYPES_H

#include <time.h>
#include <ncurses.h>
#include <stdlib.h>


// represents a direction on a 2d map and at the same time the characters in the head of the snake
typedef enum {
  DIR_UP = '^',
  DIR_RIGHT = '>',
  DIR_DOWN = 'v',
  DIR_LEFT = '<'
} direction;


// delcare a game struct
struct game;

// represents a single fruit
typedef struct fruit {
  WINDOW *win;
  // the callback function which will be called if the snake hits this fruit
  void (*effect)(struct game*);
} FRUIT;

// represents a collection of fruit, which can be eaten by the snake
typedef struct fruits {
  FRUIT* fruits; // dynamic array of fruits
  int length; // the length of the dynamic array
} FRUITS;

// the snake
/*
typedef struct snake_part {
  WINDOW* part;
  snake_part* next;
}
  
typedef struct snake {
  snake_part* head; //snake head
  snake_part* tail; //snake tail
  int length; // length of the dynamic array
  direction dir; // the direction of the snake
  int grow; // the number of times the snake has to grow 
  int eat_range;
} SNAKE;
*/

typedef struct snake {
  WINDOW** parts; // dynamic array of window pointers
  int length; // length of the dynamic array
  direction dir; // the direction of the snake
  int grow; // the number of times the snake has to grow 
  int eat_range;
} SNAKE;

// a struct containing all the data needed for a game
typedef struct game {
  FRUITS fruits; // the collection of fruits on the screen
  SNAKE snake; // the snake
  time_t started; // timestamp when the game has been started
  time_t ended;  // timestamp when the has been ended
  time_t paused; // the amount of seconds the game was paused
  int highscore; // the number of points
  
  long long base_interval;  //base interval used for both x/y movement
  long long interval;  //actual interval based on x/y
  
  int rows;
  int columns;
} GAME;

// the maximum length of a name in the highscore
#define HIGHSCORE_NAME_LENGTH 15
#define DEFAULT_INTERVAL 100000000

typedef struct highscore {
  char name[HIGHSCORE_NAME_LENGTH]; // the name of the player who made this score
  int points; // the points he collected (from game.highscore)
  long time_sec; // the number of seconds he needed to collect his points
  int highscore; // the calculated score out of the time and the points (points * 10 - time * 2)
} HIGHSCORE;

// we probably need some logging functionality
#include "glog.h"

#endif /* TYPES_H */
