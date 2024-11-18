/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#include "display.h"
#include "io.h"

// [ 출력할 내용들의 좌상단(topleft) 좌표 ]
const POSITION resource_pos = { 0, 0 };
const POSITION map_pos = { 1, 0 };

const POSITION state_pos = { 1, 63 };
const POSITION state_mes_pos = { 2, 63 };
const POSITION state_mes2_pos = { 3, 63 };
const POSITION state_mes3_pos = { 4, 63 };
const POSITION state_mes4_pos = { 5, 63 };
const POSITION state_mes5_pos = { 6, 63 };
const POSITION state_mes6_pos = { 7, 63 };
const POSITION state_mes7_pos = { 8, 63 };
const POSITION state_mes8_pos = { 9, 63 };
const POSITION state_mes9_pos = { 10, 63 };
const POSITION state_mes10_pos = { 11, 63 };
const POSITION state_mes11_pos = { 12, 63 };
const POSITION state_mes12_pos = { 13, 63 };
const POSITION state_mes13_pos = { 14, 63 };

const POSITION sysmes_pos = { 20, 0 };
const POSITION system_mes_pos[7] = { {27, 0},{26, 0},{25, 0},{24, 0},{23, 0},{22, 0}, {21, 0} };

const POSITION order_pos = { 20, 63 };
const POSITION order_mes_pos[4] = { {21, 63}, {22, 63}, {23, 63}, {24, 63} };

// [ buf ]
char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

int colorbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
 
char state_backbuf[STATE_HEIGHT][STATE_WIDTH] = { 0 };
char state_frontbuf[STATE_HEIGHT][STATE_WIDTH] = { 0 };

char sysmes_backbuf[SYSMES_HEIGHT][SYSMES_WIDTH] = { 0 };
char sysmes_frontbuf[SYSMES_HEIGHT][SYSMES_WIDTH] = { 0 };

char order_backbuf[ORDER_HEIGHT][ORDER_WIDTH] = { 0 };
char order_frontbuf[ORDER_HEIGHT][ORDER_WIDTH] = { 0 };

// [ 함수 정의 ]
void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor, int check_friend[MAP_HEIGHT][MAP_WIDTH], bool big_cursor, 
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void set_cursor_color(POSITION pos, char ch);
void highlight_P_area(POSITION pos, int check_friend[MAP_HEIGHT][MAP_WIDTH], bool big_cursor, 
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void clear_cursor_area(POSITION pos, int check_friend[MAP_HEIGHT][MAP_WIDTH], bool big_cursor);
void reset_to_original_color(POSITION pos);

void display_state_map(char state_map[STATE_HEIGHT][STATE_WIDTH]);
void state_project(char src[STATE_HEIGHT][STATE_WIDTH], char dest[STATE_HEIGHT][STATE_WIDTH]);

void sysmes_project(char src[SYSMES_HEIGHT][SYSMES_WIDTH], char dest[SYSMES_HEIGHT][SYSMES_WIDTH]);
void display_sysmes_map(char sysmes_map[SYSMES_HEIGHT][SYSMES_WIDTH]);
void clear_sysmes_frame();

void order_project(char src[ORDER_HEIGHT][ORDER_WIDTH], char dest[ORDER_HEIGHT][ORDER_WIDTH]);
void display_order_map(char order_map[ORDER_HEIGHT][ORDER_WIDTH]);

void reset_order_messages();
void clear_window(char buffer[][STATE_WIDTH], POSITION base_pos, int height, int width);


// [ 변수 정의 ]
char save_name_for_order[2];
char* order_message[4];
char* save_system_message[7] = { NULL };
char* send_system_message[1];

const int pRow[] = { -1, -1, 0, 1, 1, 1, 0, -1 };
const int pCol[] = { 0, -1, -1, -1, 0, 1, 1, 1 };
const int bigrow[] = {1, 1, 0}; 
const int bigcol[] = {0, 1, 1};

int base_barr_check[MAP_HEIGHT][MAP_WIDTH] = { 0 }; // 본진 - 1, 병영 - 2 구분
int shle_sold_check[MAP_HEIGHT][MAP_WIDTH] = { 0 }; // 은신처 - 1, 보병 - 2 구분
int frem_fight_fact_check[MAP_HEIGHT][MAP_WIDTH] = { 0 }; // 공장 - 1, 프레멘 - 2, 투사 - 3 구분

bool return_cursor = false;


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
	.pos1 = {12, 14},
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

// [ 아군 본진 출력 함수 ]
void p_f_base(OBJECT_BUILDING fb, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]) {
	POSITION fb_num[] = { fb.pos1, fb.pos2, fb.pos3, fb.pos4 };
	for (int i = 0; i < 4; i++) {
		map[0][fb_num[i].row][fb_num[i].column] = fb.repr;
		check_friend[fb_num[i].row][fb_num[i].column] = 1;
		base_barr_check[fb_num[i].row][fb_num[i].column] = 1;
	}
}

