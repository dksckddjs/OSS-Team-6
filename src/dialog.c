#include "types.h"
#include "highscore.h"
#include "dialog.h"

// create a basic dialog with title which looks like this:
// 기본 다이얼로그 윈도우 생성
// +--------------------------------------+
// +---------------- TITLE ---------------+
// +--------------------------------------+
// |                                      |
// |                                      |
// |                                      |
// |                                      |
// |                                      |
// |                                      |
// |                                      |
// |                                      |
// |                                      |
// |                                      |
// |                                      |
// |                                      |
// |                                      |
// |                                      |
// |                                      |
// |                                      |
// +--------------------------------------+
//
WINDOW *create_dialog_window(const char *title) {
  WINDOW *win;
  int sx, sy, i, u, startpos;

  // get the screen size
  // 현재 화면 크기 얻기
  getmaxyx(stdscr, sy, sx);

  /*
  18011561_cleanCode
  필요한 상수를 제외한 상수를 없애기 위해 매크로 상수로 변경
  수식의 가시성을 높이기 위해 괄호 씌우기
  // create a little window in the center of the screen with a border and a size of 40x20
  // 중심에 40x20크기의 윈도우 생성
  win = newwin(DIALOG_HEIGHT, DIALOG_WIDTH, sy / 2 - DIALOG_HEIGHT / 2, sx / 2 - DIALOG_WIDTH / 2);
  */

  // create a little window in the center of the screen with a border and a size of 40x20
  // 중심에 40x20크기의 윈도우 생성
   win = newwin(DIALOG_HEIGHT, DIALOG_WIDTH, (sy / 2) - DIALOG_HEIGHT_HALF, (sx / 2) - DIALOG_WIDTH_HALF);

 

  // create the border
  // 윈도우 테두리 생성
  wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');

  // fill the first two lines with "-" and "+" on the border
  // 처음 2줄을 "-" "+"로 채움
  /*
  18011561_cleanCode
  1, 2, 0 을 매크로 상수로 변경 (하드코딩)

   for(u = 1; u <= 2; u++) {
     for(i = 0; i < DIALOG_WIDTH; i++) {
     mvwprintw(win, u, i, "%c", i == 0 || i == DIALOG_WIDTH - 1 ? '+' : '-');
     }
   }
  */
    for(u = LINE1; u <= LINE2; u++) {
    for(i = INDEX0; i < DIALOG_WIDTH; i++) {
      mvwprintw(win, u, i, "%c", i == INDEX0 || i == INDEX_LAST ? '+' : '-');
    }
  }

  /*
  18011561_cleanCode
  불필요한 상수를 없애고 괄호를 씌워서 수식의 가독성을 높임
  // print the title in the middle of the dialog
  //윈도우 제목 적기
  startpos = DIALOG_WIDTH / 2 - strlen(title) / 2;  // 하드코딩?
  // print a space before the title
  //제목 앞에 스페이스
  mvwprintw(win, 1, startpos - 1, " ");
  // print the title itself
  //제목 자체 출력
  mvwprintw(win, 1, startpos, "%s", title);
  // print a space after the title
  //제목 뒤에 스페이스 출력
  mvwprintw(win, 1, startpos + strlen(title), " ");
  */

  // print the title in the middle of the dialog
  // 윈도우 제목 적기
  startpos = DIALOG_WIDTH_HALF - (strlen(title) / 2);  // 하드코딩?
  // print a space before the title
  // 제목 앞에 스페이스
  mvwprintw(win, SPACE, startpos - 1, " ");
  // print the title itself
  // 제목 자체 출력
  mvwprintw(win, SPACE, startpos, "%s", title);
  // print a space after the title
  // 제목 뒤에 스페이스 출력
  mvwprintw(win, SPACE, startpos + strlen(title), " ");

  return win;
}

