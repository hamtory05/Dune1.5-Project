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
const POSITION state_mes_pos = { 2, 63 };
const POSITION state_mes2_pos = { 3, 63 };

const POSITION sysmes_pos = { 20, 0 };
const POSITION system_mes_pos[7] = { {27, 0},{26, 0},{25, 0},{24, 0},{23, 0},{22, 0}, {21, 0} };

const POSITION order_pos = { 20, 63 };
const POSITION order_mes_pos[4] = { {21, 63}, {22, 63}, {23, 63}, {24, 63} };

char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

int colorbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
int check_friend[MAP_HEIGHT][MAP_WIDTH] = { 0 }; // 0 --> 아무것도 아님, 1 --> 아군, 2 --> 적군
 
char state_backbuf[STATE_HEIGHT][STATE_WIDTH] = { 0 };
char state_frontbuf[STATE_HEIGHT][STATE_WIDTH] = { 0 };

char sysmes_backbuf[SYSMES_HEIGHT][SYSMES_WIDTH] = { 0 };
char sysmes_frontbuf[SYSMES_HEIGHT][SYSMES_WIDTH] = { 0 };

char order_backbuf[ORDER_HEIGHT][ORDER_WIDTH] = { 0 };
char order_frontbuf[ORDER_HEIGHT][ORDER_WIDTH] = { 0 };

void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);

void display_state_map(char state_map[STATE_HEIGHT][STATE_WIDTH]);
void state_project(char src[STATE_HEIGHT][STATE_WIDTH], char dest[STATE_HEIGHT][STATE_WIDTH]);

void sysmes_project(char src[SYSMES_HEIGHT][SYSMES_WIDTH], char dest[SYSMES_HEIGHT][SYSMES_WIDTH]);
void display_sysmes_map(char sysmes_map[SYSMES_HEIGHT][SYSMES_WIDTH]);

void order_project(char src[ORDER_HEIGHT][ORDER_WIDTH], char dest[ORDER_HEIGHT][ORDER_WIDTH]);
void display_order_map(char order_map[ORDER_HEIGHT][ORDER_WIDTH]);


char save_name_for_order[2];
char* order_message[4];
char* save_system_message[7] = { NULL };
char* send_system_message[1];

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
		check_friend[fb_num[i].row][fb_num[i].column] = 1;
	}
}

// [ 적군 본진 출력 함수 ]
void p_e_base(OBJECT_BUILDING eb, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	POSITION eb_num[] = { eb.pos1, eb.pos2, eb.pos3, eb.pos4 };
	for (int i = 0; i < 4; i++) {
		map[0][eb_num[i].row][eb_num[i].column] = eb.repr;
		check_friend[eb_num[i].row][eb_num[i].column] = 2;
	}
}

// [ 아군 장판 출력 함수 ]
void p_f_plate(OBJECT_BUILDING fp, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	POSITION fp_num[] = { fp.pos1, fp.pos2, fp.pos3, fp.pos4 };
	for (int i = 0; i < 4; i++) {
		map[0][fp_num[i].row][fp_num[i].column] = fp.repr;
		check_friend[fp_num[i].row][fp_num[i].column] = 1;
	}
}