// [ 적군 본진 출력 함수 ]
void p_e_base(OBJECT_BUILDING eb, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]) {
	POSITION eb_num[] = { eb.pos1, eb.pos2, eb.pos3, eb.pos4 };
	for (int i = 0; i < 4; i++) {
		map[0][eb_num[i].row][eb_num[i].column] = eb.repr;
		check_friend[eb_num[i].row][eb_num[i].column] = 2;
		base_barr_check[eb_num[i].row][eb_num[i].column] = 1;
	}
}

// [ 아군 장판 출력 함수 ]
void p_f_plate(OBJECT_BUILDING fp, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]) {
	POSITION fp_num[] = { fp.pos1, fp.pos2, fp.pos3, fp.pos4 };
	for (int i = 0; i < 4; i++) {
		map[0][fp_num[i].row][fp_num[i].column] = fp.repr;
		check_friend[fp_num[i].row][fp_num[i].column] = 1;
	}
}

// [ 적군 장판 출력 함수 ] 
void p_e_plate(OBJECT_BUILDING ep, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]) {
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
void p_building(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]) {
	p_f_base(f_base, map, check_friend); // 아군 본진
	p_e_base(e_base, map, check_friend); // 적군 본진
	p_f_plate(f_plate, map, check_friend); // 아군 장판
	p_e_plate(e_plate, map, check_friend); // 적군 장판
	p_start_spice(start_spice, map); // 초기 스파이스 매장지 아군 1 적군 1
	p_rock_1(rock_1, map); // 바위 1 
	p_rock_2(rock_2, map); // 바위 2
	p_rock_3(rock_3, map); // 바위 3
	p_rock_4(rock_4, map); // 바위 4
}


// =================================== [ 유닛 ] ======================================= //
// [ 하베스터 ]


// [ 보병 ]


// =================================== [ DISPLAY ] ======================================= //

// [ Display 메인 함수 ]
void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor,
	char state_map[STATE_HEIGHT][STATE_WIDTH],
	char sysmes_map[SYSMES_HEIGHT][SYSMES_WIDTH],
	char order_map[ORDER_HEIGHT][ORDER_WIDTH],
	int check_friend[MAP_HEIGHT][MAP_WIDTH],
	bool big_cursor)
{
	display_resource(resource);
	display_map(map, check_friend);
	display_cursor(cursor, check_friend, big_cursor, map);
	display_state_map(state_map);
	display_sysmes_map(sysmes_map);
	display_order_map(order_map);
}

// [ 화면에 자원, 인구수 출력하는 함수 ]
void display_resource(RESOURCE resource) {
	set_color(COLOR_DEFAULT);
	gotoxy(resource_pos);
	printf("spice = %d/%d, population=%d/%d\n",
		resource.spice, resource.spice_max,
		resource.population, resource.population_max
	);
}

// [ Display 기본 틀 함수 ]
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

// [ 화면에 건물 및 유닛을 보여주는 함수 ]
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]) {
	project(map, backbuf);

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (frontbuf[i][j] == backbuf[i][j]) continue;

			POSITION pos = { i, j };
			char cell = backbuf[i][j];
			int color = COLOR_RESOURCE;

			// === [ 건물 및 유닛 색상 설정 ] ===
			switch (cell) {
			// [ 본진 & 병영 ]
			case 'B':
				// [ 본진 ]
				if (base_barr_check[i][j] == 1) {
					if (check_friend[i][j] == 1) {
						color = COLOR_DEFAULT + 16;  // 아군 본진 (파란색 배경) 
					}
					else if (check_friend[i][j] == 2) {
						color = COLOR_DEFAULT + 64;  // 적군 본진 (빨간색 배경)
					}
				}
				// [ 병영 ]
				else if (base_barr_check[i][j] == 2) {
					if (check_friend[i][j] == 1) {
						color = 181;
					}
				}
				break;

			// [ 장판 ]
			case 'P':  
				color = COLOR_BLACK;  
				break;

			// [ 스파이스 ]
			case '1': case '2': case '3': case '4': case '5':
			case '6': case '7': case '8': case '9': 
				color = COLOR_DEFAULT + 48;  // 주황색 배경
				break;

			// [ 바위 ]
			case 'R': 
				color = COLOR_DEFAULT;
				break;
			
			// [ 숙소 ]
			case 'D':
				color = 180; // 색 아직 미지정 
				break;

			// [ 창고 ]
			case 'G':
				color = 180; // 색 아직 미지정 
				break;

			// [ 은신처 & 보병 ]
			case 'S':
				// [ 은신처 ]
				if (shle_sold_check[i][j] == 1) {
					color = 180; // 색 아직 미지정 
					
				}

				// [ 보병 ]
				else if (shle_sold_check[i][j] == 2) {
					color = 150; // 색 아직 미지정 

				}
				break;

			// [ 투기장 ]
			case 'A':

				break;
			
			// [ 공장 ]
			case 'F':

				break;


			/* ===== [ 유닛 ] ===== */

			// [ 하베스터 ]
			case 'H':  
				if (check_friend[i][j] == 1) {
					color = COLOR_DEFAULT + 16;  // 아군 하베스터 (파란색)
				}
				else if (check_friend[i][j] == 2) {
					color = COLOR_DEFAULT + 64;  // 적군 하베스터 (빨간색)
				}
				break;

			// [ 샌드웜 ]
			case 'W': 
				color = COLOR_DEFAULT + 96;
				break;
			
			// [ 사막 독수리 ]
			case 'E': 
				color = COLOR_DEFAULT + 17;
				break;

			default:
				break;  // 기타 - COLOR_RESOURCE로 처리됨
			}

			printc(padd(map_pos, pos), cell, color);
			colorbuf[i][j] = color;
			frontbuf[i][j] = cell;
		}
	}
}


