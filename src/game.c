#include "game.h"
#include "events.h"
#include "fruits.h"
#include "snake.h"
#include "main.h"
#include "dialog.h"
#include "highscore.h"
#include "status-display.h"
#include "time-helpers.h"

// end the game, free the resources
//게임 끝내기, free 메모리
void kill_game(GAME *game) {
  kill_fruits(&game->fruits);
  kill_snake(&game->snake);
}

//화면 크기 변화 감지 
int screen_changed(GAME *game, int current_rows, int current_columns){
  if(current_rows != game->rows || current_columns != game->columns)
    return 1;
  else
    return 0;
}

int check_invalid_dir(GAME *game, direction dir){
  if(dir == DIR_UP && game->snake.dir != DIR_DOWN && dir != game->snake.dir)
    return 1;
  else if(dir == DIR_DOWN && game->snake.dir != DIR_UP && dir != game->snake.dir)
    return 1;
  else if(dir == DIR_RIGHT && game->snake.dir != DIR_LEFT && dir != game->snake.dir)
    return 1;
  else if(dir == DIR_LEFT && game->snake.dir != DIR_RIGHT && dir != game->snake.dir)
    return 1;
  else
    return 0;
    
}

void direction_change(GAME *game, int ch){
    
  if(  (ch == KEY_UP || ch == 'w') && (check_invalid_dir(game, DIR_UP) == 1)  ) {
    game->snake.dir = DIR_UP;
    //interval = default_interval * 1.3; //가로 세로 다른 속력
    game->interval = game->base_interval * 1.5;

  }else if((ch == KEY_LEFT || ch == 'a') && (check_invalid_dir(game, DIR_LEFT) == 1) ) {
    game->snake.dir = DIR_LEFT; // 가로 세로 다른 속력
    //interval = default_interval * 0.9;
    game->interval = game->base_interval * 0.9; 

  }else if((ch == KEY_RIGHT || ch == 'd') && (check_invalid_dir(game, DIR_RIGHT) == 1) ) {
    game->snake.dir = DIR_RIGHT; //가로 세로 다른 속력
    //interval = default_interval * 0.9;
    game->interval = game->base_interval * 0.9;

  }else if((ch == KEY_DOWN || ch == 's') && (check_invalid_dir(game, DIR_DOWN) == 1) ) {
    game->snake.dir = DIR_DOWN;
    //interval = default_interval * 1.3; //가로 세로 다른 속력
    game->interval = game->base_interval * 1.5;

  }

  
  return;
}

void enemy_direction_change(GAME *game){
  time_t t;
  
  srand((unsigned) time(&t));

  if( rand() % 4 == 0 && (check_invalid_dir(game, DIR_UP) == 1)){// (ch == KEY_UP || ch == 'w') && (check_invalid_dir(game, DIR_UP) == 1)  ) {
    game->snake.dir = DIR_UP;
    //interval = default_interval * 1.3; //가로 세로 다른 속력
    game->interval = game->base_interval * 1.5;

  }else if( rand() % 4 == 1 && (check_invalid_dir(game, DIR_LEFT) == 1)){//(ch == KEY_LEFT || ch == 'a') && (check_invalid_dir(game, DIR_LEFT) == 1) ) {
    game->snake.dir = DIR_LEFT; // 가로 세로 다른 속력
    //interval = default_interval * 0.9;
    game->interval = game->base_interval * 0.9; 

  }else if( rand() % 4 == 2 && (check_invalid_dir(game, DIR_RIGHT) == 1)){//(ch == KEY_RIGHT || ch == 'd') && (check_invalid_dir(game, DIR_RIGHT) == 1) ) {
    game->snake.dir = DIR_RIGHT; //가로 세로 다른 속력
    //interval = default_interval * 0.9;
    game->interval = game->base_interval * 0.9;

  }else if( rand() % 4 == 3 && (check_invalid_dir(game, DIR_DOWN) == 1)){//(ch == KEY_DOWN || ch == 's') && (check_invalid_dir(game, DIR_DOWN) == 1) ) {
    game->snake.dir = DIR_DOWN;
    //interval = default_interval * 1.3; //가로 세로 다른 속력
    game->interval = game->base_interval * 1.5;

  }

  
  return;
}

