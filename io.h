/*
* raw(?) I/O
*/
#ifndef _IO_H_
#define _IO_H_

#include "common.h"

KEY get_key(void);
void printc(POSITION pos, char ch, int color);
void gotoxy(POSITION pos);
void set_color(int color);
void prints(POSITION pos, char str[100]); // 문자열 출력 함수

#endif