/* ======================================= [ 커서 범위 함수 ] ======================================= */

// [ 건물, 장판 커서 색깔 변경 함수 ]
void set_cursor_color(POSITION pos, char ch) {
	printc(padd(map_pos, pos), ch, B_CURSOR_COLOR);
}

// [ 커서 잔상색깔 해결 함수 ]
void reset_to_original_color(POSITION pos) {
	int color = colorbuf[pos.row][pos.column];
	char ch = frontbuf[pos.row][pos.column];
	printc(padd(map_pos, pos), ch, color);
}

// [ 커서 잔상 색깔 해결 함수 ]
void clear_cursor_area(POSITION pos, int check_friend[MAP_HEIGHT][MAP_WIDTH], bool big_cursor) {

	// [ 장판일 때 2 X 2 크기 ]
	if (frontbuf[pos.row][pos.column] == 'P' && check_friend[pos.row][pos.column] == 1) {
		
		// [ 현재 위치 ]
		reset_to_original_color(pos);  
		
		// [ 8 방향 검사 ]
		for (int i = 0; i < 8; ++i) {  
			int newRow = pos.row + pRow[i];
			int newCol = pos.column + pCol[i];
			if (frontbuf[newRow][newCol] == 'P' && check_friend[pos.row][pos.column] == 1) {
				POSITION newPos = { newRow, newCol };
				reset_to_original_color(newPos);
			}
		}
	}
	// [ 2 X 2 커서 ]
	else if (big_cursor == true) {
		
		// [ 현재 위치 ]
		reset_to_original_color(pos);  

		// [ 2 X 2 검사 ]
		for (int i = 0; i < 3; ++i) {  
			int newRow = pos.row + bigrow[i];
			int newCol = pos.column + bigcol[i];
			
			POSITION newPos = { newRow, newCol };
			reset_to_original_color(newPos);
			
		}
	}

	// [ 1 X 1 커서 ]
	else {
		reset_to_original_color(pos);  
		
	}
}

