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
const POSITION state_pos = { 1, 63 };
const POSITION state_mes = { 2, 63 };

char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
int colorbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char state_backbuf[STATE_HEIGHT][STATE_WIDTH] = { 0 };
char state_frontbuf[STATE_HEIGHT][STATE_WIDTH] = { 0 };

void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);
void display_state_map(char state_map[STATE_HEIGHT][STATE_WIDTH]);
void state_project(char src[STATE_HEIGHT][STATE_WIDTH], char dest[STATE_HEIGHT][STATE_WIDTH]);


// =================================== [ 건물 ] ======================================= //

// ==== [ 구조체로 건물 선언 ] ==== //

// [ 아군 본진 BASE ]
OBJECT_BUILDING f_base = {
	.pos1 = {16, 1},
	.pos2 = {16, 2},
	.pos3 = {15, 1},
	.pos4 = {15, 2},
	.repr = 'B',
	.layer = 0
};

// [ 적군 본진 BASE ]
OBJECT_BUILDING e_base = {
	.pos1 = {1, 57},
	.pos2 = {1, 58},
	.pos3 = {2, 57},
	.pos4 = {2, 58},
	.repr = 'B',
	.layer = 0
};

// [ 초기 스파이스 매장지 2개 ]
OBJECT_BUILDING start_spice = {
	.pos1 = {12, 1},
	.pos2 = {5, 58},
	.repr = '5',
	.layer = 0
};

// [ 아군 장판 ]
OBJECT_BUILDING f_plate = {
	.pos1 = {15, 3},
	.pos2 = {15, 4},
	.pos3 = {16, 3},
	.pos4 = {16, 4},
	.repr = 'P',
	.layer = 0
};


// [ 적군 장판 ]
OBJECT_BUILDING e_plate = {
	.pos1 = {1, 55},
	.pos2 = {2, 55},
	.pos3 = {1, 56},
	.pos4 = {2, 56},
	.repr = 'P',
	.layer = 0
};

// [ 바위 1 ~ 4 ]
OBJECT_BUILDING rock_1 = {
	.pos1 = {12, 6},
	.repr = 'R',
	.layer = 0
};
OBJECT_BUILDING rock_2 = {
	.pos1 = {5, 50},
	.repr = 'R',
	.layer = 0
};
OBJECT_BUILDING rock_3 = {
	.pos1 = {3, 27},
	.pos2 = {3, 28},
	.pos3 = {4, 27},
	.pos4 = {4, 28},
	.repr = 'R',
	.layer = 0
};
OBJECT_BUILDING rock_4 = {
	.pos1 = {12, 35},
	.pos2 = {12, 36},
	.pos3 = {13, 35},
	.pos4 = {13, 36},
	.repr = 'R',
	.layer = 0
};

// ==== [ 함수로 건물 출력 ] ==== //

// [ 아군 본진 출력 함수 ]
void p_f_base(OBJECT_BUILDING fb, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	POSITION fb_num[] = { fb.pos1, fb.pos2, fb.pos3, fb.pos4 };
	for (int i = 0; i < 4; i++) {
		map[0][fb_num[i].row][fb_num[i].column] = fb.repr;
	}
}

// [ 적군 본진 출력 함수 ]
void p_e_base(OBJECT_BUILDING eb, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	POSITION eb_num[] = { eb.pos1, eb.pos2, eb.pos3, eb.pos4 };
	for (int i = 0; i < 4; i++) {
		map[0][eb_num[i].row][eb_num[i].column] = eb.repr;
	}
}

// [ 아군 장판 출력 함수 ]
void p_f_plate(OBJECT_BUILDING fp, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	POSITION fp_num[] = { fp.pos1, fp.pos2, fp.pos3, fp.pos4 };
	for (int i = 0; i < 4; i++) {
		map[0][fp_num[i].row][fp_num[i].column] = fp.repr;
	}
}

// [ 적군 장판 출력 함수 ] 
void p_e_plate(OBJECT_BUILDING ep, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	POSITION ep_num[] = { ep.pos1, ep.pos2, ep.pos3, ep.pos4 };
	for (int i = 0; i < 4; i++) {
		map[0][ep_num[i].row][ep_num[i].column] = ep.repr;
	}
}

// [ 초기 스파이스 매장지 2개  아군  적군 출력 함수 ]
void p_start_spice(OBJECT_BUILDING ss, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	POSITION ss_num[] = { ss.pos1, ss.pos2 };
	for (int i = 0; i < 2; i++) {
		map[0][ss_num[i].row][ss_num[i].column] = ss.repr;
	}
}

