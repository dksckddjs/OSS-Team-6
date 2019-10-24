#include "snake.h"
#include "events.h"

// check if a part of the snake is on the given spot
//뱀의 일부가 특정 위치에 있는지 확인
// returns a WINDOW * if one is found, NULL if not
//있으면 WINDOW * 리턴, 없으면 NULL
WINDOW *snake_part_is_on(SNAKE *snake, int posy, int posx) {
  int i;
  int cury, curx; // position of the current snake part we are iterating, 뱀의 위치

  // iterate each part
  //각 부분 들어가기
  for(i = 0; i < snake->length; i++) {
    // get the position of the current part
    //현재 부위의 좌표값 구하기
    getbegyx(snake->parts[i], cury, curx);

    // compare the position
    //좌표값 비교
    if(cury == posy && curx == posx) {
      // return the current part
      //현재 부분 리턴
      return snake->parts[i];
    }
  }

  return NULL;
}

// increase the length of the name by one
//뱀 길이 1 증가
void grow_snake(SNAKE *snake, int posy, int posx) {
  // a window containing a single char of a snake
  //뱀 1칸
  WINDOW *win;

  // increase the length of the snake
  //뱀 길이 늘리기
  snake->length++;
  // allocate memory for the new part
  //새로운 부분 메모리 할당
  if(snake->length == 0) {
    // initialize the array with malloc if it is the first part
    //처음 칸이면 malloc
    snake->parts = malloc(sizeof(WINDOW*) * snake->length);
  } else {
    // increase the amount of allocated memory
    //처음 칸이 아니면 realloc
    snake->parts = realloc(snake->parts, sizeof(WINDOW*) * snake->length);
  }
  
  // create a new window
  //새로운 윈도우 생성
  snake->parts[snake->length - 1] = win = newwin(1, 1, posy, posx);
  // print the character on the window
  //뱀 부분 화면에 출력
  wattron(win, COLOR_PAIR(2));
  wprintw(win, "%c", 'O');
  // display the part
  //뱀 부분 보여주기
  wrefresh(win);

  return;
}

// free the allocated memory for the snake
//뱀에 할당된 메모리 프리
void kill_snake(SNAKE *snake) {
  int i;
  // delete each created window of the snake
  //뱀 각 부분별로 윈도우 없애기
  for(i = 0; i < snake->length; i++) {
    delwin(snake->parts[i]);
  }
  // free the resources allocated for the window pointers
  //윈도우 포인터 대한 메모리 프리
  free(snake->parts);

  return;
}

// move the snake
//뱀 움직이기
int move_snake(GAME *game) {
  int success = 1, i;

  // some variables containing positions of certain things
  //특정 윈도우의 좌표
  int curx, cury, tmpy, tmpx;

  // callbacks to check collisions
  //충돌 검사 테이블
  int (*collision_checks[EVENTS])(GAME*, int, int) = {
    check_self_collision,
    check_extended_border_collision,
    check_fruit_collision
  };

  // callbacks which will be called automaticly if a collision is detected
  // the callback at position 1 in the array belongs to the check-function
  // at position 1 from the array above
  //충돌 일어나면 자동으로 핸들러 실행
  int (*collision_handlers[EVENTS])(GAME*, int, int) = {
    check_self_collision_handler,
    check_border_collision_handler,
    check_fruit_collision_handler
  };


  // difference on x-axis according to the direction
  //뱀이 움직이는 방향과 x축 대한 차이
  int xdiff = game->snake.dir == DIR_LEFT ? -1 : (game->snake.dir == DIR_RIGHT ? 1 : 0);
  // difference on y-axis according to the direction
  //뱀이 움직이는 방향과 y축 대한 차이
  int ydiff = game->snake.dir == DIR_UP ? -1 : (game->snake.dir == DIR_DOWN ? 1 : 0);

  // the position of the snake head
  //뱀 머리 위치
  getbegyx(game->snake.parts[0], cury, curx);

  // make a copy
  //복사하기
  tmpy = cury;
  tmpx = curx;

  // calculate the new position and prevent from exceeding the size of the screen
  //새로운 위치 계산하고 테두리 안벗어나게 검사
  cury = (cury + ydiff) % game->rows;
  curx = (curx + xdiff) % game->columns;
  // the values have to be positive
  //값이 양수여야 함
  cury = cury < 0 ? game->rows + cury : cury;
  curx = curx < 0 ? game->columns + curx : curx;

  // check for collisons and execute the handlers if a collision occured
  //충돌 확인하고 핸들러 실행
  for(i = 0; i < EVENTS && success; i++) {
    // collision?
    //충돌?
    if((collision_checks[i](game, cury, curx))) {
      // should we end the game because of the collision?
      //뱀 죽음?
      if(!collision_handlers[i](game, cury, curx)) {
        success = 0;
      }
    }
  }

  // no collisions ?
  //충돌 없음?
  if(success) {
    // set he direction of the head
    //뱀 머리의 방향 설정
    mvwprintw(game->snake.parts[0], 0, 0, "%c", game->snake.dir);
    // move the window
    //윈도우 움직이기
    mvwin(game->snake.parts[0], cury, curx);

    // copy values back
    //다시 복사하기
    cury = tmpy;
    curx = tmpx;

    // iterate through each part of the snake
    //뱀이 각 부위별로 들어가기
    for(i = 1; i < game->snake.length; i++) {
      // get the position of the current part
      //부위의 위치 구하기
      getbegyx(game->snake.parts[i], tmpy, tmpx);

      // move the part to the position of the previous one
      //전의 위치로 움직이기
      mvwin(game->snake.parts[i], cury, curx);

      // make a copy
      //복사
      cury = tmpy;
      curx = tmpx;
    }

    // grow?
    //길이 증가 필요?
    if(game->snake.grow > 0) {
      // grow the snake
      //길이 증가하기
      grow_snake(&game->snake, cury, curx);

      // decrease the grow counter (number of times the snake will grow in the future)
      //뱀이 길어질수 있는 수 줄이기
      game->snake.grow--;
    } else {
      // is the snake head on the same position as the last part of the snake was before?
      //혀내 뱀 머리의 위치가 전에 뱀 몸이 있엇던 부위인가?
      getbegyx(game->snake.parts[0], tmpy, tmpx);
      if(!(tmpy == cury && tmpx == curx)) {
        // if no print a space at this position
        //아니면 스페이스 출력
        WINDOW *cur = newwin(1, 1, cury, curx);
        wprintw(cur, "%c", ' ');
        wrefresh(cur);
        delwin(cur);
      }
    }

    // redraw the snake on the screen
    //뱀 화면에 다시 그리기
    redraw_snake(&game->snake);
  }
  return success;
}

// redraw the whole snake
//뱀 전체 다시 그리기
void redraw_snake(SNAKE *snake) {
  int i;
  // iterate through each part of the snake
  //뱀 각 부위별로 들어가기
  for(i = 0; i < snake->length; i++) {
    // redraw the current part
    //부위 다시 그리기
    redrawwin(snake->parts[i]);
    wrefresh(snake->parts[i]);
  }
}
