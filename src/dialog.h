#ifndef DIALOG_H
#define DIALOG_H
#include "types.h"
#include <curses.h>

#define DIALOG_WIDTH 40
#define DIALOG_WIDTH_HALF (DIALOG_WIDTH/2)
#define DIALOG_HEIGHT 20
#define DIALOG_HEIGHT_HALF (DIALOG_HEIGHT/2)
#define CONTENT_WIDTH (DIALOG_WIDTH - 2)
#define LINE0 0
#define LINE1 1
#define LINE2 2
#define INDEX0 0
#define INDEX_LAST (DIALOG_WIDTH - 1)
#define SPACE 1
#define HIGHSCORE_LIMIT 14
WINDOW *create_dialog_window();
int create_numbered_dialog(const char *title, const char *contents, int lines);
int create_enter_dialog(const char *title, const char *contents, int lines);
void wait_return(WINDOW *win);
int display_menu();
void display_controls();
void display_help();
void display_highscore(GAME *game, char *buf, int length);
void show_highscores();
int pause_dialog();
int clear_score_dialog();
void enter_string(char *title, char *content, int lines, int posy, int posx, char *buf, int length);


#endif /* DIALOG_H */
