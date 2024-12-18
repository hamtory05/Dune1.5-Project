/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "common.h"

// [ 색상 정의 ]
#define COLOR_DEFAULT	15
#define COLOR_CURSOR	15
#define COLOR_RESOURCE  112
#define COLOR_BLACK		15
#define B_CURSOR_COLOR	230


// [ DISPLAY ]
void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor,
	char state_map[STATE_HEIGHT][STATE_WIDTH], 
	char sysmes_map[SYSMES_HEIGHT][SYSMES_WIDTH],
	char order_map[ORDER_HEIGHT][ORDER_WIDTH],
	int check_friend[MAP_HEIGHT][MAP_WIDTH],
	bool big_cursor
);


// [ KEY 명령어 함수 ]
void state_spacebar(RESOURCE *resource, CURSOR cursor, OBJECT_SAMPLE* f_hav_obj, OBJECT_SAMPLE* e_hav_obj,
	int check_friend[MAP_HEIGHT][MAP_WIDTH], bool p_key_press, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], bool d_key_press, bool g_key_press,
	bool b_b_key_press, bool s_key_press, OBJECT_SAMPLE* havs[MAX_HAV], bool* space_key_press);
void state_esc(void);
void press_h(RESOURCE *resource, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor, OBJECT_SAMPLE* f_hav_obj, int hav_count, OBJECT_SAMPLE* havs[MAX_HAV]);
void press_b(RESOURCE* resource, CURSOR cursor, int check_friend[MAP_HEIGHT][MAP_WIDTH]);
void press_p(void);
void press_d(void);
void press_g(void);
void press_b_b(void);
void press_s(void);
void press_s_s(CURSOR cursor, RESOURCE* resource, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], bool* space_key_press);
void press_f(CURSOR cursor, RESOURCE* resource, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], bool* space_key_press);


// [ 시스템 메시지 ]
void p_system_message(char* new_message);


// [ 건물 함수 ]
void p_f_base(OBJECT_BUILDING fb, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]);	 // 아군 본진
void p_e_base(OBJECT_BUILDING eb, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]);	 // 적군 본진
void p_f_plate(OBJECT_BUILDING fp, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]);	 // 아군 장판
void p_e_plate(OBJECT_BUILDING ep, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]);	 // 적군 장판
void p_start_spice1(OBJECT_BUILDING ss, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);// 아군 스파이스
void p_start_spice2(OBJECT_BUILDING ss, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);// 적군 스파이스
void p_rock_1(OBJECT_BUILDING r1, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);	 // 바위 1
void p_rock_2(OBJECT_BUILDING r2, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);	 // 바위 2
void p_rock_3(OBJECT_BUILDING r3, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);	 // 바위 3
void p_rock_4(OBJECT_BUILDING r4, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);	 // 바위 4
void p_building(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]);						 // 모든 건물 출력

void hako_building(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]);

#endif