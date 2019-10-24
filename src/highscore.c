#include "highscore.h"

// calculate the highscore out of the points and the time needed
//하이스코어를 저무와 시간으로 계산
int calculate_score(int points, long time_sec) {
  // 10 to highscore for every point you make -2 for every second you need
  //1점당 하이스코어 10점, -2점당 시간 1초
  return points * 10 - time_sec * 2;
}

char* user_home;
char* hscore_file_path;
size_t hscore_path_len;
char* hscore_file;

#ifdef _WIN32
#include <windows.h>
int msnake_mkdir(char * dirname) {
    return _mkdir(dirname);
}
#else
#include <sys/types.h>
#include <sys/stat.h>
int msnake_mkdir(char * dirname) {
    return mkdir(dirname, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
}
#endif

static char *highscore_file = NULL;
char *get_highscore_file() {
    if(highscore_file == NULL) {
        // get the highscore file name
        //하이스코어 파일 이름 불러오기
        char *user_home = getenv("HOME");
        if(user_home == NULL) {
            // if no $HOME variable is available we probably are on windows
            //$HOME이 없으면 윈도스로 추측
            user_home = getenv("APPDATA");
            if(user_home == NULL) {
                // if that still didn't work, simply use the current directory
                //그래도 안되면 현재 디렉토리 사용
                user_home = ".";
            }
        }

        int highscore_directory_path_length = strlen(user_home) + 1 + strlen(HIGHSCORE_DIR);
        int highscore_path_length = highscore_directory_path_length + 1 + strlen(HIGHSCORE_FILE) + 1;

        highscore_file = malloc(highscore_path_length);
        strcpy(highscore_file, user_home);
        strcat(highscore_file, "/");
        strcat(highscore_file, HIGHSCORE_DIR);

        // make sure the directory exists
        msnake_mkdir(highscore_file);
        strcat(highscore_file, "/");
        strcat(highscore_file, HIGHSCORE_FILE);
    }
    return highscore_file;
}


// read the highscore file
//하이스코어 파일 읽기
HIGHSCORE *read_highscore(int *num) {
  // static pointer to an array of highscore records
  //하이스코어 배열 대한 스태틱 포인터
  static HIGHSCORE *ptr = NULL;
  // the highscore file handle
  //하이스코어 파일 핸들
  FILE *hs_file;
  int x, swapped;
  // the current record
  //현재 레코드
  HIGHSCORE current;

  // if a NULL pointer is given, free all the allocated memory and exit
  //NULL 포인터이면 메모리 프리하고 종료
  if(num == NULL) {
    free(ptr);
    ptr = NULL;
    free(highscore_file);
    highscore_file = NULL;
    return NULL;
  }

  // set num to 0 in case the highscore file cannot be opened
  //파일을 못열수도 있으니 num을 0으로 초기화
  *num = 0;

  // open the highscore file
  //파일 열기
  if((hs_file = fopen(get_highscore_file(), "r")) == NULL) {
    return NULL;
  }

  // free the allocated memory of calls before
  //전에 콜 대한 메모리 프리
  if(ptr != NULL) {
    free(ptr);
  }

  // initialize ptr
  //포인터 초기화
  ptr = malloc(*num);

  // read the records into the `ptr`-array
  //하이스코어를 포인터 위치로 읽어들이기
  while(fread(&current, sizeof(HIGHSCORE), 1, hs_file) != 0) {
    // next record
    //다음 항목
    (*num)++;
    // allocate memory for the new record
    //새로운 스코어 대해 메모리 realloc
    ptr = realloc(ptr, sizeof(HIGHSCORE) * *num);
    // save the new record in the array
    //새로운 레코드 배열에 저장
    ptr[*num - 1] = current;
  }

  // sort the whole thing using a simple bubble sort algorithm
  //버블 소트로 정렬
  do {
    // if this variable contains 0 at the end of the following for loop the
    // array is properly sorted
    //마지막 값이 0이면 정렬 성공
    swapped = 0;
    // iterate througth each record except the last
    //마지막 빼고 각각 들어가기
    for(x = 0; x < *num - 1; x++) {
      // check if the next record's value is greater than the value of the current one
      //다음 레코드가 현재보다 큰지 확인
      if(ptr[x].highscore < ptr[x + 1].highscore) {
        // swap the position of the two elements
        //서로 바꾸기
        current = ptr[x];
        ptr[x] = ptr[x + 1];
        ptr[x + 1] = current;
        // the array isn't properly sorted
        // 정렬 실패
        swapped = 1;
      }
    }
  } while (swapped);

  // return the array
  //배열 리턴
  return ptr;
}

// add an entry to the highscore file
//하이스코어 파일에 항목 추가
int add_highscore(char *name, int points, int time_sec) {
  FILE *hs_file = NULL;
  HIGHSCORE highscore;

  // write the values into the struct
  // 값을 구조체어 쓰기
  strncpy(highscore.name, name, HIGHSCORE_NAME_LENGTH - 1);
  highscore.name[HIGHSCORE_NAME_LENGTH - 1] = '\0'; // make sure the string is ended properly
  highscore.points = points;
  highscore.time_sec = time_sec;
  highscore.highscore = calculate_score(points, time_sec);

  // open the highscore file in append mode
  //append모드로 하이스코어 파일 열기
  if((hs_file = fopen(get_highscore_file(), "a")) == NULL) {
    return 1;
  }
  // append a structure to the file
  //구조체를 파일에 붙히기
  fwrite(&highscore, sizeof(HIGHSCORE), 1, hs_file);
  // close the file
  //파일 닫기
  fclose(hs_file);
  return 0;
}

// clear the highscore file
//하이스코어 파일 지우기
void clear_highscore() {
  FILE *hs_file = NULL;
  // open the file in write mode so it gets truncated
  //write모드로 열어서 내용 지우기
  if((hs_file = fopen(get_highscore_file(), "w")) != NULL) {
    fclose(hs_file);
  }
}