// [ 커서 색상 변경 및 출력하는 함수 ]
void highlight_P_area(POSITION pos, int check_friend[MAP_HEIGHT][MAP_WIDTH], bool big_cursor,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {

	// [ 장판일 때 2 X 2 크기 ]
	if (frontbuf[pos.row][pos.column] == 'P' && check_friend[pos.row][pos.column] == 1) {
		set_cursor_color(pos, frontbuf[pos.row][pos.column]);  // 현재 위치
		for (int i = 0; i < 8; ++i) {  // 8방향 검사
			int newRow = pos.row + pRow[i];
			int newCol = pos.column + pCol[i];
			if (frontbuf[newRow][newCol] == 'P' && check_friend[pos.row][pos.column] == 1) {
				POSITION newPos = { newRow, newCol };
				set_cursor_color(newPos, frontbuf[newRow][newCol]);
			}
		}
	}
	// [ 건물이 2 X 2일 때 커서 2 X 2 크기 ]
	else if (big_cursor == true) {

		// [ 현재 위치 ]
		// [ 지을 수 없는 곳에 닿으면 색 지우기 ]
		if (map[0][pos.row][pos.column] == ' ' &&  map[1][pos.row][pos.column] == -1 && \
			map[2][pos.row][pos.column] == -1) {
			set_cursor_color(pos, frontbuf[pos.row][pos.column]);
		}
		
		// [ 2 X 2 검사 ]
		for (int i = 0; i < 3; ++i) {  
			int newRow = pos.row + bigrow[i];
			int newCol = pos.column + bigcol[i];
			// [ 지을 수 없는 곳에 닿으면 색 지우기 ]
			if (newRow != MAP_HEIGHT - 1 && newCol != MAP_WIDTH - 1 && map[0][newRow][newCol] == ' ' && \
				map[1][newRow][newCol] == -1 && map[2][newRow][newCol] == -1) {
				POSITION newPos = { newRow, newCol };
				set_cursor_color(newPos, frontbuf[newRow][newCol]);
			}
		}
	}

	// [ 1 X 1 커서 ]
	else {
		// [ 1 X 1 커서 출력 ]
		printc(padd(map_pos, pos), frontbuf[pos.row][pos.column], COLOR_BLACK);
		
		// [ 생산자원이 없어서 건물을 설치를 못할 때의 예외처리 2 X 2 잔상 지우기 ]
		if (return_cursor) {
			for (int i = 0; i < 3; ++i) {
				int newRow = pos.row + bigrow[i];
				int newCol = pos.column + bigcol[i];

				// [ 2 X 2 잔상이 남아있다면 원래 색상으로 되돌리기 ]
				if (newRow < MAP_HEIGHT && newCol < MAP_WIDTH) {
					POSITION newPos = { newRow, newCol };
					reset_to_original_color(newPos);
				}
			}
		}
	}
}

// [ 메인 커서 출력 함수 ]
void display_cursor(CURSOR cursor, int check_friend[MAP_HEIGHT][MAP_WIDTH], bool big_cursor,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;
	
	// [ 커서 원래 색으로 되돌리기 & 장판을 원래 색으로 되돌리기 ] 
	clear_cursor_area(prev, check_friend, big_cursor);

	// [ 커서 출력 & 장판 색 바꾸기 ]
	highlight_P_area(curr, check_friend, big_cursor, map);
}	


/* ======================================= [ 기본 틀 출력 함수 ] ======================================= */

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


/* ======================================= [ 시스템 메시지 함수 & 초기화 함수 ] ======================================= */

// [ 명령 메시지 초기화 및 명령 변수 초기화 ]
void reset_order_messages() {
	for (int i = 0; i < 4; i++) order_message[i] = 0;
}

// [ 상태, 명령창 초기화 함수 ]
void clear_window(char buffer[][STATE_WIDTH], POSITION base_pos, int height, int width) {
	for (int i = 1; i < height - 1; i++) {
		for (int j = 1; j < width - 1; j++) {
			POSITION pos = { i, j };
			printc(padd(base_pos, pos), buffer[i][j], COLOR_BLACK);
		}
	}
}

// // [ 시스템 메시지 초기화 함수 ]
void clear_sysmes_frame() {
	for (int i = 1; i < SYSMES_HEIGHT - 1; i++) {
		for (int j = 1; j < SYSMES_WIDTH - 1; j++) {
			POSITION pos = { i, j };
			printc(padd(sysmes_pos, pos), sysmes_backbuf[i][j], COLOR_BLACK);
		}
	}
}

// [ 시스템 메시지 출력 ]
void p_system_message(char* new_message) {
	// [ 메시지를 위로 한 칸씩 밀기 ]
	for (int i = 6; i > 0; i--) {
		save_system_message[i] = save_system_message[i - 1];
	}
	save_system_message[0] = new_message;

	// [ 시스템 메시지 초기화 후 메시지 출력 ]
	clear_sysmes_frame();
	POSITION pos_system = { 0, 1 };
	for (int i = 0; i < 7; i++) {
		if (save_system_message[i] != NULL) {
			prints(padd(system_mes_pos[i], pos_system), save_system_message[i]);
		}
	}
}


/* ======================================= [ 각종 키 명령어 함수 ] ======================================= */

// [ 스페이스 바 키를 눌렀을 때 ]
void state_spacebar(RESOURCE* resource, CURSOR cursor, OBJECT_SAMPLE* f_hav_obj, OBJECT_SAMPLE* e_hav_obj, 
	int check_friend[MAP_HEIGHT][MAP_WIDTH], bool p_key_press, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], bool d_key_press, bool g_key_press,
	bool b_b_key_press, bool s_key_press) {

	// [ 변수 지정 ]
	POSITION curr = cursor.current;
	POSITION pos_state = { 0, 1 };
	POSITION pos_order = { 0, 1 };
	char cell = backbuf[curr.row][curr.column];

	// [ 초기화 ]
	reset_order_messages();
	clear_window(state_backbuf, state_pos, STATE_HEIGHT, STATE_WIDTH);
	clear_window(order_backbuf, order_pos, ORDER_HEIGHT, ORDER_WIDTH);
	return_cursor = false;

	
	switch (cell) {
	case ' ':
		prints(padd(state_mes_pos, pos_state), "(빈)지형 : 사막 지형");
		break;

	// [ 본진 & 병영 ]
	case 'B':
		// [ 본진 ]
		if (base_barr_check[curr.row][curr.column] == 1) {
			if (check_friend[curr.row][curr.column] == 1) {
				save_name_for_order[0] = 'B';
				save_name_for_order[1] = 'F';
				prints(padd(state_mes_pos, pos_state), "건물 : 아군 본진");
				prints(padd(state_mes2_pos, pos_state), "하베스터를 생산할 수 있다.");
				prints(padd(order_mes_pos[0], pos_order), "하베스터 생산 (H / ESC)");
				prints(padd(order_mes_pos[1], pos_order), "생산비용 : 5, 필요 인구수 : 5");
			}
			else if (check_friend[curr.row][curr.column] == 2) {
				save_name_for_order[0] = 'B';
				save_name_for_order[1] = 'E';
				prints(padd(state_mes_pos, pos_state), "건물 : 적군 본진");
			}
		}
		// [ 병영 ]
		else if (base_barr_check[curr.row][curr.column]	 == 2) {
			prints(padd(state_mes_pos, pos_state), "건물 : 병영");
			prints(padd(state_mes2_pos, pos_state), "보병을 생산할 수 있다.");
			order_message[0] = "보병 생산(S / ESC)";
			prints(padd(order_mes_pos[0], pos_order), order_message[0]);
		}
		break;

	// [ 장판 ] 
	case 'P':
		if (check_friend[curr.row][curr.column] == 1) {
			prints(padd(state_mes_pos, pos_state), "건물 : 아군 장판");
			prints(padd(state_mes2_pos, pos_state), "숙소, 창고, 병영, 은신처를 설치할 수 있다.");
		}
		else if (check_friend[curr.row][curr.column] == 2) {
			prints(padd(state_mes_pos, pos_state), "건물 : 적군 장판");
		}
		break;

	// [ 스파이스 ]
	case '1': case '2': case '3': case '4': case '5':
	case '6': case '7': case '8': case '9':
		prints(padd(state_mes_pos, pos_state), "건물 : 스파이스");
		break;

	// [ 바위 ]
	case 'R':
		prints(padd(state_mes_pos, pos_state), "건물 : 바위");
		break;

	// [ 숙소 ]
	case 'D':
		prints(padd(state_mes_pos, pos_state), "건물 : 숙소");
		prints(padd(state_mes2_pos, pos_state), "인구 최대치가 10 증가한다.");
		break;

	// [ 창고 ]
	case 'G':
		prints(padd(state_mes_pos, pos_state), "건물 : 창고");
		prints(padd(state_mes2_pos, pos_state), "스파이스 보관 최대치가 10 증가한다.");
		break;

	// [ 은신처 & 보병 ]
	case 'S':
		// [ 은신처 ]
		if (shle_sold_check[curr.row][curr.column] == 1) {
			prints(padd(state_mes_pos, pos_state), "건물 : 은신처");
			prints(padd(state_mes2_pos, pos_state), "프레멘을 생산할 수 있다.");
			order_message[0] = "프레멘 생산(F / ESC)";
			prints(padd(order_mes_pos[0], pos_order), order_message[0]);
		}

		// [ 보병 ]
		else if (shle_sold_check[curr.row][curr.column] == 2) {
			prints(padd(state_mes_pos, pos_state), "유닛 : 보병");
			order_message[0] = "M : 이동, P : 순찰";
			prints(padd(order_mes_pos[0], pos_order), order_message[0]);
		}

		break;

	// [ 투기장 ]
	case 'A':
		prints(padd(state_mes_pos, pos_state), "건물 : 투기장");
		prints(padd(state_mes2_pos, pos_state), "투사를 생산할 수 있다.");
		order_message[0] = "적군만 투사를 생산할 수 있습니다.";
		prints(padd(order_mes_pos[0], pos_order), order_message[0]);
		break;

	// [ 공장 & 프레멘 & 투사 ]
	case 'F':
		// [ 공장 ]
		if (frem_fight_fact_check[curr.row][curr.column] == 1) {
			prints(padd(state_mes_pos, pos_state), "건물 : 공장");
			prints(padd(state_mes2_pos, pos_state), "중전차를 생산할 수 있다.");
			order_message[0] = "적군만 중전차를 생산할 수 있습니다.";
			prints(padd(order_mes_pos[0], pos_order), order_message[0]);
		}

		// [ 프레멘 ]
		else if (frem_fight_fact_check[curr.row][curr.column] == 2) {
			prints(padd(state_mes_pos, pos_state), "특수 유닛 : 프레멘");
			order_message[0] = "M : 이동, P : 순찰";
			prints(padd(order_mes_pos[0], pos_order), order_message[0]);
		}

		// [ 투사 ]
		else if (frem_fight_fact_check[curr.row][curr.column] == 3) {
			prints(padd(state_mes_pos, pos_state), "적군 유닛 : 투사");
			order_message[0] = "적군 유닛은 이동, 순찰 명령어를 사용할 수 없습니다.";
			prints(padd(order_mes_pos[0], pos_order), order_message[0]);
		}

		break;


	/* ===== [유닛] ====== */
	// [ 하베스터 ]
	case 'H':
		if (colorbuf[curr.row][curr.column] == COLOR_DEFAULT + 16) {
			char mes_buf[100];
			// [ 상태 메시지 ]
			prints(padd(state_mes_pos, pos_state), "유닛 : 아군 하베스터");
			sprintf_s(mes_buf, sizeof(mes_buf), "체력: %d, 공격력: 0, 이동주기: 2초, 시야: 0", f_hav_obj->hp);
			prints(padd(state_mes2_pos, pos_state), mes_buf);

			// [ 하베스터 초상화 ]
			prints(padd(state_mes4_pos, pos_state), "     -----------------");
			prints(padd(state_mes5_pos, pos_state), "     |   _________   |");
			prints(padd(state_mes6_pos, pos_state), "     |  /[=] | [=]\\  |");
			prints(padd(state_mes7_pos, pos_state), "     |  |[=] | [=]|  |");
			prints(padd(state_mes8_pos, pos_state), "     |  -----------  |");
			prints(padd(state_mes9_pos, pos_state), "     |   O       O   | ");
			prints(padd(state_mes10_pos, pos_state), "     |               |");
			prints(padd(state_mes11_pos, pos_state), "     -----------------");
			// [ 명령 메시지 ]
			prints(padd(order_mes_pos[0], pos_order), "H : 수확, M : 이동");
		}
		else if (colorbuf[curr.row][curr.column] == COLOR_DEFAULT + 64) {
			char mes_buf[100];
			prints(padd(state_mes_pos, pos_state), "유닛 : 적군 하베스터");
			sprintf_s(mes_buf, sizeof(mes_buf), "체력: %d, 공격력: 0, 이동주기: 2초, 시야: 0", e_hav_obj->hp);
			prints(padd(state_mes2_pos, pos_state), mes_buf);
		}
		break;

	// [ 샌드웜 ]
	case 'W':
		prints(padd(state_mes_pos, pos_state), "유닛 : 샌드웜");
		break;

	// [ 중전차 ]
	case 'T':
		prints(padd(state_mes_pos, pos_state), "유닛 : 중전차");
		order_message[0] = "적군 유닛은 이동, 순찰 명령어를 사용할 수 없습니다.";
		prints(padd(order_mes_pos[0], pos_order), order_message[0]);
		break;
	}


	// [ 스페이스바로 장판 설치 ]
	if (p_key_press && resource->spice >= 1) {

		// [ 커서 자리 P ]
		map[0][curr.row][curr.column] = 'P';
		check_friend[curr.row][curr.column] = 1;

		// [ 커서 주변 자리 P ]
		for (int i = 0; i < 3; i++) {
			int b_row = curr.row + bigrow[i];
			int b_col = curr.column + bigcol[i];
			map[0][b_row][b_col] = 'P';
			check_friend[b_row][b_col] = 1;
		}

		// [ 생산 자원 소모 ]
		resource->spice -= 1;
		p_system_message("장판을 건설하였습니다.");

		// [ 초기화 ]
		clear_window(state_backbuf, state_pos, STATE_HEIGHT, STATE_WIDTH);
	}

	// [ 장판을 설치할 수 없을 때 ]
	else if (p_key_press && resource->spice < 1) {
		p_system_message("생산자원이 부족하여 건설할 수 없습니다.");
		return_cursor = true;
	}


	// [ 스페이스바로 숙소 설치 ]
	if (d_key_press && resource->spice >= 2) {

		// [ 커서 자리 P ]
		map[0][curr.row][curr.column] = 'D';
		check_friend[curr.row][curr.column] = 1;

		// [ 커서 주변 자리 P ]
		for (int i = 0; i < 8; ++i) {  // 8방향 검사
			int newRow = curr.row + pRow[i];
			int newCol = curr.column + pCol[i];
			if (frontbuf[newRow][newCol] == 'P' && check_friend[newRow][newCol] == 1) {
				map[0][newRow][newCol] = 'D';
			}
		}

		// [ 생산 자원 소모 ]
		resource->spice -= 2;
		p_system_message("숙소를 건설하였습니다.");

		// [ 초기화 ]
		clear_window(state_backbuf, state_pos, STATE_HEIGHT, STATE_WIDTH);
	}
	
	// [ 숙소를 설치할 수 없을 때 ]
	else if (d_key_press && resource->spice < 2) {
		p_system_message("생산자원이 부족하여 건설할 수 없습니다.");
		return_cursor = true;
	}


	// [ 스페이스바로 창고 설치 ]
	if (g_key_press && resource->spice >= 4) {
		// [ 커서 자리 P ]
		map[0][curr.row][curr.column] = 'G';
		check_friend[curr.row][curr.column] = 1;

		// [ 커서 주변 자리 P ]
		for (int i = 0; i < 8; ++i) {  // 8방향 검사
			int newRow = curr.row + pRow[i];
			int newCol = curr.column + pCol[i];
			if (frontbuf[newRow][newCol] == 'P' && check_friend[newRow][newCol] == 1) {
				map[0][newRow][newCol] = 'G';
			}
		}

		// [ 생산 자원 소모 ]
		resource->spice -= 2;
		p_system_message("숙소를 건설하였습니다.");

		// [ 초기화 ]
		clear_window(state_backbuf, state_pos, STATE_HEIGHT, STATE_WIDTH);
	}
	
	// [ 창고를 설치할 수 없을 때 ]
	else if (d_key_press && resource->spice < 4) {
		p_system_message("생산자원이 부족하여 건설할 수 없습니다.");
		return_cursor = true;
	}


	// [ 스페이스바로 병영 설치 ]
	if (b_b_key_press && resource->spice >= 4) {
		map[0][curr.row][curr.column] = 'B';
		check_friend[curr.row][curr.column] = 1;
		base_barr_check[curr.row][curr.column] = 2;

		// [ 커서 주변 자리 P ]
		for (int i = 0; i < 8; ++i) {  // 8방향 검사
			int newRow = curr.row + pRow[i];
			int newCol = curr.column + pCol[i];
			if (frontbuf[newRow][newCol] == 'P' && check_friend[newRow][newCol] == 1) {
				map[0][newRow][newCol] = 'B';
				base_barr_check[newRow][newCol] = 2;
			}
		}

		// [ 생산 자원 소모 ]
		resource->spice -= 4;
		p_system_message("병영을 건설하였습니다.");

		// [ 초기화 ]
		clear_window(state_backbuf, state_pos, STATE_HEIGHT, STATE_WIDTH);
	}
	
	// [ 병영을 설치할 수 없을 때 ]
	else if (b_b_key_press && resource->spice < 4) {
		p_system_message("생산자원이 부족하여 건설할 수 없습니다.");
		return_cursor = true;
	}


	// [ 스페이스바로 은신처 설치 ]
	if (s_key_press && resource->spice >= 5) {
		map[0][curr.row][curr.column] = 'S';
		check_friend[curr.row][curr.column] = 1;
		shle_sold_check[curr.row][curr.column] = 1; // 은신처 - 1, 보병 - 2

		// [ 커서 주변 자리 P ]
		for (int i = 0; i < 8; ++i) {  // 8방향 검사
			int newRow = curr.row + pRow[i];
			int newCol = curr.column + pCol[i];
			if (frontbuf[newRow][newCol] == 'P' && check_friend[newRow][newCol] == 1) {
				map[0][newRow][newCol] = 'S';
				shle_sold_check[newRow][newCol] = 1;
			}
		}

		// [ 생산 자원 소모 ]
		resource->spice -= 5;
		p_system_message("은신처를 건설하였습니다.");

		// [ 초기화 ]
		clear_window(state_backbuf, state_pos, STATE_HEIGHT, STATE_WIDTH);
	}
	
	// [ 은신처를 설치할 수 없을 때 ]
	else if (s_key_press && resource->spice < 5) {
		p_system_message("생산자원이 부족하여 건설할 수 없습니다.");
		return_cursor = true;
	}


}