// [ 바위 1 ~ 4 출력 함수 ]
void p_rock_1(OBJECT_BUILDING r1, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	POSITION r1_num[] = { r1.pos1 };
	map[0][r1_num[0].row][r1_num[0].column] = r1.repr;
}
void p_rock_2(OBJECT_BUILDING r2, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	POSITION r2_num[] = { r2.pos1 };
	map[0][r2_num[0].row][r2_num[0].column] = r2.repr;
}
void p_rock_3(OBJECT_BUILDING r3, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	POSITION r3_num[] = { r3.pos1, r3.pos2, r3.pos3, r3.pos4 };
	for (int i = 0; i < 2; i++) {
		map[0][r3_num[i].row][r3_num[i].column] = r3.repr;
	}
}
void p_rock_4(OBJECT_BUILDING r4, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	POSITION r4_num[] = { r4.pos1, r4.pos2, r4.pos3, r4.pos4 };
	for (int i = 0; i < 2; i++) {
		map[0][r4_num[i].row][r4_num[i].column] = r4.repr;
	}
}


// [ 모든 건물 출력 함수 ]
void p_building(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {

	p_f_base(f_base, map); // 아군 본진

	p_e_base(e_base, map); // 적군 본진

	p_f_plate(f_plate, map); // 아군 장판

	p_e_plate(e_plate, map); // 적군 장판

	p_start_spice(start_spice, map); // 초기 스파이스 매장지 아군 1 적군 1

	p_rock_1(rock_1, map); // 바위 1 
	p_rock_2(rock_2, map); // 바위 2
	p_rock_3(rock_3, map); // 바위 3
	p_rock_4(rock_4, map); // 바위 4
	
}


// =================================== [ 유닛 ] ======================================= //

// ==== [ 구조체로 유닛 선언 ] ==== //

// [ 샌드웜 ]
OBJECT_SAMPLE sandworm_1 = {
	.pos = {2, 4},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'W',
	.move_period = 2500,
	.next_move_time = 2500,
	.layer = 1
};

OBJECT_SAMPLE sandworm_2 = {
	.pos = {12, 55},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'W',
	.move_period = 2500,
	.next_move_time = 2500,
	.layer = 1
};

// [ 아군 하베스터 ]
OBJECT_SAMPLE f_hav = {
	.pos = {14, 1},
	.dest = {12, 1}, // 스파이스 위치로 바꿔야함
	.repr = 'H',
	.move_period = 2000,
	.next_move_time = 2000,
	.layer = 1
};


// [ 적군 하베스터 ]
OBJECT_SAMPLE e_hav = {
	.pos = {3, 58},
	.dest = {5, 58}, // 스파이스 위치로 바꿔야함
	.repr = 'H',
	.move_period = 2000,
	.next_move_time = 2000,
	.layer = 1
};

// ==== [ 함수로 유닛 출력 ] ==== //

// [ 아군 하베스터 출력 함수 ]
void p_f_hav(OBJECT_SAMPLE f_hav, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	map[1][f_hav.pos.row][f_hav.pos.column] = f_hav.repr;
}
// [ 적군 하베스터 출력 함수 ]
void p_e_hav(OBJECT_SAMPLE e_hav, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	map[1][e_hav.pos.row][e_hav.pos.column] = e_hav.repr;
}

// [ 샌드웜 1 출력 함수 ]
void p_sandworm_1(OBJECT_SAMPLE sandworm_1, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	map[1][sandworm_1.pos.row][sandworm_1.pos.column] = sandworm_1.repr;
}

// [샌드웜 2 출력 함수 ]
void p_sandworm_2(OBJECT_SAMPLE sandworm_2, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	map[1][sandworm_2.pos.row][sandworm_2.pos.column] = sandworm_2.repr;
}

// [ 모든 유닛 출력 함수 ]
void p_unit(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	p_f_hav(f_hav, map);
	p_e_hav(e_hav, map);
	p_sandworm_1(sandworm_1, map);
	p_sandworm_2(sandworm_2, map);
}

// =================================== [ DISPLAY ] ======================================= //

void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor,
	char state_map[STATE_HEIGHT][STATE_WIDTH])
{
	display_resource(resource);
	display_map(map);
	display_cursor(cursor);
	// NEW FUNC
	p_building(map);
	p_unit(map);
	display_state_map(state_map);
	// display_system_message()
	// display_object_info()
	// display_commands()
	// ...
}

