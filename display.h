/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "common.h"

// 표시할 색상 정의. 대충 맞춰 뒀는데, 취향껏 추가하거나 변경하기
#define COLOR_DEFAULT	15
#define COLOR_CURSOR	15
#define COLOR_RESOURCE  112
#define COLOR_BLACK 15



// 지금은 자원, 맵, 커서만 표시
// 앞으로 화면에 표시할 내용들 여기에 추가하기
void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor,
	char state_map[STATE_HEIGHT][STATE_WIDTH], 
	char sysmes_map[SYSMES_HEIGHT][SYSMES_WIDTH],
	char order_map[ORDER_HEIGHT][ORDER_WIDTH],
	int check_friend[MAP_HEIGHT][MAP_WIDTH]
);

void state_spacebar(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	char state_map[STATE_HEIGHT][STATE_WIDTH], CURSOR cursor,
	int check_friend[MAP_HEIGHT][MAP_WIDTH]);

void state_esc(char state_map[STATE_HEIGHT][STATE_WIDTH], char order_map[ORDER_HEIGHT][ORDER_WIDTH]);

void press_h(RESOURCE *resource, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char sysmes_map[SYSMES_HEIGHT][SYSMES_WIDTH],
	int check_friend[MAP_HEIGHT][MAP_WIDTH]);

void p_system_message(char str[], char sysmes_map[SYSMES_HEIGHT][SYSMES_WIDTH]);

// [ 건물 함수 ]
void p_f_base(OBJECT_BUILDING fb, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]);	 // 아군 본진
void p_e_base(OBJECT_BUILDING eb, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]);	 // 적군 본진
void p_f_plate(OBJECT_BUILDING fp, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]);	 // 아군 장판
void p_e_plate(OBJECT_BUILDING ep, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]);	 // 적군 장판
void p_start_spice(OBJECT_BUILDING ss, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);// 아군 적군 스파이스
void p_rock_1(OBJECT_BUILDING r1, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);	 // 바위 1
void p_rock_2(OBJECT_BUILDING r2, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);	 // 바위 2
void p_rock_3(OBJECT_BUILDING r3, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);	 // 바위 3
void p_rock_4(OBJECT_BUILDING r4, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);	 // 바위 4
void p_building(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]);						 // 모든 건물 출력


#endif