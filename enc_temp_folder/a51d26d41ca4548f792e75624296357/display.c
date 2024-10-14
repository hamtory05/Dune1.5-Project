/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#include "display.h"
#include "io.h"

// 출력할 내용들의 좌상단(topleft) 좌표
const POSITION resource_pos = { 0, 0 };
const POSITION map_pos = { 1, 0 };


char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);


void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor)
{
	display_resource(resource);
	display_map(map);
	display_cursor(cursor);
	// display_system_message()
	// display_object_info()
	// display_commands()
	// ...
}

void display_resource(RESOURCE resource) {
	set_color(COLOR_RESOURCE);
	gotoxy(resource_pos);
	printf("spice = %d/%d, population=%d/%d\n",
		resource.spice, resource.spice_max,
		resource.population, resource.population_max
	);
}

// subfunction of draw_map()
void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]) {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			for (int k = 0; k < N_LAYER; k++) {
				if (src[k][i][j] >= 0) {
					dest[i][j] = src[k][i][j];
				}
			}
		}
	}
}

void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	project(map, backbuf);

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (frontbuf[i][j] != backbuf[i][j]) {
				POSITION pos = { i, j };

				// [ 아군 본진 ]
				if (backbuf[i][j] == 'B' && (i == 16 || i == 15) && (j == 1 || j == 2)) {
					// < 파란색 배경 >
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT + 16);
				}

				// [ 적군 본진 ] 
				else if (backbuf[i][j] == 'B' && (i == 1 || i == 2) && (j == 57 || j == 58)) {
					// < 빨간색 배경 >
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT + 64);
				}

				// [ 아군 적군 장판 ]
				else if (backbuf[i][j] == 'P') {
					// < 검은색 배경 >
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT);
				}

				// [ 스파이스 ]
				else if (backbuf[i][j] == '5') {
					// < 주황색 배경 >
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT + 48);
				}

				else {
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT);
				}

				// 샌드웜 색깔 + 96



			}
			frontbuf[i][j] = backbuf[i][j];
		}
	}
}

// frontbuf[][]에서 커서 위치의 문자를 색만 바꿔서 그대로 다시 출력
void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;

	char ch = frontbuf[prev.row][prev.column];
	printc(padd(map_pos, prev), ch, COLOR_DEFAULT);

	ch = frontbuf[curr.row][curr.column];
	printc(padd(map_pos, curr), ch, COLOR_CURSOR);

	// 이전 위치의 문자를 기본 색으로 출력
	char prev_char = frontbuf[prev.row][prev.column];

	// 이전 위치가 'B'인 경우, 파란색 배경으로 설정
	if (prev_char == 'B') {
		printc(padd(map_pos, prev), prev_char, COLOR_DEFAULT + 16);
	}
	else {
		printc(padd(map_pos, prev), prev_char, COLOR_DEFAULT);
	}

	// 현재 위치의 문자를 확인하고, 'B'일 경우 파란색 배경으로 출력
	char curr_char = frontbuf[curr.row][curr.column];
	if (curr_char == 'B') {
		// 'B' 문자의 배경색을 파란색으로 설정
		printc(padd(map_pos, curr), curr_char, COLOR_DEFAULT + 16);
	}
	else {
		// 기본 색으로 커서 출력
		printc(padd(map_pos, curr), curr_char, COLOR_CURSOR);
	}



}