// [ 적군 장판 출력 함수 ] 
void p_e_plate(OBJECT_BUILDING ep, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	POSITION ep_num[] = { ep.pos1, ep.pos2, ep.pos3, ep.pos4 };
	for (int i = 0; i < 4; i++) {
		map[0][ep_num[i].row][ep_num[i].column] = ep.repr;
		check_friend[ep_num[i].row][ep_num[i].column] = 2;
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


// =================================== [ DISPLAY ] ======================================= //

void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor,
	char state_map[STATE_HEIGHT][STATE_WIDTH],
	char sysmes_map[SYSMES_HEIGHT][SYSMES_WIDTH],
	char order_map[ORDER_HEIGHT][ORDER_WIDTH])
{
	display_resource(resource);
	display_map(map);
	display_cursor(cursor);
	// NEW FUNC
	p_building(map);
	display_state_map(state_map);
	display_sysmes_map(sysmes_map);
	display_order_map(order_map);
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
				if (backbuf[i][j] == 'B' && check_friend[i][j] == 1) {
					// < 파란색 배경 >
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT + 16);
					colorbuf[i][j] = COLOR_DEFAULT + 16;
				}

				// [ 적군 본진 ] 
				else if (backbuf[i][j] == 'B' && check_friend[i][j] == 2) {
					// < 빨간색 배경 >
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT + 64);
					colorbuf[i][j] = COLOR_DEFAULT + 64;
				}

				// [ 아군 장판 ]
				else if (backbuf[i][j] == 'P' && check_friend[i][j] == 1) {
					// < 검은색 배경 >
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_BLACK);
					colorbuf[i][j] = COLOR_BLACK;
				}
				
				// [ 적군 장판 ]
				else if (backbuf[i][j] == 'P' && check_friend[i][j] == 2) {
					// < 검은색 배경 >
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_BLACK);
					colorbuf[i][j] = COLOR_BLACK;
				}

				// [ 스파이스 ]
				else if (backbuf[i][j] == '1' || backbuf[i][j] == '2' || backbuf[i][j] == '3' || backbuf[i][j] == '4' || \
					backbuf[i][j] == '5' || backbuf[i][j] == '6' || backbuf[i][j] == '7' || backbuf[i][j] == '8' || backbuf[i][j] == '9') {
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

				// < 샌드웜 >
				else if (backbuf[i][j] == 'W') {
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT + 96); 
					colorbuf[i][j] = COLOR_DEFAULT + 96;
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


// [ 상태창 기본 틀 함수 ] 
void state_project(char src[STATE_HEIGHT][STATE_WIDTH], char dest[STATE_HEIGHT][STATE_WIDTH]) {
	for (int i = 0; i < STATE_HEIGHT; i++) {
		for (int j = 0; j < STATE_WIDTH; j++) {
			if (src[i][j] >= 0) {
				dest[i][j] = src[i][j];
			}
		}
	}
}

// [ 상태창 그리기 함수 ]
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


// [ 시스템창 기본 틀 함수 ]
void sysmes_project(char src[SYSMES_HEIGHT][SYSMES_WIDTH], char dest[SYSMES_HEIGHT][SYSMES_WIDTH]) {
	for (int i = 0; i < SYSMES_HEIGHT; i++) {
		for (int j = 0; j < SYSMES_WIDTH; j++) {
			if (src[i][j] >= 0) {
				dest[i][j] = src[i][j];
			}
		}
	}
}

// [ 시스템 창 그리기 함수 ]
void display_sysmes_map(char sysmes_map[SYSMES_HEIGHT][SYSMES_WIDTH]) {
	sysmes_project(sysmes_map, sysmes_backbuf);

	for (int i = 0; i < SYSMES_HEIGHT; i++) {
		for (int j = 0; j < SYSMES_WIDTH; j++) {
			if (sysmes_frontbuf[i][j] != sysmes_backbuf[i][j]) {
				POSITION pos = { i, j };
				if (sysmes_backbuf[i][j] == '#') {
					printc(padd(sysmes_pos, pos), sysmes_backbuf[i][j], COLOR_RESOURCE);
				}
			}
			sysmes_frontbuf[i][j] = sysmes_backbuf[i][j];
		}
	}
}


// [ 명령창 기본 틀 함수 ]
void order_project(char src[ORDER_HEIGHT][ORDER_WIDTH], char dest[ORDER_HEIGHT][ORDER_WIDTH]) {
	for (int i = 0; i < ORDER_HEIGHT; i++) {
		for (int j = 0; j < ORDER_WIDTH; j++) {
			if (src[i][j] >= 0) {
				dest[i][j] = src[i][j];
			}
		}
	}
}

// [ 명령창 그리기 함수 ]
void display_order_map(char order_map[ORDER_HEIGHT][ORDER_WIDTH]) {
	order_project(order_map, order_backbuf);

	for (int i = 0; i < ORDER_HEIGHT; i++) {
		for (int j = 0; j < ORDER_WIDTH; j++) {
			if (order_frontbuf[i][j] != order_backbuf[i][j]) {
				POSITION pos = { i, j };
				if (order_backbuf[i][j] == '#') {
					printc(padd(order_pos, pos), order_backbuf[i][j], COLOR_RESOURCE);
				}
			}
			order_frontbuf[i][j] = order_backbuf[i][j];
		}
	}
}



// [ 스페이스바를 눌렀을 때 ]
void state_spacebar(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], 
	char state_map[STATE_HEIGHT][STATE_WIDTH], CURSOR cursor) {
	POSITION curr = cursor.current;
	POSITION pos_state = { 0, 1 }; 
	POSITION pos_order = { 0, 1 };

	// [ 명령창을 위한 변수 초기화 ]
	for (int i = 0; i < 2; i++) {
		save_name_for_order[i] = ' ';
	}

	// [ 상태 & 명령창 초기화 ]
	for (int i = 1; i < STATE_HEIGHT - 1; i++) {
		for (int j = 1; j < STATE_WIDTH - 1; j++) {
			POSITION pos = { i, j }; 
			printc(padd(state_pos, pos), state_backbuf[i][j], COLOR_BLACK);
		}
	}
	for (int i = 1; i < ORDER_HEIGHT - 1; i++) { 
		for (int j = 1; j < ORDER_WIDTH - 1; j++) {
			POSITION pos = { i, j };
			printc(padd(order_pos, pos), order_backbuf[i][j], COLOR_BLACK);
		}
	}

	// [ 명령 메시지 초기화 ]
	for (int i = 0; i < 4; i++) { 
		order_message[i] = 0;
	}
	

	// [ 상태 & 명령 출력 ]
	// < 빈 지형 >
	if (backbuf[curr.row][curr.column] == ' ') {
		char state_message[] = "(빈)지형 : 사막 지형";
		prints(padd(state_mes_pos, pos_state), state_message);
	}
	// < 아군 본진 >
	else if (backbuf[curr.row][curr.column] == 'B' && \
		(curr.row == 16 || curr.row == 15) && (curr.column == 1 || curr.column == 2)) {
		save_name_for_order[0] = 'B';
		save_name_for_order[1] = 'F';
		char state_message[] = "건물 : 아군 본진";
		prints(padd(state_mes_pos, pos_state), state_message);

		char state_message2[] = "하베스터를 생산할 수 있다.";
		prints(padd(state_mes2_pos, pos_state), state_message2);

		order_message[0] = "하베스터 생산 (H / ESC)";
		prints(padd(order_mes_pos[0], pos_order), order_message[0]);
	}
	// < 적군 본진 >
	else if (backbuf[curr.row][curr.column] == 'B' && \
		(curr.row == 1 || curr.row == 2) && (curr.column == 57 || curr.column == 58)) {
		save_name_for_order[0] = 'B';
		save_name_for_order[1] = 'E';
		char state_message[] = "건물 : 적군 본진";
		prints(padd(state_mes_pos, pos_state), state_message);
	}
	// < 아군 장판 >
	else if (backbuf[curr.row][curr.column] == 'P' && check_friend[curr.row][curr.column] == 1) {
		save_name_for_order[0] = 'P';
		save_name_for_order[1] = 'F';
		char state_message[] = "건물 : 아군 장판";
		prints(padd(state_mes_pos, pos_state), state_message);

		char state_message2[] = "숙소, 창고, 병영, 은신처를 설치할 수 있다.";
		prints(padd(state_mes2_pos, pos_state), state_message2);

		order_message[0] = "숙소를 설치하시겠습니까?   (D / ESC)";
		order_message[1] = "창고를 설치하시겠습니까?   (G / ESC)";
		order_message[2] = "병영을 설치하시겠습니까?   (B / ESC)";
		order_message[3] = "은신처를 설치하시겠습니다? (S / ESC)";
		for (int i = 0; i < 4; i++) {
			prints(padd(order_mes_pos[i], pos_order), order_message[i]);
		}
	}
	// < 적군 장판 >
	else if (backbuf[curr.row][curr.column] == 'P' && check_friend[curr.row][curr.column] == 2) {
		save_name_for_order[0] = 'P';
		save_name_for_order[1] = 'E';
		char state_message[] = "건물 : 적군 장판";
		prints(padd(state_mes_pos, pos_state), state_message);
	}
	// < 스파이스 >
	else if (backbuf[curr.row][curr.column] == '1' || backbuf[curr.row][curr.column] == '2' || backbuf[curr.row][curr.column] == '3' || \
		backbuf[curr.row][curr.column] == '4' || backbuf[curr.row][curr.column] == '5' || backbuf[curr.row][curr.column] == '6' || \
		backbuf[curr.row][curr.column] == '7' || backbuf[curr.row][curr.column] == '8' || backbuf[curr.row][curr.column] == '9') {
		char state_message[] = "건물 : 스파이스";
		prints(padd(state_mes_pos, pos_state), state_message);
	}
	// < 바위 >
	else if (backbuf[curr.row][curr.column] == 'R') {
		char state_message[] = "건물 : 바위";
		prints(padd(state_mes_pos, pos_state), state_message);
	}
	// < 아군 하베스터 >
	else if (backbuf[curr.row][curr.column] == 'H' && colorbuf[curr.row][curr.column] == COLOR_DEFAULT + 16) {
		char state_message[] = "유닛 : 아군 하베스터";
		prints(padd(state_mes_pos, pos_state), state_message);
	}
	// < 적군 하베스터 >
	else if (backbuf[curr.row][curr.column] == 'H' && colorbuf[curr.row][curr.column] == COLOR_DEFAULT + 64) {
		char state_message[] = "유닛 : 적군 하베스터";
		prints(padd(state_mes_pos, pos_state), state_message);
	}
	// < 샌드웜 >
	else if (backbuf[curr.row][curr.column] == 'W') {
		char state_message[] = "유닛 : 샌드웜"; 
		prints(padd(state_mes_pos, pos_state), state_message);
	}

}