// [ ESC 키를 눌렀을 때 ]
void state_esc(void) {
	// [ 초기화 ]
	reset_order_messages();
	clear_window(state_backbuf, state_pos, STATE_HEIGHT, STATE_WIDTH);
	clear_window(order_backbuf, order_pos, ORDER_HEIGHT, ORDER_WIDTH);
}

// [ 하베스터 생산 H키를 눌렀을 때 ]
void press_h(RESOURCE* resource, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH], CURSOR cursor, OBJECT_SAMPLE* f_hav_obj, int hav_count, OBJECT_SAMPLE* havs[MAX_HAV]) {
	POSITION curr = cursor.current;

	// [ 아군 본진 --> 하베스터 생산 ] [ 하베스터 생산 비용 5, 인구수 5 ]
	if (map[0][curr.row][curr.column] == 'B' && check_friend[curr.row][curr.column] == 1) {

		// [ 하베스터를 생산할 수 있을 때 ]
		if (resource->spice >= 5 && resource->population + 5 <= resource->population_max) {

			// [ 하베스터를 추가할 수 있을 때만 생성 ]
			if (hav_count < MAX_HAV) {
				OBJECT_SAMPLE new_hav = { 
					{ 14, 2 }, { 14, 2 }, 'H', 2000, 2000, 
					COLOR_DEFAULT + 16, 70, 0, 0, 0 
				};

				// [ 하베스터를 배열에 추가 ]
				havs[0] = f_hav_obj;
				havs[hav_count++] = &new_hav;

				// [ 맵에 하베스터 위치 반영 ]
				map[1][new_hav.pos.row][new_hav.pos.column] = 'H';
				check_friend[new_hav.pos.row][new_hav.pos.column] = 1;

				// [ 시스템 메시지 추가 ]
				p_system_message("하베스터가가 생산되었습니다.");

				// [ 자원 감소, 인구수 증가 ]
				resource->spice -= 5;
				resource->population += 5;
			}
			else {
				p_system_message("하베스터 최대 생성 개수를 초과하여 더 이상 생산할 수 없습니다.");
			}

		}
		// [ 하베스터를 생산할 수 없을 때 ]
		else {
			if (resource->spice < 5) {
				// [ 시스템 메시지 추가 ]
				p_system_message("생산 자원이 부족하여 하베스터를 생산할 수 없습니다.");
			}
			else if (resource->population + 5 > resource->population_max) {
				// [ 시스템 메시지 추가 ]
				p_system_message("인구수가 부족하여 하베스터를 생산할 수 없습니다.");
			}
		}
	}
	// [ 아군 하베스터 수확 행동 ]
	else if (map[0][curr.row][curr.column] == '1' || map[0][curr.row][curr.column] == '2' || map[0][curr.row][curr.column] == '3' || \
		map[0][curr.row][curr.column] == '4' || map[0][curr.row][curr.column] == '5' || map[0][curr.row][curr.column] == '6' || \
		map[0][curr.row][curr.column] == '7' || map[0][curr.row][curr.column] == '8' || map[0][curr.row][curr.column] == '9') {
		// [ 하베스터 목적지를 스파이스 위치로 설정 ]
		POSITION new_dest = { curr.row, curr.column };
		
	}
}