// wait until enter is pressed
// 엔터 눌릴때까지 디라기ㅣ
void wait_return(WINDOW *win) {
  int ch; // the char currently pressed 현제 입력값

  // request a char
  // 입력 요청
  while((ch = wgetch(win))) {
    // is it the "enter"-key?
    // 엔터키?
    /*
    18011561_cleanCode
    이 while문 아래로 더 실행할 작업이 없는것을 보고
    break; 이 아닌 return; 으로 변경
    if(ch == '\n') {
      // get out of here
      // 나가기
      break;
    } 
    */

    if(ch == '\n') { 
      // get out of here
      // 나가기
      return;
    } 
  }
}

// creates a simple dialog with content, you can press 'enter' to close the dialog
// 내용 첨가된 간단한 다이얼로그 윈도우 생성, 엔터로 나가기
int create_enter_dialog(const char *title, const char *contents, int lines) {
  WINDOW *win = create_dialog_window(title);
  int i;

  // insert the stuff in "contents" into the dialog
  // "contents" 안의 내용 첨가
  for(i = INDEX0; i < lines; i++) {
    mvwprintw(win, i + 3, 1, "%s", &contents[i * CONTENT_WIDTH]);
  }

  // display the dialog
  // 다이얼로그 표시
  wrefresh(win);

  // wait for 'enter'
  // 엔터 기다리기
  wait_return(win);
  
  // free the llocated memory of the window
  // 윈도우에 할당된 메모리 프리
  delwin(win);
  return 0;
}


// creates a dialog with menu entries, you can press numbers to select a menu entry and close the dialog
// 숫자 리스트 있는 메뉴 표시, 숫자 누르면 선택 후 종료
int create_numbered_dialog(const char *title, const char *contents, int lines) {
  WINDOW *win = create_dialog_window(title);
  int i, ch, number = 0;

  // insert menu entries into the dialog
  // 메뉴 내용 첨가
  for(i = INDEX0; i < lines; i++) {
    mvwprintw(win, i + 3, 1, &contents[i * CONTENT_WIDTH], i + 1);
  }

  // display the dialog
  // 윈도우 보여주기
  wrefresh(win);

  // wait for some input
  // 인풋 기다리기
  while((ch = wgetch(win))) {
    // error? begin again.
    // error? 다시 인풋
    if(ch == ERR) continue;

    // select the first menu entry if enter is pressed
    // 엔터 눌리면 1번 선택
    if(ch == '\n') ch = '1';
    // a number pressed?
    // 숫자가 입력되었나?
    if(ch >= '0' && ch <= '9') {
      number = ch - '0';
      // prevent from handling numbers which are > than the number of menu entries
      // 메뉴 항목보다 많은 숫자 무시
      
      /* 
         18011561_cleanCode
         number = ch - '0' 하였는데 if의 조건에서 ch - '0'을 사용하고있어서
         ch - '0' 을 number로 변경하였고
         어차피 허락된 숫자 이외에는 눌러도 대기 하기 때문에
         범위 내의 숫자를 입력 하였을때 윈도우를 없애고
         숫자값을 반환하는것으로 변경
       
         if(ch - '0' <= lines) {
         // get out of the loop
         //루프 나가기
         break;
       }
      */

      if(number <= lines) {
        // get out of the loop
        // return the number pressed
        // 선택된 숫자 반환
        delwin(win);
        // 윈도우 없애기

        return number;
      }
    }
  }
  // 입력된 number 
  // delete the window
  // 윈도우 없애기
  // delwin(win);

  // return the number pressed
  // 선택된 숫자 리턴
  // return number;
  
}

// displays the main menu
//메인 메뉴 표시
int display_menu() {
  // the contents of the menu
  //메뉴 내용
  // %i wil be replaced with the number of the menu entry
  //&i는 메뉴항목 내용
  char menu[][CONTENT_WIDTH] = {
  "%i) Start the game",
  "%i) Highscores",
  "%i) Controls",
  "%i) Help",
  "%i) Clear Highscores",
  "%i) Exit"
  };
  
  // create a numbered dialog
  //숫자 리스트 다이얼로그 윈도우 생성
  return create_numbered_dialog("MENU", (char *)menu, 6);
}

