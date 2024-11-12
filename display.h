/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "common.h"

// [ ���� ���� ]
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


// [ KEY ��ɾ� �Լ� ]
void state_spacebar(RESOURCE *resource, CURSOR cursor, int check_friend[MAP_HEIGHT][MAP_WIDTH], 
	bool p_key_press, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], bool d_key_press, bool g_key_press,
	bool b_b_key_press, bool s_key_press);

void state_esc(void);

void press_h(RESOURCE *resource, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]);

void press_b(RESOURCE* resource, CURSOR cursor, int check_friend[MAP_HEIGHT][MAP_WIDTH]);

void press_p(void);

void press_d(void);

void press_g(void);

void press_b_b(void);

void press_s(void);

// [ �ý��� �޽��� ]
void p_system_message(char* new_message);


// [ �ǹ� �Լ� ]
void p_f_base(OBJECT_BUILDING fb, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]);	 // �Ʊ� ����
void p_e_base(OBJECT_BUILDING eb, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]);	 // ���� ����
void p_f_plate(OBJECT_BUILDING fp, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]);	 // �Ʊ� ����
void p_e_plate(OBJECT_BUILDING ep, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]);	 // ���� ����
void p_start_spice(OBJECT_BUILDING ss, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);// �Ʊ� ���� �����̽�
void p_rock_1(OBJECT_BUILDING r1, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);	 // ���� 1
void p_rock_2(OBJECT_BUILDING r2, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);	 // ���� 2
void p_rock_3(OBJECT_BUILDING r3, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);	 // ���� 3
void p_rock_4(OBJECT_BUILDING r4, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);	 // ���� 4
void p_building(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]);						 // ��� �ǹ� ���


#endif