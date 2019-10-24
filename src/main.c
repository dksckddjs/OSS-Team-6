#include "main.h"
#include "dialog.h"
#include "highscore.h"
#include "game.h"

int main() {

  // for some better random numbers (and not always the same)
  //더 효율적인 난수 생성
  srand(time(NULL));
  
  init_curses();

  main_menu();

  end_curses();

  // free the allocated memory for the highscore
  //하이스코어에 할당된 메모리 프리
  read_highscore(NULL);
  // close the logfile
  //로그 파일 닫기
  glog(NULL);
  return EXIT_SUCCESS;
}

void main_menu() {
  int selected_menu_entry;
  do {
    selected_menu_entry = display_menu();

    /* 스위치문으로 기능 바꿈
    if(selected_menu_entry == 1) {
      // run the game
      run();
    } else if(selected_menu_entry == 2) {
      // display the highscores
      show_highscores();
    } else if(selected_menu_entry == 3) {
      // display a dialog which explains the controls of the game
      display_controls();
    } else if(selected_menu_entry == 4) {
      // display a dialog which explains the elements of the game
      display_help();
    } else if(selected_menu_entry == 5) {
      // clear highscores
      if(clear_score_dialog() == 1) {
          clear_highscore();
      }
    }
    // leave if the menu entry "exit" is chosen
    */

    //메뉴 1 ~ 6
    switch(selected_menu_entry){
    case 1:
      //게임 실행
      run();
      break;

    case 2:
      //하이스코어 보기
      show_highscores();
      break;

    case 3:
      //조작법 보기
      display_controls();
      break;

    case 4:
      //도움말 보기
      display_help();
      break;

    case 5:
      //하이스코어 지우기, 1로 확인
      if(clear_score_dialog() == 1){
        clear_highscore();
      }
      break;

    }

  } while(selected_menu_entry != 6);
}

void init_curses() {

#if _BSD_SOURCE || _POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600
  // by default, ncurses stops the execution of the program for
  // 1 second if escape is pressed. Disable this behaviour.
  //Ncurses의 디폴트 기능으로 1이 눌리면 프로그램의 실행을 정지한다.이러한 기능 끄기
  setenv("ESCDELAY", "0", 1);
#endif
  
  initscr();
  // get more control over the input
  // 인풋 대해 더 컨트롤
  cbreak();
  // getch() returns ERR if no input is present and doesn't wait
  //getch()가 인풋이 없으면 ERR를 리턴하고 기다리지 않기
  nodelay(stdscr, TRUE);
  // don't echo the inserted keys to the screen
  //화면에 인풋 키 보여주지 않기
  noecho();
  // colors!
  //색갈 정의
  start_color();
  set_colors();
  // also grab keys like F1 etc.
  // f1등의 키도 인풋받기
  keypad(stdscr, TRUE);
}

void set_colors() {
  // these don't actually have anything to do with binary, so we are free to use "normal" numbers
  //바이너리 아님, 정수 사용
  init_pair(1, COLOR_RED, COLOR_BLACK); // red on black, 빨/검
  init_pair(2, COLOR_GREEN, COLOR_BLACK); // green on black, 초/검
  init_pair(3, COLOR_YELLOW, COLOR_BLACK); // yellow on black, 노/검
  init_pair(4, COLOR_BLUE, COLOR_BLACK); // blue on black, 파/검
  init_pair(5, COLOR_MAGENTA, COLOR_BLACK); // magenta on black, 보/검
  init_pair(6, COLOR_CYAN, COLOR_BLACK); // cyan on black, 파/검
  init_pair(7, COLOR_WHITE, COLOR_BLACK); // white on black, 흰/검
}

void end_curses() {
  endwin();
}