// displays the controls dialog
// 조작법 표시
void display_controls() {
  // the dialog contents
  // 조작법 내용
  char controls[][CONTENT_WIDTH] = {
  "w - move up",
  "a - move left",
  "s - move down",
  "d - move right",
  "you can use the arrow keys too",
  "",
  "8 - slower",
  "9 - faster",
  "0 - reset speed",
  "p - pause the game"
  "",
  "press enter to go back to the menu .."
  };

  // create the dialog
  // 이다얼로그 생성
  create_enter_dialog("CONTROLS", (const char *)controls, 11);
}

// displays the help menu
// 도움말 표시
void display_help() {
  // the contents of the dialog
  // 도움말 내용
  char help[][CONTENT_WIDTH] = {
  " < ^ > v  the snake head",
  " O        the snake body",
  "          you shouldn't hit yourself  ",
  " #        the wall, don't hit it!",
  " x        simple food, +1 point",
  "          +1 length, +1 new food",
  " @        double food, +1 point",
  "          +1 length, +2 new food",
  " %        mega food, +10 points",
  "          +5 length, +1 new food",
  " &        eat boost, increases",
  "          range of eating for a",
  "          short time",
  "",
  "press enter to go back to the menu .."
  };

  // create the dialog
  // 윈도우 생성
  create_enter_dialog("HELP", (const char *)help, 15);
}

// display a mesage to the user that the highscore has been cleared
// 하이스코어가 삭제되었다는 메뉴 표시
int clear_score_dialog() {
  // the contents of the dialog
  // 다이얼로그 내용
  char text[][CONTENT_WIDTH] = {
  "%i) Delete all the saved scores.",
  "%i) No. Don't delete them.", 
  };

  // create the dialog
  // 다이얼로그 생성
  return create_numbered_dialog("CLEAR SCORES?", (char *)text, 2);
}

// displays the pause dialog
// 일시 정지 표시
int pause_dialog() {
  // the contents of the dialog
  // 일시 정지 내용
  char dialog[][CONTENT_WIDTH] = {
  "%i) Resume",
  "%i) Exit"
  };

  // display the numbered dialog
  // 숫자 리스트 다이얼로그 표시
  return create_numbered_dialog("PAUSE", (char *)dialog, 2);
}

// display a table containing the highscores of the players
//하이스코어 담긴 테이블 표시
void show_highscores() {
  int num, i;
  char *highscore_table;
  // read all the submitted highscores from a file
  // 파일에서 하이스코어 읽기
  HIGHSCORE *highscore = read_highscore(&num);

/*
  18011561_cleanCode
  구조상 불필요한 if문을 제거하고, 매크로 상수를 사용하여 하드코딩을 수정하였다.
  // limit the highscore display to (at the moment 14) entries
  // 하이스코어를 14개만 표시
  if(num > DIALOG_HEIGHT - 6) {
    num = DIALOG_HEIGHT - 6;
  }
*/
  // limit the highscore display to (at the moment 14) entries
  // 하이스코어를 14개만 표시
  num = HIGHSCORE_LIMIT;

  // create a char array for the content of the highscore table
  //하이스코어 내용
  highscore_table = calloc((num + 2) * CONTENT_WIDTH, sizeof(char));

  // create a simple table heading inside of the dialog
  //다이얼로그에 간단한 헤더 생성
  snprintf(highscore_table, CONTENT_WIDTH, "POS            NAME  PTS   SEC  SCORE");
  snprintf(highscore_table + CONTENT_WIDTH, CONTENT_WIDTH, "-------------------------------------");

  // insert the highscores
  //하이스코어 대입
  for(i = 0; i < num; i++) {
    // insert a highscore record
    //하이스코어 점수 배열에 대입
    snprintf(highscore_table + (i + 2) * CONTENT_WIDTH, // the position on the highscore table array  하이스코어 배열에서 위치
      CONTENT_WIDTH, // do not allow the line to be longer than 38 chars  38자보다 짧게
      "%2i. %15s %4i  %4li  %5i", // the format of the line  줄 포맷
      i + 1, // the rank  등수
      highscore[i].name, // the data 점수
      highscore[i].points,
      highscore[i].time_sec,   
      highscore[i].highscore);
  }

  // create a dialog and display the data
  // 다이얼로그 생성 후 보여주기
  create_enter_dialog("HIGHSCORES", highscore_table, num + 2);

  // free the resources
  // 자원 프리
  free(highscore_table);
}

