#include "events.h"
#include "snake.h"
#include "fruits.h"

// check if the snake collides with a fruit
//뱀이 과일과 충돌하는지 확인
int check_fruit_collision(GAME* game, int cury, int curx) {
  // calculate the range, limit the range to a maximum of 5
  //사정거리 계산, 5로 최대값 한정
  int range = (game->snake.eat_range >= 5 ? 5 : game->snake.eat_range) - 1;
  // calculate the start position
  //시작점 계산
  int startx = curx - range;
  int starty = cury - range;
  // calculate the end position
  //끝 지점 계산
  int endx = startx + range * 2 + 1;
  int endy = starty + range * 2 + 1;
  int x,y;

  int on = 0; // check if there is food in the calculated range, 사정거리 안 먹이 있는지 검사
  // iterate througth every field inside the range
  //사정거리 안 모든 칸 검사
  for(x = startx; x  < endx; x++) {
    for(y = starty; y  < endy; y++) {
      // is a fruit on the current field?
      //칸에 과일 유무?
      if(fruit_is_on(&game->fruits, y, x) != NULL) {
        // exclude the field in the middle
        //중앙 칸은 제외
        if(!(curx == x && cury == y)) {
          // execute the handler for this field (to eat the fruit)
          //과일 핸들러 실행
          check_fruit_collision_handler(game, y, x);
        }
        on = 1; // found one!, 과일 찾았을때
      }
    }
  }
  return on;
}
 
// calls the effect of the fruit
//과일 효과 실행
int check_fruit_collision_handler(GAME* game, int cury, int curx) {
  // the the fruit by the given position
  //특ㅈㅇ 칸에 있는 과일
  FRUIT *fruit = fruit_is_on(&game->fruits, cury, curx);
  // is one on this position?
  //특정 칸에 과일 유무 검사
  if(fruit != NULL) {
    // execute the effect of the fruit
    //과일 효과 실행
    fruit->effect(game);

    WINDOW *win = newwin(1, 1, cury, curx);
    wprintw(win, " ");
    wrefresh(win);
    delwin(win);
    // remove the fruit from the game
    //과일 게임에서 없애기
    kill_fruit_by_ptr(&game->fruits, fruit);
  }
  return 1;
}

// check if the snake collides with a border which looks like this
//밑에처럼 생긴 테두리와 충돌 검사
// #################
// #               #
// #               #
// #               #
// #               #
// #               #
// #################
int check_border_collision(GAME* game, int cury, int curx) {
  // check if the given position has a distance of 1 from the screen border
  //위치가 벽에서 거리가 1인지 확인
  return cury <= 0 || curx <= 0 || cury >= game->rows - 1 || curx >= game->columns - 1; 
}

// check if the snake collides with the border which looks like this
//밑에처럼 생긴 테두리와 충돌 검사
// #######   #######
// #               #
// #               #
//                  
// #               #
// #               #
// #######   #######
int check_extended_border_collision(GAME* game, int cury, int curx) {
  int range, rangex1, rangex2, rangey1, rangey2;

  // # calculate the positions where holes in the wall start and end
  //벽에 구멍의 시작점과 끝점 계산
  //  do that for the top and the bottom (x-axis)
  //밑과 위 검사
  range = game->columns / 2;
  rangex1 = range * 0.9;
  rangex2 = range * 1.1;
  // do that for the left and the right side (y-axis)
  // 오른쪽과 왼쪽 검사
  range = game->rows / 2;
  rangey1 = range * 0.9;
  rangey2 = range * 1.1;
  
  // check if the given position is at the border but not within the holes
  //위치가 테두리에 있지만 구멍 속이 있지 않을 때 검사
  return check_border_collision(game, cury, curx)
    && !((curx >= rangex1 && curx <= rangex2) || (cury >= rangey1 && cury <= rangey2));
}

// ends the game if a collison is present
//충돌 일어나면 게임 종료
int check_border_collision_handler(GAME* game, int cury, int curx) {
  return 0; // code for end of the game, 게임 종료 코드
}



// check if the snake is colliding on it's self
//뱀이 자기 자신과 충돌 검사
// we only need to check if the snake head is colliding
//머리만 검사
int check_self_collision(GAME* game, int cury, int curx) {
  WINDOW* on;
  // check if the position of the snake head is matching with the position of a snake part
  //뱀 머리가 뱀 몸과 같은 위치에 있는지 검사
  // exept for the last part (because it will move
  //마지막 부분 빼고
  return !((on = snake_part_is_on(&game->snake, cury, curx)) == NULL || on == game->snake.parts[game->snake.length - 1]);
}

// ends the game if a collision is present
//충돌 일어나면 게임 종료
int check_self_collision_handler(GAME* game, int cury, int curx) {
  return 0; // code for end of the game, 게임 종료 코드
}