void display_resource(RESOURCE resource) {
	set_color(COLOR_DEFAULT);
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
				
				// === [ 건물 ] === //
				
				// [ 아군 본진 ]
				if (backbuf[i][j] == 'B' && (i == 16 || i == 15) && (j == 1 || j == 2)) {
					// < 파란색 배경 >
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT + 16);
					colorbuf[i][j] = COLOR_DEFAULT + 16;
				}

				// [ 적군 본진 ] 
				else if (backbuf[i][j] == 'B' && (i == 1 || i == 2) && (j == 57 || j == 58)) {
					// < 빨간색 배경 >
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT + 64);
					colorbuf[i][j] = COLOR_DEFAULT + 64;
				}

				// [ 아군 적군 장판 ]
				else if (backbuf[i][j] == 'P') {
					// < 검은색 배경 >
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_BLACK);
					colorbuf[i][j] = COLOR_BLACK;
				}

				// [ 스파이스 ]
				else if (backbuf[i][j] == '5') {
					// < 주황색 배경 >    아직 색 못 찾음
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT + 48);
					colorbuf[i][j] = COLOR_DEFAULT + 48;
				}

				// [ 바위 ]
				else if (backbuf[i][j] == 'R') {
					// < 회색 배경 >      아직 색 못 찾음
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT);
					colorbuf[i][j] = COLOR_DEFAULT;
				}

				// ==== [ 유닛 ] ====

				// < 아군 하베스터 >
				else if (backbuf[i][j] == 'H' && i == 14 && j == 1) {
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT + 16);
					colorbuf[i][j] = COLOR_DEFAULT + 16;
				}

				// < 적군 하베스터 >
				else if (backbuf[i][j] == 'H' && i == 3 && j == 58) {
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT + 64);
					colorbuf[i][j] = COLOR_DEFAULT + 64;
				}

				// < 샌드웜 1 >
				else if (backbuf[i][j] == 'W' && i == 2 && j == 4) {
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT + 200); // 색은 아직 미지정
					colorbuf[i][j] = COLOR_DEFAULT + 200;
				}

				// < 샌드웜 2 >
				else if (backbuf[i][j] == 'W' && i == 12 && j == 55) {
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT + 200); // 색은 아직 미지정
					colorbuf[i][j] = COLOR_DEFAULT + 200;
				}

				// < 기타 >
				else {
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_RESOURCE);
					colorbuf[i][j] = COLOR_RESOURCE;
				}
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
	printc(padd(map_pos, prev), ch, COLOR_BLACK);

	// [ 커서가 지나간 자리의 색이 지워지지 않게 변경 ]
	int pre_color = colorbuf[prev.row][prev.column]; // 전 위치의 색깔을 기억
	printc(padd(map_pos, prev), ch, pre_color); // 지나간 자리를 다시 원래 색으로 변경

	ch = frontbuf[curr.row][curr.column];
	printc(padd(map_pos, curr), ch, COLOR_BLACK);
}	


// [ 상태 & 명령창 기본 틀 함수 ] 
void state_project(char src[STATE_HEIGHT][STATE_WIDTH], char dest[STATE_HEIGHT][STATE_WIDTH]) {
	for (int i = 0; i < STATE_HEIGHT; i++) {
		for (int j = 0; j < STATE_WIDTH; j++) {
			if (src[i][j] >= 0) {
				dest[i][j] = src[i][j];
			}
		}
	}
}


// [ 상태 & 명령창 그리기 함수 ]
void display_state_map(char state_map[STATE_HEIGHT][STATE_WIDTH]) {
	state_project(state_map, state_backbuf);

	for (int i = 0; i < STATE_HEIGHT; i++) {
		for (int j = 0; j < STATE_WIDTH; j++) {
			if (state_frontbuf[i][j] != state_backbuf[i][j]) {
				POSITION pos = { i, j };
				if (state_backbuf[i][j] == '#') {
					printc(padd(state_pos, pos), state_backbuf[i][j], COLOR_RESOURCE);
				}
			}
			state_frontbuf[i][j] = state_backbuf[i][j];
		}
	}
}


// [ 스페이스바를 눌렀을 때 ]
void state_spacebar(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], 
	char state_map[STATE_HEIGHT][STATE_WIDTH], CURSOR cursor) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;
	POSITION pos_state = { 0, 1 };

	// [ 상태 & 명령창 초기화 ]
	for (int i = 1; i < STATE_HEIGHT - 1; i++) {
		for (int j = 1; j < STATE_WIDTH - 1; j++) {
			POSITION pos = { i, j }; 
			prints(padd(state_pos, pos), " ");
		}
	}
	
	// [ 상태 & 명령 출력 ]
	// < 빈 지형 >
	if (backbuf[curr.row][curr.column] == ' ') {
		char state_message[100] = "지형 : 사막 지형";
		prints(padd(state_mes, pos_state), state_message);
	}
	// < 아군 본진 >
	else if (backbuf[curr.row][curr.column] == 'B' && \
		(curr.row == 16 || curr.row == 15) && (curr.column == 1 || curr.column == 2)) {
		char state_message[100] = "건물 : 아군 본진";
		prints(padd(state_mes, pos_state), state_message);
	}


	
}

// [ ESC 키를 눌렀을 때 ]
void state_esc(char state_map[STATE_HEIGHT][STATE_WIDTH]) { 
	
}