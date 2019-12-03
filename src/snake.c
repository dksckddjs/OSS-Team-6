#include "snake.h"
#include "events.h"

SNAKEBODY *getLastBody(SNAKE *snake) {
	SNAKEBODY *tmp = snake->head;
	while (tmp->next != NULL) {
		tmp = tmp->next;
	}
	return tmp;
}


// check if a part of the snake is on the given spot
//뱀의 일부가 특정 위치에 있는지 확인
// returns a WINDOW * if one is found, NULL if not
//있으면 WINDOW * 리턴, 없으면 NULL
WINDOW *snake_part_is_on(SNAKE *snake, int posy, int posx) {
	int i;
	int cury, curx; // position of the current snake part we are iterating, 뱀의 위치

	// iterate each part
	//각 부분 들어가기
   /*
	for(i = 0; i < snake->length; i++) {
	  // get the position of the current part
	  //현재 부위의 좌표값 구하기
	  getbegyx(snake->parts[i], cury, curx);//cury와 curx가 반환된 현재 뱀의 위치

	  // compare the position
	  //좌표값 비교
	  if(cury == posy && curx == posx) {
		// return the current part
		//현재 부분 리턴
		return snake->parts[i];
	  }
	}
	*/
	SNAKEBODY *sb = snake->head;
	while (sb->next != NULL) {
		if (sb->posx == curx && sb->posy == cury) {
			return sb->body;
		}
		sb = sb->next;
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
	/*
	if(snake->length == 0) {
	  // initialize the array with malloc if it is the first part
	  //처음 칸이면 malloc
	  snake->parts = malloc(sizeof(WINDOW*) * snake->length);
	} else {
	  // increase the amount of allocated memory
	  //처음 칸이 아니면 realloc
	  snake->parts = realloc(snake->parts, sizeof(WINDOW*) * snake->length);
	}
	*/
	SNAKEBODY* newBody;//새 삽입 부분
	newBody = (SNAKEBODY*)malloc(sizeof(SNAKEBODY));
	newBody->next = NULL;

	// create a new window
	//새로운 윈도우 생성
	if (snake->length == 0) {//머리 처리(처음 케이스)
		snake->head = newBody;
	}
	else {//snake->parts[snake->length - 1] = win = newwin(1, 1, posy, posx);//배열의 마지막 위치에 뱀의 새부분을 저장
		getLastBody(snake)->next = newBody;
	}
	newBody->body = win = newwin(1, 1, posy, posx);

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
	// 뱀 삭제

	// 뱀의 머리가 NULL일 때 까지 반복한다.
	while (snake->head != NULL) {
		SNAKEBODY *tmp = getLastBody(snake); // 먼저 뱀의 마지막 부분을 구하고
		if (tmp == snake->head) { // 마지막 부분이 머리인지 확인한다.
			delwin(tmp->body);
			snake->head = NULL;
			free(tmp);
			return;
		}
		SNAKEBODY *tmp2 = snake->head;
		while (tmp2 != tmp) tmp2 = tmp2->next; // 마지막 부분이 머리가 아니라면 뱀의 마지막에서 두번째로 간다.
		tmp2->next = NULL; // 마지막에서 두번째에서 마지막 뱀의 몸에 접근할 수 있기 때문에
		delwin(tmp->body); // 이렇게 할당 해제한다.
		free(tmp);
	}

	// free the resources allocated for the window pointers
	//윈도우 포인터 대한 메모리 프리

	return;
}

// move the snake
//뱀 움직이기
int move_snake(GAME *game) {
	int success = 1, i;
	SNAKEBODY *head = game->snake->head;

	// some variables containing positions of certain things
	//특정 윈도우의 좌표
	int curx, cury, tmpy, tmpx;

	// callbacks to check collisions
	//충돌 검사 테이블
	int(*collision_checks[EVENTS])(GAME*, int, int) = {
	  check_self_collision,
	  check_extended_border_collision,
	  check_fruit_collision
	};

	// callbacks which will be called automaticly if a collision is detected
	// the callback at position 1 in the array belongs to the check-function
	// at position 1 from the array above
	//충돌 일어나면 자동으로 핸들러 실행
	int(*collision_handlers[EVENTS])(GAME*, int, int) = {
	  check_self_collision_handler,
	  check_border_collision_handler,
	  check_fruit_collision_handler
	};


	// difference on x-axis according to the direction
	//뱀이 움직이는 방향과 x축 대한 차이
	int xdiff = game->snake->dir == DIR_LEFT ? -1 : (game->snake->dir == DIR_RIGHT ? 1 : 0);
	// difference on y-axis according to the direction
	//뱀이 움직이는 방향과 y축 대한 차이
	int ydiff = game->snake->dir == DIR_UP ? -1 : (game->snake->dir == DIR_DOWN ? 1 : 0);

	// the position of the snake head
	//뱀 머리 위치
	getbegyx(head->body, cury, curx);

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
	for (i = 0; i < EVENTS && success; i++) {
		// collision?
		//충돌?
		if ((collision_checks[i](game, cury, curx))) {
			// should we end the game because of the collision?
			//뱀 죽음?
			if (!collision_handlers[i](game, cury, curx)) {
				success = 0;
			}
		}
	}

	// no collisions ?
	//충돌 없음?
	if (success) {
		// set he direction of the head
		//뱀 머리의 방향 설정

		mvprintw(game->snake->dir, 0, 0, "%c", game->snake->dir);
		// move the window
		//윈도우 움직이기
		mvwin(game->snake->head, cury, curx);

		// copy values back
		//다시 복사하기
		cury = tmpy;
		curx = tmpx;

		// iterate through each part of the snake
		//뱀이 각 부위별로 들어가기

		/*
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
		*/
		SNAKEBODY *tmp = head->next;
		while (tmp->next != NULL) {
			getbegyx(tmp, tmpy, tmpx);
			mvwin(tmp, cury, curx);

			cury = tmpy;
			curx = tmpx;
			tmp = tmp->next;
		}
		if (game->snake->grow > 0) {
			grow_snake(&game->snake, cury, curx);
			game->snake->grow--;
		}
		else {
			getbegyx(head, tmpy, tmpx);
			if (!(tmpy == cury && tmpx == curx)) {
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
	// iterate through each part of the snake
	//뱀 각 부위별로 들어가기
	SNAKEBODY *sb = snake->head;
	while (sb->next != NULL) {
		redrawwin(sb->body);
		wrefresh(sb->body);
		sb = sb->next;
	}
}

