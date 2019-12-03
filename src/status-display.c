#include "status-display.h"
#include "highscore.h"

// creates a little display on the top right of the screen informing you
// how long your snake is, how much points you have, how long you played
// and what your current score is
//
//화면 위-오른쪽에 현재 상태 나타내는 숫자 출력
//
// the whole thing should look like this:
//
// #####################015L-00123P-0012s-1206SCR
//                                              #
//                                              #
//                                              #
//                                              #
//                                              #
void status_display(GAME *game) {
  const int width = 25; // the width in chars of the display, 풀력되는 숫자 넓이

  // create a new window in the top right corner with a height of 1
  // and a width of 'width'
  //위-오른쪽에 작은 윈도우 생성
  WINDOW *win = newwin(1, width, 0, game->columns - width);

  // print out the current length, the points, the time and the current highscore
  //현재 뱀 길이, 점수, 시간, 하이스코어 출력
  wprintw(win,"%03iL-%05iP-%04is-%04iSCR",
      game->snake->length,
      game->highscore,
      time(NULL) - game->started - game->paused,
      calculate_score(game->highscore, time(NULL) - game->started - game->paused));

  // display it
  //화면에 보여주기
  wrefresh(win);

  // free the memory needed for the window
  //윈도우 대한 메모리 프리
  delwin(win);
}
