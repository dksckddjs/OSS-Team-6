#include "fruits.h"
#include "effects.h"
#include "events.h"
#include "snake.h"

// free the resources needed by the fruits
//과일 필요한 메모리 프리
void kill_fruits(FRUITLIST* fruits) {
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
 
// remove a single fruit from the game
//과일 1개 없애기
void kill_fruit_by_ptr(FRUITLIST *fruits, FRUITNODE *fruit) {
  int i;
  // check if FRUIT is in the array of FRUITS
  //과일이 과일 배열에 있는지 확인
  for(i = 0; i < fruits->length; i++) {
    if(&fruits->fruits[i] == fruit) {
      // remove the fruit
      //과일 없애기
      kill_fruit_at_idx(fruits, i);
      break;
    }
  }
}

// remove a single fruit from the fruits array
//과일 배열에서 과일 1개 없애기
void kill_fruit_at_idx(FRUITLIST *fruits, int i) {
  // move the frutis afterwards to front
  //남은 과일 앞으로 이동
  for(i++; i < fruits->length; i++) {
    fruits->fruits[i-1] = fruits->fruits[i];
  }

  // realloc lessmemory
  //메모리 재할당
  // 이 함수를 사용할 때 기존에 지우려던 fruit을 메모리 할당 해제함으로써 fruitList에서 나가게 하는 것이아닌,
  // 지우려던 fruit을 제외하고 다른것들이 단일 링크드 리스트에서 이어지게 한 후 메모리 재할당을 통해 없애는 방식이다.
  fruits->fruits = realloc(fruits->fruits, --fruits->length * sizeof(FRUITNODE));
}


// returns a FRUIT if one is found on the given point, else NULL
//특정 위치에 과일 있으면 과일 개체 리턴, 없으면 NULL
FRUITNODE *fruit_is_on(FRUITLIST *fruits, int posy, int posx) {
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
      //현재 과일 대한 포인터 리턴
      return &fruits->fruits[i];
    }
  }
  return NULL;
}

int isNullSpace(GAME *game,int randy,int randx) {
  if(snake_part_is_on(&game->snake,randy,randx) != NULL || fruit_is_on(&game->fruits, randy, randx) != NULL || check_extended_border_collision(game, randy, randx)) {
    return 0;
  }
  return 1;
}

// create a new fruit in the game
//새로운 과일 생성
void grow_fruit(GAME* game) {
  int randy,randx;
  /*
  do {
    randy = rand() % game->rows;
    randx = rand() % game->columns;
    // is nothing else there in the generated position?
    //빈 위치이지 검사
  } while (snake_part_is_on(&game->snake, randy, randx) != NULL || fruit_is_on(&game->fruits, randy, randx) != NULL || check_extended_border_collision(game, randy, randx));
  */

  randy = rand() % game->rows;
  randx = rand() % game->columns;
  // generate a new random position until a empty spot is found
  //빈 위치 찾을때까지 랜덤으로 위치 생성
  if(!isNullSpace(game, randy, randx)) {
    grow_fruit(game);
    return;
  }// 가독성은 아래꺼 속도는 위에꺼

  // increase the length
  //길이 늘리기
  game->fruits.length++;
  // allocate memory for the new fruit
  //새로운 과일 대한 메모리 할당
  if(game->fruits.length == 0) {
    // initialize the array with malloc if it is the first fruit in the array
    //처음 과일이면 malloc
    game->fruits.fruits = malloc(sizeof(FRUITNODE) * game->fruits.length);
  } else {
    // allocate more memory
    //realloc
    game->fruits.fruits = realloc(game->fruits.fruits, sizeof(FRUITNODE) * game->fruits.length);
  }
  
  // get a filled struct (containing the displayed char, the effect & co.) of the new fruit
  // 과일 특성 담긴 개체 얻기
  get_fruit(&game->fruits.fruits[game->fruits.length - 1], randy, randx);
}


// fill in data into a fruit struct
//과일 개체 생성
void get_fruit(FRUITNODE *fruit, int posy, int posx) {
  // how the diffrent fruits are displayed
  //과일 모양
  static char chars[EFFECTS] = {'x', '@', '%', '&', 'r'};
  static int colors[EFFECTS] = { 4 ,  6 ,  3 ,  5 , 7}; // see color definitions in the end of main.c, 메인에 있는 색깔
  // the different effects of the fruits
  //과일 효과
  static void (*effects[EFFECTS])(GAME *) = {
    normal_effect, // see effects.c, effects.c 보기
    double_grow,
    mega_food,
    eat_boost,
    reduce_speed
  };
  // the chance a certain fruit appears, 특정 과일 생성되는 확률
  static int chance[EFFECTS] = {100, 25, 5, 2, 5};
  // the sum of all the chances, 확률 대한 합
  static int max_chance = 137;

  int i = 0;
  int sum = 0;

  // generate a random number
  //난수 생성
  int random = rand() % max_chance;
  // get the number of the fruit out of the random number
  // the index will increase every time the random value is greater
  // than the sum of all elements before the index and the element with the index
  // there is also a bounds check for the array
  //난수가 인덱스 전의 값들보다 크면 인덱스도 커짐
  //배열 대한 오버플로우 확인
  while(random >= (sum += chance[i]) && i + 1 < EFFECTS) {
    i++; // next index, 다음 인덱스
  };

  // create a new window, 새로운 윈도우 생성
  fruit->win = newwin(1, 1, posy, posx);
  // assign the effect, 효과 부여
  fruit->effect = effects[i];
  // set the color and brightness, 밝기와 색깔 부여
  wattron(fruit->win, (A_BOLD | COLOR_PAIR(colors[i]) ) );
  // print the char on the window, 윈도우에 캐릭터 출력
  wprintw(fruit->win, "%c", chars[i]);
  // show the fruit, 과일 보여주기
  wrefresh(fruit->win);
}

// redraw all the fruits on the screen
//화면 모든 과일 리프레쉬
void redraw_fruits(FRUITLIST *fruits) {
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