// [ B (설치 가능 건물 목록) 키를 눌렀을 때 ]
void press_b(RESOURCE* resource, CURSOR cursor, int check_friend[MAP_HEIGHT][MAP_WIDTH]) {

	// [ 변수 지정 ]
	POSITION curr = cursor.current;
	POSITION pos_order = { 0, 1 };
	char cell = backbuf[curr.row][curr.column];

	// [ 초기화 ]
	reset_order_messages();
	clear_window(order_backbuf, order_pos, ORDER_HEIGHT, ORDER_WIDTH);

	switch (cell) {

	// [ 사막 지형 (빈 지형) ]
	case ' ':
		prints(padd(order_mes_pos[0], pos_order), "장판을 설치하시겠습니까? (P / ESC)");
		break;

	// [ 장판 ]
	case 'P':
		if (check_friend[curr.row][curr.column] == 1) {
			// [ 명령어 출력 ]
			char* messages[] = {
			"숙소를 설치하시겠습니까?   (D / ESC)",
			"창고를 설치하시겠습니까?   (G / ESC)",
			"병영을 설치하시겠습니까?   (B / ESC)",
			"은신처를 설치하시겠습니다? (S / ESC)"
			};

			for (int i = 0; i < 4; i++) {
				order_message[i] = messages[i];
				prints(padd(order_mes_pos[i], pos_order), order_message[i]);
			}
		}
		break;
	}
}

