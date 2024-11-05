/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "common.h"

// ǥ���� ���� ����. ���� ���� �״µ�, ���ⲯ �߰��ϰų� �����ϱ�
#define COLOR_DEFAULT	15
#define COLOR_CURSOR	15
#define COLOR_RESOURCE  112
#define COLOR_BLACK 15



// ������ �ڿ�, ��, Ŀ���� ǥ��
// ������ ȭ�鿡 ǥ���� ����� ���⿡ �߰��ϱ�
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