// [ ESC 키를 눌렀을 때 ]
void state_esc(char state_map[STATE_HEIGHT][STATE_WIDTH], char order_map[ORDER_HEIGHT][ORDER_WIDTH]) {
	// [ 명령창을 위한 변수 초기화 ]
	for (int i = 0; i < 2; i++) {
		save_name_for_order[i] = ' ';
	}

	// [ 상태 & 명령창 초기화 ]
	for (int i = 1; i < STATE_HEIGHT - 1; i++) {
		for (int j = 1; j < STATE_WIDTH - 1; j++) {
			POSITION pos = { i, j };
			printc(padd(state_pos, pos), state_backbuf[i][j], COLOR_BLACK);
		}
	}
	for (int i = 1; i < ORDER_HEIGHT - 1; i++) {
		for (int j = 1; j < ORDER_WIDTH - 1; j++) {
			POSITION pos = { i, j };
			printc(padd(order_pos, pos), order_backbuf[i][j], COLOR_BLACK);
		}
	}

	// [ 명령 메시지 초기화 ]
	for (int i = 0; i < 4; i++) {
		order_message[i] = 0;
	}
}


// [ 하베스터 생산 H키를 눌렀을 때 ]
void press_h(RESOURCE resource, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char sysmes_map[SYSMES_HEIGHT][SYSMES_WIDTH]) {
	// [ 아군 본진 --> 하베스터 생산 ] [ 하베스터 생산 비용 5, 인구수 5 ]
	if (save_name_for_order[0] == 'B') {
		if (save_name_for_order[1] == 'F') {
			// [ 하베스터를 생산할 수 있을 때 ]
			if (resource.spice >= 5 && resource.population + 5 <= resource.population_max) {
				// [ 하베스터 생산 ] 
				map[1][15][3] = 'H';
				send_system_message[0] = "하베스터가 1기가 생산되었습니다.";
				p_system_message(send_system_message[0], sysmes_map);
				
				// 자원감소, 인구수 증가
				resource.spice -= 5;
				resource.population += 5;
			}
			// [ 하베스터를 생산할 수 없을 때 ]
			else {
				if (resource.spice < 5) {
					send_system_message[0] = "생산 자원이 부족하여 하베스터를 생산할 수 없습니다.";
					p_system_message(send_system_message[0], sysmes_map);
				}
				else if (resource.population  + 5 > resource.population_max) {
					send_system_message[0] = "인구수가 부족하여 하베스터를 생산할 수 없습니다.";
					p_system_message(send_system_message[0], sysmes_map);
				}
			}
		}
	}
}