// [ P (장판 설치) 키를 눌렀을 때 ]
void press_p(void) {
	POSITION pos_order = { 0, 1 };
	clear_window(order_backbuf, order_pos, ORDER_HEIGHT, ORDER_WIDTH);
	prints(padd(order_mes_pos[0], pos_order), "스페이스바를 눌러 장판 설치를 완료하세요.");
}

// [ D (숙소 설치) 키를 눌렀을 때 ]
void press_d(void) {
	POSITION pos_order = { 0, 1 };
	clear_window(order_backbuf, order_pos, ORDER_HEIGHT, ORDER_WIDTH);
	prints(padd(order_mes_pos[0], pos_order), "스페이스바를 눌러 숙소 설치를 완료하세요.");
}

// [ G (창고 설치) 키를 눌렀을 때 ]
void press_g(void) {
	POSITION pos_order = { 0, 1 };
	clear_window(order_backbuf, order_pos, ORDER_HEIGHT, ORDER_WIDTH);
	prints(padd(order_mes_pos[0], pos_order), "스페이스바를 눌러 창고 설치를 완료하세요.");
}

// [ B - B (병영 설치) 키를 눌렀을 때 ]
void press_b_b(void) {
	POSITION pos_order = { 0, 1 };
	clear_window(order_backbuf, order_pos, ORDER_HEIGHT, ORDER_WIDTH);
	prints(padd(order_mes_pos[0], pos_order), "스페이스바를 눌러 병영 설치를 완료하세요.");
}

