#include "fruits.h"
#include "effects.h"
#include "events.h"
#include "snake.h"

// free the resources needed by the fruits
//과일 필요한 메모리 프리
void kill_fruits(FRUITS* fruits) {
  int i;

  // iterate throgh each fruit and delete the window
  //과일 1칸씩 윈도우 없애기
  for(i = 0; i < fruits->length; i++) {
    delwin(fruits->fruits[i].win);
  }
  // reset the length
  //길이 초기화
  fruits->length = 0;
  // free the allocated memory
  //메모리 프리
  free(fruits->fruits);
}

// remove a single fruit from the game by it's position
//특정 위치에서 과일 없애기
void kill_fruit(FRUITS *fruits, int posy, int posx) {
  FRUIT *fruit;
  // check if a fruit is on the given position
  //과일이 특정 우이치에 있는지 검사
  if((fruit = fruit_is_on(fruits, posy, posx)) != NULL) {
    // remove the fruit
    //과일 없애기
    kill_fruit_by_ptr(fruits, fruit);
  }
}
  
// remove a single fruit from the game
//과일 1개 없애기
void kill_fruit_by_ptr(FRUITS *fruits, FRUIT *fruit) {
  int i;
  // check if FRUIT is in the array of FRUITS
  //과일이 과일 배열에 있는지 확인
  for(i = 0; i < fruits->length; i++) {
    if(&fruits->fruits[i] == fruit) {
      // remove the fruit
      //과일 없애기
      kill_fruit_at_pos(fruits, i);
      break;
    }
  }
}

// remove a single fruit from the fruits array
//과일 배열에서 과일 1개 없애기
void kill_fruit_at_pos(FRUITS *fruits, int i) {
  // move the frutis afterwards to front
  //남은 과일 앞으로 이동
  for(i++; i < fruits->length; i++) {
    fruits->fruits[i-1] = fruits->fruits[i];
  }

  // realloc lessmemory
  //메모리 리알록
  fruits->fruits = realloc(fruits->fruits, --fruits->length * sizeof(FRUIT));
}


// returns a FRUIT if one is found on the given point, else NULL
//특정 위치에 과일 있으면 과일 개체 리턴, 없으면 NULL
FRUIT *fruit_is_on(FRUITS *fruits, int posy, int posx) {
  int i;
  int y,x;

  // iterate each fruit
  //과일 배열 각각 검사
  for(i = 0; i < fruits->length; i++) {
    // read the position of the current fruit
    //현재 과일 위치 읽기
    getbegyx(fruits->fruits[i].win, y, x);
    // check if the positions are matching
    //과일 위치가 배열 안의 과일 위치가 맞는지 확인
    if(posy == y && posx == x) {
      // return a pointer to the current fruit
      //현제 과일 대한 포인터 리턴
      return &fruits->fruits[i];
    }
  }
  return NULL;
}

// create a new fruit in the game
//새로운 과일 생성
void grow_fruit(GAME* game) {
  int randy,randx;
  // generate a new random position until a empty spot is found
  //빈 위치 찾을때까지 랜덤으로 위치 생성
  do {
    randy = rand() % game->rows;
    randx = rand() % game->columns;
    // is nothing else there in the generated position?
    //빈 위치이지 검사
  } while (snake_part_is_on(&game->snake, randy, randx) != NULL || fruit_is_on(&game->fruits, randy, randx) != NULL || check_extended_border_collision(game, randy, randx));

  // increase the length
  //길이 늘리기
  game->fruits.length++;
  // allocate memory for the new fruit
  //새로운 과일 대한 메모리 할당
  if(game->fruits.length == 0) {
    // initialize the array with malloc if it is the first fruit in the array
    //처음 과일이면 malloc
    game->fruits.fruits = malloc(sizeof(FRUIT) * game->fruits.length);
  } else {
    // allocate more memory
    //realloc
    game->fruits.fruits = realloc(game->fruits.fruits, sizeof(FRUIT) * game->fruits.length);
  }
  
  // get a filled struct (containing the displayed char, the effect & co.) of the new fruit
  //과일 특성 담긴 개체 얻기
  get_fruit(&game->fruits.fruits[game->fruits.length - 1], randy, randx);
}


// fill in data into a fruit struct
//과일 개체 생성
void get_fruit(FRUIT *fruit, int posy, int posx) {
  // how the diffrent fruits are displayed
  //과일 모양
  static char chars[EFFECTS] = {'x', '@', '%', '&'};
  static int colors[EFFECTS] = { 4 ,  6 ,  3 ,  5 }; // see color definitions in the end of main.c, 메인에 있는 색갈
  // the different effects of the fruits
  //과일 효과
  static void (*effects[EFFECTS])(GAME *) = {
    normal_effect, // see effects.c, effects.c 보기
    double_grow,
    mega_food,
    eat_boost
  };
  // the chance a certain fruit appears, 특정 과일 생성되는 확률
  static int chance[EFFECTS] = {100, 25, 5, 2};
  // the sum of all the chances, 확률 대한 합
  static int max_chance = 132;

  int i = 0;
  int sum = 0;

  // generate a random number
  //난수 생성
  int random = rand() % max_chance;
  // get the number of the fruit out of the random number
  //과일 숫자를 난수로부터 생성
  // the index will increase every time the random value is greater
  // than the sum of all elements before the index and the element with the index
  // there is also a bounds check for the array
  //난수가 인덱스 전의 값들보다 크면 인덱스도 커짐
  //배열 대한 오버플로우도 확인
  while(random >= (sum += chance[i]) && i + 1 < EFFECTS) {
    i++; // next index, 다음 인덱스
  };

  // create a new window, 새로운 윈도우 생성
  fruit->win = newwin(1, 1, posy, posx);
  // assign the effect, 효과 부여
  fruit->effect = effects[i];
  // set the color and brightness, 밝기와 색갈 부여
  wattron(fruit->win, (A_BOLD | COLOR_PAIR(colors[i]) ) );
  // print the char on the window, 윈도우에 캐릭터 출력
  wprintw(fruit->win, "%c", chars[i]);
  // show the fruit, 가일 보여주기
  wrefresh(fruit->win);
}

// redraw all the fruits on the screen
//화면 모든 과일 리프레쉬
void redraw_fruits(FRUITS *fruits) {
  int i;

  // iterate through each fruit.
  //과일 배열 각각 들어가기
  for(i = 0; i < fruits->length; i++) {
    // redraw it!
    //다시 그리기
    redrawwin(fruits->fruits[i].win);
    wrefresh(fruits->fruits[i].win);
  }
}