void enter_string(char *title, char *content, int lines, int posy, int posx, char *buf, int length) {
  WINDOW *win = create_dialog_window(title);
  int i;
  int pos = 0;
  int ch = 0;

  // print out the content
  // 내용 충전
  for(i = 0; i < lines; i++) {
    mvwprintw(win, i + 3, 1, "%s", &content[i * CONTENT_WIDTH]);
  }

  // - let the user input a string
  // - 유저가 스트링 입력
  // read a char until we receive a line break
  // 엔터 나올때까지 받기
  while((ch = mvwgetch(win, posy, posx + pos)) && ch != '\n') {
    // prevent a buffer overflow
    // 버퍼 오버플로우 방지
    if(ch == KEY_BACKSPACE || ch == 127) {
      // delete the character at the current position from the screen and the buffer
      // 지우기 기능
      buf[pos] = '\0';
      mvwprintw(win, posy, posx + pos, " ");

      // go 1 back in the buffer but not under 0
      // 뒤로 1칸 가기, 단 0 이하로는 가지 말기
      if(pos > 0) {
        pos--;
      }

      // delete the last typed character from the screen and the buffer
      // 마지막으로 친 캐릭터 지우기
      mvwprintw(win, posy, posx + pos, " ");
      buf[pos] = '\0';
    
    // check if it is a letter
    //글자인지 확인
    } else if((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
      // print the character to the screen
      // 화면에 글자 출력
      mvwprintw(win, posy, posx + pos, "%c", ch);

      // write the typed key into the buffer
      // 친 키를 버퍼에 쓰기
      buf[pos] = ch; 
      // move forward in the buffer but not over the length - 1
      // 버퍼를 앞으로 진행, 단 -1 초과하지 않기
      if(pos < length - 1) {
        pos++;
      }
    }
  }

  buf[length - 1] = '\0';
  // delete the dialog
  // 다이얼로그 지우기
  delwin(win);
}

// displays the highscore dialog
// 하이스코어 보여주기
void display_highscore(GAME *game, char *buf, int length) {
  int lines = 6;
  // allocate enougth memory
  // 메모리 할당
  char *content = calloc(lines * CONTENT_WIDTH, sizeof(char));

  // generate the window contents
  // 내용 생성
  snprintf(content                    , CONTENT_WIDTH, "snake length : %i", game->snake.length);
  snprintf(content + CONTENT_WIDTH    , CONTENT_WIDTH, "points       : %i", game->highscore);
  snprintf(content + CONTENT_WIDTH * 2, CONTENT_WIDTH, "time         : %lis", game->ended - game->started - game->paused);
  snprintf(content + CONTENT_WIDTH * 3, CONTENT_WIDTH, "highscore    : %i", calculate_score(game->highscore, game->ended - game->started - game->paused));
  snprintf(content + CONTENT_WIDTH * 4, CONTENT_WIDTH, "---------------------------------");
  snprintf(content + CONTENT_WIDTH * 5, CONTENT_WIDTH, "name to be added to the highscore");
  snprintf(content + CONTENT_WIDTH * 5, CONTENT_WIDTH, "Name: ");

  // show the dialog
  // 다이얼로그 보여주기
  enter_string("GAME OVER", content, lines, 8, 7, buf, length);

  // free the memory
  // 메모리 프리
  free(content);
}