// [ S (은신처 설치) 키를 눌렀을 때 ]
void press_s(void) {
	POSITION pos_order = { 0, 1 };
	clear_window(order_backbuf, order_pos, ORDER_HEIGHT, ORDER_WIDTH);
	prints(padd(order_mes_pos[0], pos_order), "스페이스바를 눌러 은신처 설치를 완료하세요.");
}

// [ S (보병 생산) 키를 눌렀을 때 ]
void press_s_s(CURSOR cursor, RESOURCE* resource, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	POSITION curr = cursor.current;
	if (map[0][curr.row][curr.column] == 'B' && base_barr_check[curr.row][curr.column] == 2 && \
		resource->spice >= 1 && resource->population + 1 <= resource->population_max) {
		
	}
	else if (resource->spice < 1) {
		p_system_message("생산 자원이 부족하여 하베스터를 생산할 수 없습니다.");
	}
	else if (resource->population + 1 > resource->population_max) {
		p_system_message("인구수가 부족하여 하베스터를 생산할 수 없습니다.");
	}
}

// [ F (프레멘 생산) 키를 눌렀을 때 ]
void press_f() {

}

// [ M (이동) 키를 눌렀을 때 ]
void press_m() {

}

// [ P (순찰) 키를 눌렀을 때 ]
void press_p_p() {

}

