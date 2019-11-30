#include "effects.h"
#include "fruits.h"

/* Here you can find some diffrent effect diffrent fruits may have if you eat them */
/*과일 효과*/
// ---- normal/일반 ----
// char: 'x', 글자 x
// grow by 1, 1칸 증가
// increase highscore by 1, 하이스코어 1 증가
// grow 1 new fruit, 과일 1개 생성
void normal_effect(GAME *game) {
  grow_fruit(game);
  game->snake.grow++;
  game->highscore++;
}

// ---- double grow/2배 증가 ----
// char: '@', 글자 @
// grow by 1, 1칸 증가
// increase highscore by 1, 하이스코어 1 증가
// grow 2 new fruit, 과일 2개 생성
void double_grow(GAME *game) {
  normal_effect(game);
  grow_fruit(game);
}

// ---- mega food/메가 과일 ----
// char: '%', 글자 %
// grow by 5, 5칸 증가
// increase highscore by 10, 하이스코어 10 증가
// grow 1 new fruit, 과일 1개 생성
void mega_food(GAME *game) {
  normal_effect(game);
  game->snake.grow += 4;
  game->highscore += 9;
}

// ---- eat boost/먹이 파워업 ----
// char '&', 글자 &
// grow by 1, 1칸 증가
// increase highscore by 1, 하이스코어 1 증가
// grow 1 new fruit, 과일 1개 생성
// SPECIAL: the range the snake eats food increases by 1 temporarily, 먹이 사정거리 1로 증가
void eat_boost(GAME *game) {
  normal_effect(game);
  game->snake.eat_range++;
}