void run() {
  int ch = 0, ich, i, current_columns, current_rows, success = 1;

  int temp;
  // some variables for the timer (inclusive the interval)
  //타이머 위한 변수
  struct timespec last_time              = {};
  struct timespec current_time           = {};
  //long long default_interval                = 100000000;
  //long long base_interval             = default_interval; // 100 ms

  //long long interval                     = default_interval;
  
  long long res;
  char playername[HIGHSCORE_NAME_LENGTH] = {};

  int range_counter = 0;

  // create the game struct
  //GAME 생성
  GAME game = {};
  
  // set the eat range to 1
  //뱀 먹이 사정거리 1로 설정
  game.snake.eat_range = 1;
  game.base_interval = DEFAULT_INTERVAL;
  game.interval = DEFAULT_INTERVAL;

  GAME enemy = {};
  enemy.snake.eat_range = 5;
  enemy.base_interval = DEFAULT_INTERVAL;
  enemy.interval = DEFAULT_INTERVAL;
  
  // helper variable to keep track of how long we've paused
  //일시정지 시간 저장하는 변수
  time_t pause_start;

  // get the dimensions of the terminal and store them in the GAME struct
  //윈도우 크기 구해 GAME에 저장
  getmaxyx(stdscr, game.rows, game.columns);
  getmaxyx(stdscr, enemy.rows, enemy.columns);
  
  // clear the whole screen
  //화면 모두 지우기
  clear();

  // draw the walls
  //테두리 그리기
  draw_border(&game);

  // show the newly created walls
  //테두리 보여주기
  refresh();

  // place the snake in the middle of the game field
  //뱀을 중심에 생성
  grow_snake(&game.snake, game.rows / 2, game.columns / 2);
  game.snake.dir = DIR_LEFT;

  grow_snake(&enemy.snake, enemy.rows / 3, enemy.columns / 3);
  game.snake.dir = DIR_LEFT;


  
  // create some fruits on the screen
  //과일 생성
  // NOM, NOM, NOM
  for(i = 0; i < 50; i++) {
    grow_fruit(&game);
  }
  
  // get the time when the game started
  //게임 시작 시간 저장
  time(&game.started);
  // get the current time
  //현재 시간 저장
  current_utc_time(&last_time);

  // start the event loop
  //이벤트 루프 시작
  while((ich = getch()) && success) {

    // key typed?
    //스위치문으로 대체
    /*
    if(ich == ERR) {
    } else if(ich == '0') {
      // reset the speed
      interval = default_interval;
    } else if(ich == '8') {
      // speed up
      interval *= 1.1;
    } else if(ich == '9') {
      // slow down
      interval *= 0.9;
    } else {
      // use this key as a direction
      ch = ich;
    }
    */

    switch(ich){
    case '0':
      //뱀 속도 기본값으로 지정
      game.base_interval = DEFAULT_INTERVAL;
      break;

    case '8':
      //뱀 속도 기본값 올리기
      game.base_interval *= 1.1;
      break;

    case '9':
      //뱀 속도 기본값 내리기
      game.base_interval *= 0.9;
      break;

    default:
      //다음 코드로 전달
      if(ich != ERR)
        ch = ich;
      break;

    }
    // check if we have an overrun
    //기간이 초과했는지 확인
    current_utc_time(&current_time);

    // calculate the dirrence between the last snake move and the current time
    //마지막 움직인 시간과 현재 시간의 차이 계산
    res = timeval_diff(&last_time, &current_time);

    // is the interval over?
    //뱀 이동 주기 끝?
    if(res > game.interval) {
      // has an effect on the eat_range ?
      //먹이 사정거리에 영향?
      if(game.snake.eat_range > 1) {
        // every 200th field, decrease the range
        //200번째마다 사정거리 줄이기
        range_counter = (range_counter + 1) % 150;
        // it turns to 0 after the 200th field
        //200번 후에는 사정거리 0
        if(range_counter == 0) {
          game.snake.eat_range--; // so, decrease it!  사정거리 줄이기
        }
      }

      /*
      // new direction?
      //새로운 방향?
      if((ch == KEY_UP || ch == 'w') && game.snake.dir != DIR_DOWN && game.snake.dir != DIR_UP) {
        game.snake.dir = DIR_UP;
        interval = default_interval * 1.3; //가로 세로 다른 속력
      } else if((ch == KEY_LEFT || ch == 'a') && game.snake.dir != DIR_RIGHT && game.snake.dir != DIR_LEFT) {
        game.snake.dir = DIR_LEFT; // 가로 세로 다른 속력
        interval = default_interval * 0.9;
      } else if((ch == KEY_RIGHT || ch == 'd') && game.snake.dir != DIR_LEFT && game.snake.dir != DIR_RIGHT) {
        game.snake.dir = DIR_RIGHT; //가로 세로 다른 속력
        interval = default_interval * 0.9;
      } else if((ch == KEY_DOWN || ch == 's') && game.snake.dir != DIR_UP && game.snake.dir != DIR_DOWN) {
        game.snake.dir = DIR_DOWN;
        interval = default_interval * 1.3; //가로 세로 다른 속력
      }
      */

      direction_change(&game, ch);
      // move the snake
      //뱀 움직이기
      success = move_snake(&game);

      enemy_direction_change(&enemy);
      temp = move_snake(&enemy);
      /*
      if(move_snake(&enemy) == 0)
	kill_snake(&enemy.snake);
      */
      
      // refresh the screen
      //화면 다시 리프레쉬
      refresh();

      // display the status bar (top-right)
      //위-오른쪽 스태터스바 표출
      status_display(&game);

      // update the time when we last moved the snake
      //마지막으로 뱀 움직인 시간 업데이트
      last_time = current_time;
    }
    
    getmaxyx(stdscr, current_rows, current_columns);
    // 'p' pressed || size of the terminal changed
    //p가 눌리면 || 윈도우 크기가 바뀌면
    
    //if(ich == 'p' || (current_rows != game.rows || current_columns != game.columns)) {
    

    if(ich == 'p' || screen_changed(&game, current_rows, current_columns) == 1){
      // use the terminal new size
      //새로운 윈도우 크기 사용
      game.rows = current_rows;
      game.columns = current_columns;

      // get the time
      //현재 시간 얻기
      time(&pause_start);

      // show the pause dialog
      //일시정시 화면 표출
      switch(pause_dialog()) {
        case 2:
          // leave the game if '2' is pressed
          //2가 눌리면 나가기
          success = 0;
          break;
        default:
          // redraw the screen on resume
          //화면 전체 리프레쉬
          game.paused += time(NULL) - pause_start;
          redraw_game(&game);
          break;
      }
    }
  }

  // get the time when the game has ended
  //게임 끋난 시간 얻기
  time(&game.ended);

  // display the highscore dialog & let the player enter his name
  //하이스코어 메뉴 표시, 이름 입력
  display_highscore(&game, playername, HIGHSCORE_NAME_LENGTH);

  // has a name been entered? if not don't create a highscore entry
  //이름이 안 입력되면 하이스코어 저장하지 않기
  if(playername[0]) {
    add_highscore(playername, game.highscore, game.ended - game.started - game.paused);
  }

  // free all the resources reserved in the game struct
  //GAME 메모리 해제
  kill_game(&game);
}

void draw_border(GAME *game) {
  int x, y;
  // create a border
  //테두리 그리기
  attron(A_BOLD | COLOR_PAIR(6));
  for(x = 0; x < game->columns; x++) {
    for(y = 0; y < game->rows; y++) {
      if(check_extended_border_collision(game, y, x)) {
        mvprintw(y, x, "#");
      }
    }
  }
}

// redraw the whole screen
//전체 화면 리프레쉬
void redraw_game(GAME *game) {
  // redraw the main window (containg the border and stuff)
  //메인 화면 리프레쉬
  clear();
  draw_border(game);
  redrawwin(stdscr);
  refresh();
  
  // redraw the fruits
  //과일 리프레쉬
  redraw_fruits(&game->fruits);

  // redraw the snake
  //뱀 리프레쉬
  redraw_snake(&game->snake);
}