// [ 시스템 메시지 출력 ]
void p_system_message(char str[], char sysmes_map[SYSMES_HEIGHT][SYSMES_WIDTH]) {
	POSITION pos_system = { 0, 1 };

	// 시스템 메시지 틀 초기화
	for (int i = 1; i < SYSMES_HEIGHT - 1; i++) {
		for (int j = 1; j < SYSMES_WIDTH - 1; j++) {
			POSITION pos = { i, j };
			printc(padd(sysmes_pos, pos), sysmes_backbuf[i][j], COLOR_BLACK);

		}
	}

	// 시스템 메시지를 위로 올리기
	if (save_system_message[0] != NULL) {
		if (save_system_message[1] != NULL) {
			if (save_system_message[2] != NULL) {
				if (save_system_message[3] != NULL) {
					if (save_system_message[4] != NULL) {
						if (save_system_message[5] != NULL) {
							save_system_message[6] = save_system_message[5];
						}
						save_system_message[5] = save_system_message[4];
					}
					save_system_message[4] = save_system_message[3];
				}
				save_system_message[3] = save_system_message[2];
			}
			save_system_message[2] = save_system_message[1];
		}
		save_system_message[1] = save_system_message[0];
	}

	save_system_message[0] = str;
	

	// 시스템 메시지 출력
	for (int i = 0; i < 7; i++) {
		if (save_system_message[i] != NULL) {
			prints(padd(system_mes_pos[i], pos_system), save_system_message[i]);
		}
	}

}