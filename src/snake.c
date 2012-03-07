#include "snake.h"
#include "events.h"

WINDOW *snake_part_is_on(SNAKE *snake, int posy, int posx) {
  int i;
  int cury, curx;
  for(i = 0; i < snake->length; i++) {
    getbegyx(snake->parts[i], cury, curx);
    if(cury == posy && curx == posx) {
      return snake->parts[i];
    }
  }
  return NULL;
}

void grow_snake(SNAKE *snake, int posy, int posx) {
  char schr = 'O';
  WINDOW *win;
  if(snake->length == 0) {
    snake->parts = malloc(sizeof(WINDOW*) * ++snake->length);
    schr = '0';
  } else {
    snake->parts = realloc(snake->parts, sizeof(WINDOW*) * ++snake->length);
  }
  
  snake->parts[snake->length - 1] = win = newwin(1, 1, posy, posx);
  wprintw(win, "%c", schr);
  wrefresh(win);
}

void kill_snake(SNAKE *snake) {
  int i;
  for(i = 0; i < snake->length; i++) {
    delwin(snake->parts[0]);
  }
  free(snake->parts);
}


int move_snake(GAME *game) {
  int (*collision_checks[EVENTS]) (GAME*, int, int) = {
    check_self_collision,
    check_border_collision,
    check_fruit_collision
  };

  int (*collision_handlers[EVENTS])(GAME*, int, int) = {
    check_self_collision_handler,
    check_border_collision_handler,
    check_fruit_collision_handler
  };
  int success = 1;
  int i;
  int test = 0;
  int xdiff = game->snake.dir == DIR_LEFT ? -1 : (game->snake.dir == DIR_RIGHT ? 1 : 0);
  int ydiff = game->snake.dir == DIR_UP ? -1 : (game->snake.dir == DIR_DOWN ? 1 : 0);
  glog("diff y: %i x: %i", ydiff, xdiff);
  int curx;
  int cury;
  int tmpy;
  int tmpx;
  getbegyx(game->snake.parts[0], cury, curx);
  tmpy = cury;
  tmpx = curx;
  cury += ydiff;
  curx += xdiff;
  for(i = 0; i < EVENTS && success; i++) {
    if(test = collision_checks[i](game, cury, curx)) {
      if(!collision_handlers[i](game, cury, curx)) {
        success = 0;
      }
    }
  }
  if(success) {
    glog("part0 y: %i, x: %i", cury, curx);
    mvwin(game->snake.parts[0], cury, curx);
    wsyncup(game->snake.parts[0]);
    wrefresh(game->snake.parts[0]);
    cury = tmpy;
    curx = tmpx;
    for(i = 1; i < game->snake.length; i++) {
      getbegyx(game->snake.parts[i], tmpy, tmpx);
      mvwin(game->snake.parts[i], cury, curx);
      glog("part%i y: %i, x: %i", i, cury, curx);
      cury = tmpy;
      curx = tmpx;
      wsyncup(game->snake.parts[i]);
      wrefresh(game->snake.parts[i]);
    }


    glog("going to grow?");
    if(game->snake.grow > 0) {
      glog("yes");
      grow_snake(&game->snake, cury, curx);
      game->snake.grow--;
    } else {
      glog("no");
      WINDOW *cur = newwin(1, 1, cury, curx);
      wprintw(cur, "%c", ' ');
      wrefresh(cur);
      delwin(cur);
    }
  }
  glog("back");
  return success;
}