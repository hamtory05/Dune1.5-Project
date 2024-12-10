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

int colorbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 }; // 건물, 유닛 색 확인
 
char state_backbuf[STATE_HEIGHT][STATE_WIDTH] = { 0 };
char state_frontbuf[STATE_HEIGHT][STATE_WIDTH] = { 0 };

char sysmes_backbuf[SYSMES_HEIGHT][SYSMES_WIDTH] = { 0 };
char sysmes_frontbuf[SYSMES_HEIGHT][SYSMES_WIDTH] = { 0 };

char order_backbuf[ORDER_HEIGHT][ORDER_WIDTH] = { 0 };
char order_frontbuf[ORDER_HEIGHT][ORDER_WIDTH] = { 0 };

// == [ 함수 정의 ] ==
void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor, int check_friend[MAP_HEIGHT][MAP_WIDTH], bool big_cursor, 
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]); // 커서 출력 함수
void set_cursor_color(POSITION pos, char ch); // 커서 색 정하기
void highlight_P_area(POSITION pos, int check_friend[MAP_HEIGHT][MAP_WIDTH], bool big_cursor, 
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]); // 장판위에 커서 올렸을 때 커서 크기 변하기
void clear_cursor_area(POSITION pos, int check_friend[MAP_HEIGHT][MAP_WIDTH], bool big_cursor); // 커서 잔상 해결
void reset_to_original_color(POSITION pos); // 커서 잔상 해결

void display_state_map(char state_map[STATE_HEIGHT][STATE_WIDTH]); // 상태창 그리기 함수
void state_project(char src[STATE_HEIGHT][STATE_WIDTH], char dest[STATE_HEIGHT][STATE_WIDTH]); // 상태창 기본틀

void sysmes_project(char src[SYSMES_HEIGHT][SYSMES_WIDTH], char dest[SYSMES_HEIGHT][SYSMES_WIDTH]); // 시스템창 기본틀 함수
void display_sysmes_map(char sysmes_map[SYSMES_HEIGHT][SYSMES_WIDTH]); // 시스템창 그리기 함수
void clear_sysmes_frame(); // 시스템 메시지 초기화 함수

void order_project(char src[ORDER_HEIGHT][ORDER_WIDTH], char dest[ORDER_HEIGHT][ORDER_WIDTH]); // 명령창 기본틀 함수
void display_order_map(char order_map[ORDER_HEIGHT][ORDER_WIDTH]); // 명령창 그리기 함수

void reset_order_messages(); // 명령 메시지 초기화 함수
void clear_window(char buffer[][STATE_WIDTH], POSITION base_pos, int height, int width); // 상태 명령창 초기화


// [ 변수 정의 ]
char save_name_for_order[2];
char* order_message[4]; // 명령 메시지 
char* save_system_message[7] = { NULL }; // 시스템 메시지 저장
char* send_system_message[1]; // 시스템 메시지 

const int pRow[] = { -1, -1, 0, 1, 1, 1, 0, -1 }; // 8방향 확인
const int pCol[] = { 0, -1, -1, -1, 0, 1, 1, 1 }; // 8방향 확인
const int bigrow[] = {1, 1, 0}; 
const int bigcol[] = {0, 1, 1};

int base_barr_check[MAP_HEIGHT][MAP_WIDTH] = { 0 }; // 본진 - 1, 병영 - 2 구분
int shle_sold_check[MAP_HEIGHT][MAP_WIDTH] = { 0 }; // 은신처 - 1, 보병 - 2 구분
int frem_fight_fact_check[MAP_HEIGHT][MAP_WIDTH] = { 0 }; // 공장 - 1, 프레멘 - 2, 투사 - 3 구분

bool return_cursor = false; // 커서 되돌리기

// [ 보병 불러오기 ]
extern OBJECT_SAMPLE* sold[MAX_SOLD]; // 보병 배열
extern int sold_count; // 보병 갯수
extern int selected_sold; // 보병 선택되었는지 확인

// [ 프레멘 불러오기 ]
extern OBJECT_SAMPLE* frem[MAX_FREM]; // 프레멘 배열
extern int frem_count; // 프레멘 갯수
extern int selected_frem; // 프레멘 선택되었는지 확인


// =================================== [ 건물 ] ======================================= //

// ==== [ 구조체로 건물 선언 ] ==== //

// [ 아군 본진 BASE ]
OBJECT_BUILDING f_base = {
	.pos1 = {16, 1},
	.pos2 = {16, 2},
	.pos3 = {15, 1},
	.pos4 = {15, 2},
	.repr = 'B',
	.layer = 0,
	.hp = 50
};

// [ 적군 본진 BASE ]
OBJECT_BUILDING e_base = {
	.pos1 = {1, 57},
	.pos2 = {1, 58},
	.pos3 = {2, 57},
	.pos4 = {2, 58},
	.repr = 'B',
	.layer = 0,
	.hp = 50
};

// [ 초기 스파이스 매장지 아군 ]
OBJECT_BUILDING start_spice1 = {
	.pos1 = {12, 1},
	.repr = '5',
	.layer = 0
};

// [ 초기 스파이스 매장지 적군 ]
OBJECT_BUILDING start_spice2 = {
	.pos1 = {5, 58},
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
void p_start_spice1(OBJECT_BUILDING ss, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	POSITION ss_num[] = { ss.pos1 };
	map[0][ss_num[0].row][ss_num[0].column] = ss.repr;
}

void p_start_spice2(OBJECT_BUILDING ss, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	POSITION ss_num[] = { ss.pos1 };
	map[0][ss_num[0].row][ss_num[0].column] = ss.repr;
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

OBJECT_BUILDING* SPICE[MAX_SPICE]; // 스파이스 배열
int spi_count = 2; // 스파이스 갯수 

OBJECT_BUILDING* DOR[MAX_DOR]; // 숙소 배열
int dor_count = 0; // 숙소 갯수

OBJECT_BUILDING* GAR[MAX_GAR]; // 창고 배열
int gar_count = 0; // 창고 갯수

OBJECT_BUILDING* BAR[MAX_BAR]; // 병영 배열
int bar_count = 0; // 병영 갯수

OBJECT_BUILDING* SHE[MAX_SHE]; // 은신처 배열
int she_count = 0; // 은신처 갯수



// [ 모든 건물 출력 함수 ]
void p_building(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]) {
	p_f_base(f_base, map, check_friend); // 아군 본진
	p_e_base(e_base, map, check_friend); // 적군 본진
	p_f_plate(f_plate, map, check_friend); // 아군 장판
	p_e_plate(e_plate, map, check_friend); // 적군 장판
	p_start_spice1(start_spice1, map); // 초기 스파이스 매장지 아군 
	p_start_spice1(start_spice2, map); // 초기 스파이스 매장지 적군 
	p_rock_1(rock_1, map); // 바위 1 
	p_rock_2(rock_2, map); // 바위 2
	p_rock_3(rock_3, map); // 바위 3
	p_rock_4(rock_4, map); // 바위 4
	SPICE[0] = &start_spice1;
	SPICE[1] = &start_spice2;
}


// [ 적군 건물 ]
OBJECT_BUILDING* E_DOR[MAX_DOR]; // 적군 숙소 배열
int e_dor_count = 1; // 적군 숙소 갯수

OBJECT_BUILDING* E_GAR[MAX_GAR]; // 적군 창고 배열
int e_gar_count = 1; // 적군 창고 갯수

OBJECT_BUILDING* ARE[MAX_ARE]; // 적군 투기장 배열
int are_count = 1; // 적군 투기장 갯수

OBJECT_BUILDING* FAC[MAX_FAC]; // 적군 공장 배열
int fac_count = 1; // 적군 공장 갯수



/* -=-=-=-=-=-=-=-=[ 하코넨 ]-=-=-=-=-=-=-=-= -*/

// [ 하코넨 숙소 1 ]
OBJECT_BUILDING e_dor1 = {
	 .pos1 = { 1, 50 },
	 .pos2 = { 1, 51 },
	 .pos3 = { 2, 50 },
	 .pos4 = { 2, 51 },
	 .repr = 'D',
	 .hp = 10,
	 .layer = 0
};

// [ 하코넨 창고 1 ]
OBJECT_BUILDING e_gar1 = {
	 .pos1 = { 1, 47 },
	 .pos2 = { 1, 46 },
	 .pos3 = { 2, 47 },
	 .pos4 = { 2, 46 },
	 .repr = 'G',
	 .hp = 10,
	 .layer = 0
};

// [ 하코넨 투기장 1 ]
OBJECT_BUILDING e_are1 = {
	 .pos1 = { 5, 51 },
	 .pos2 = { 5, 52 },
	 .pos3 = { 6, 51 },
	 .pos4 = { 6, 52 },
	 .repr = 'A',
	 .hp = 15,
	 .layer = 0
};


// [ 하코넨 공장 1 ]
OBJECT_BUILDING e_fac1 = {
	 .pos1 = { 5, 47 },
	 .pos2 = { 5, 46 },
	 .pos3 = { 6, 47 },
	 .pos4 = { 6, 46 },
	 .repr = 'F',
	 .hp = 30,
	 .layer = 0
};

void hako_print_building(OBJECT_BUILDING* bd[], int cnt, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]);
void hako_list_building(void);



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
	set_color(COLOR_DEFAULT);// 자원 출력
	gotoxy(resource_pos);
	printf("                                                      "); // 기존 문자열 지우기
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
				if (check_friend[i][j] == 1) {
					color = 180;
				}
				else if (check_friend[i][j] == 2) {
					color = COLOR_DEFAULT + 64;
				}
				break;

			// [ 창고 ]
			case 'G':
				if (check_friend[i][j] == 1) {
					color = 180;
				}
				else if (check_friend[i][j] == 2) {
					color = COLOR_DEFAULT + 64;
				}
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
				color = COLOR_DEFAULT + 64;
				break;
			
			// [ 공장, 프레멘, 투사 ]
			case 'F':
				// [ 공장 - 1 ]
				if (frem_fight_fact_check[i][j] == 1) {
					color = COLOR_DEFAULT + 64;
				}
				// [ 프레멘 - 2 ]
				else if (frem_fight_fact_check[i][j] == 2) {
					color = 60; // 색 아직 미지정
				}
				// [ 투사 - 3 ]
				else if (frem_fight_fact_check[i][j] == 3) {
					color = COLOR_DEFAULT + 64;
				}

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

			// [ 중전차 ]
			case 'T':
				color = COLOR_DEFAULT + 64;
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

// [ 시스템 메시지 메인 출력 함수 ]
void p_system_message(char* new_message) {
	// 배열의 마지막 메시지 해제 (동적 할당된 경우)
	if (save_system_message[6] != NULL) {
		free(save_system_message[6]);
		save_system_message[6] = NULL;
	}

	// 메시지를 위로 한 칸씩 밀기
	for (int i = 6; i > 0; i--) {
		save_system_message[i] = save_system_message[i - 1];
	}

	// 새 메시지 추가 (안전한 복사)
	save_system_message[0] = malloc(strlen(new_message) + 1);
	if (save_system_message[0] != NULL) {
		strncpy_s(save_system_message[0], strlen(new_message) + 1, new_message, strlen(new_message));
		save_system_message[0][strlen(new_message)] = '\0'; // 널 종료 보장
	}

	// 시스템 메시지 초기화 후 메시지 출력
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
	bool b_b_key_press, bool s_key_press, OBJECT_SAMPLE* havs[MAX_HAV], bool* space_key_press) {

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
			order_message[0] = "이동, 순찰 명령어를 사용할 수 없습니다.";
			prints(padd(order_mes_pos[0], pos_order), order_message[0]);
		}

		break;


	/* ===== [유닛] ====== */
	// [ 하베스터 ]
	case 'H':
		for (int i = 0; i < MAX_HAV; i++) {
			if (havs[i] != NULL && havs[i]->pos.row == curr.row && havs[i]->pos.column == curr.column && colorbuf[curr.row][curr.column] == COLOR_DEFAULT + 16) {
				char mes_buf[100];
				// [ 상태 메시지 ]
				prints(padd(state_mes_pos, pos_state), "유닛 : 아군 하베스터");
				sprintf_s(mes_buf, sizeof(mes_buf), "체력: %d, 공격력: 0, 이동주기: 2초, 시야: 0", havs[i]->hp);
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
		}

		break;

	// [ 샌드웜 ]
	case 'W':
		prints(padd(state_mes_pos, pos_state), "유닛 : 샌드웜");
		*space_key_press = false;
		break;

	// [ 중전차 ]
	case 'T':
		prints(padd(state_mes_pos, pos_state), "유닛 : 중전차");
		order_message[0] = "이동, 순찰 명령어를 사용할 수 없습니다.";
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
		int input_curr[4][2] = { 0 };

		// [ 초기값 설정 ]
		int minRow = curr.row;
		int minCol = curr.column;

		// [ 현재 위치 포함 8방향 검사 배열 ]
		int dRow[9] = { 0, -1, -1, -1, 0, 1, 1, 1, 0 }; // 현재 위치 포함 (8방향)
		int dCol[9] = { 0, -1, 0, 1, 1, 1, 0, -1, -1 };

		for (int i = 0; i < 9; ++i) {
			int checkRow = curr.row + dRow[i];
			int checkCol = curr.column + dCol[i];

			// [ P인지 확인 ]
			if (map[0][checkRow][checkCol] == 'P') {
				// 가장 작은 행 또는 같은 행에서 열이 작은 위치 선택
				if (checkRow < minRow || (checkRow == minRow && checkCol < minCol)) {
					minRow = checkRow;
					minCol = checkCol;
				}
			}
		}

		// [ 가장 작은 P의 위치를 input_curr[0]에 저장 ]
		input_curr[0][0] = minRow;
		input_curr[0][1] = minCol;

		// [ pos2, pos3, pos4 설정 ]
		input_curr[1][0] = minRow;        // 위쪽 오른쪽
		input_curr[1][1] = minCol + 1;

		input_curr[2][0] = minRow + 1;    // 아래쪽 왼쪽
		input_curr[2][1] = minCol;

		input_curr[3][0] = minRow + 1;    // 아래쪽 오른쪽
		input_curr[3][1] = minCol + 1;

		// [ 맵과 관련된 데이터 업데이트 ]
		for (int i = 0; i < 4; i++) {
			map[0][input_curr[i][0]][input_curr[i][1]] = 'D';
			check_friend[input_curr[i][0]][input_curr[i][1]] = 1;
		}

		if (dor_count < MAX_DOR) {
			OBJECT_BUILDING* new_dor = (OBJECT_BUILDING*)malloc(sizeof(OBJECT_BUILDING));
			if (!new_dor) {
				p_system_message("메모리 할당 실패");
				return;
			}

			// [ 새로운 숙소 ]
			*new_dor = (OBJECT_BUILDING){
				{ input_curr[0][0], input_curr[0][1] },
				{ input_curr[1][0], input_curr[1][1] },
				{ input_curr[2][0], input_curr[2][1] },
				{ input_curr[3][0], input_curr[3][1] },
				'D', 0, 10
			};

			// [ 배열에 추가 ]
			DOR[dor_count] = new_dor;
			dor_count++;

			// [ 생산 자원 소모 ]
			resource->spice -= 2;
			resource->population_max += 10;
			p_system_message("숙소를 건설하였습니다.");
			char mes[100];
			sprintf_s(mes, sizeof(mes), "숙소 건설 후 최대 인구수: %d", resource->population_max);
			p_system_message(mes);
		}
		else {
			p_system_message("숙소를 더이상 건설할 수 없습니다.");
		}

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
		int input_curr[4][2] = { 0 };

		// [ 초기값 설정 ]
		int minRow = curr.row;
		int minCol = curr.column;

		// [ 현재 위치 포함 8방향 검사 배열 ]
		int dRow[9] = { 0, -1, -1, -1, 0, 1, 1, 1, 0 }; // 현재 위치 포함 (8방향)
		int dCol[9] = { 0, -1, 0, 1, 1, 1, 0, -1, -1 };

		for (int i = 0; i < 9; ++i) {
			int checkRow = curr.row + dRow[i];
			int checkCol = curr.column + dCol[i];

			// [ P인지 확인 ]
			if (map[0][checkRow][checkCol] == 'P') {
				// 가장 작은 행 또는 같은 행에서 열이 작은 위치 선택
				if (checkRow < minRow || (checkRow == minRow && checkCol < minCol)) {
					minRow = checkRow;
					minCol = checkCol;
				}
			}
		}

		// [ 가장 작은 P의 위치를 input_curr[0]에 저장 ]
		input_curr[0][0] = minRow;
		input_curr[0][1] = minCol;

		// [ pos2, pos3, pos4 설정 ]
		input_curr[1][0] = minRow;        // 위쪽 오른쪽
		input_curr[1][1] = minCol + 1;

		input_curr[2][0] = minRow + 1;    // 아래쪽 왼쪽
		input_curr[2][1] = minCol;

		input_curr[3][0] = minRow + 1;    // 아래쪽 오른쪽
		input_curr[3][1] = minCol + 1;

		// [ 맵과 관련된 데이터 업데이트 ]
		for (int i = 0; i < 4; i++) {
			map[0][input_curr[i][0]][input_curr[i][1]] = 'G';
			check_friend[input_curr[i][0]][input_curr[i][1]] = 1;
		}

		if (gar_count < MAX_GAR) {
			OBJECT_BUILDING* new_gar = (OBJECT_BUILDING*)malloc(sizeof(OBJECT_BUILDING));
			if (!new_gar) {
				p_system_message("메모리 할당 실패");
				return;
			}

			*new_gar = (OBJECT_BUILDING){
				{ input_curr[0][0], input_curr[0][1] },
				{ input_curr[1][0], input_curr[1][1] },
				{ input_curr[2][0], input_curr[2][1] },
				{ input_curr[3][0], input_curr[3][1] },
				'G', 0, 10
			};

			GAR[gar_count] = new_gar;
			gar_count++;
				
			// [ 생산 자원 소모 ]
			resource->spice -= 2;
			p_system_message("숙소를 건설하였습니다.");
		}
		else {
			p_system_message("숙소를 더이상 건설할 수 없습니다.");
		}
		

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
		int input_curr[4][2] = { 0 };

		// [ 초기값 설정 ]
		int minRow = curr.row;
		int minCol = curr.column;

		// [ 현재 위치 포함 8방향 검사 배열 ]
		int dRow[9] = { 0, -1, -1, -1, 0, 1, 1, 1, 0 }; // 현재 위치 포함 (8방향)
		int dCol[9] = { 0, -1, 0, 1, 1, 1, 0, -1, -1 };

		for (int i = 0; i < 9; ++i) {
			int checkRow = curr.row + dRow[i];
			int checkCol = curr.column + dCol[i];

			// [ P인지 확인 ]
			if (map[0][checkRow][checkCol] == 'P') {
				// 가장 작은 행 또는 같은 행에서 열이 작은 위치 선택
				if (checkRow < minRow || (checkRow == minRow && checkCol < minCol)) {
					minRow = checkRow;
					minCol = checkCol;
				}
			}
		}

		// [ 가장 작은 P의 위치를 input_curr[0]에 저장 ]
		input_curr[0][0] = minRow;
		input_curr[0][1] = minCol;

		// [ pos2, pos3, pos4 설정 ]
		input_curr[1][0] = minRow;        // 위쪽 오른쪽
		input_curr[1][1] = minCol + 1;

		input_curr[2][0] = minRow + 1;    // 아래쪽 왼쪽
		input_curr[2][1] = minCol;

		input_curr[3][0] = minRow + 1;    // 아래쪽 오른쪽
		input_curr[3][1] = minCol + 1;

		// [ 맵과 관련된 데이터 업데이트 ]
		for (int i = 0; i < 4; i++) {
			map[0][input_curr[i][0]][input_curr[i][1]] = 'B';
			base_barr_check[input_curr[i][0]][input_curr[i][1]] = 2;
		}

		// 동적 메모리 할당
		if (bar_count < MAX_BAR) {
			OBJECT_BUILDING* new_bar = (OBJECT_BUILDING*)malloc(sizeof(OBJECT_BUILDING));
			if (!new_bar) {
				p_system_message("메모리 할당 실패");
				return;
			}
			// [ 새로운 병영 ]
			*new_bar = (OBJECT_BUILDING){
				.pos1 = { input_curr[0][0], input_curr[0][1] },
				.pos2 = { input_curr[1][0], input_curr[1][1] },
				.pos3 = { input_curr[2][0], input_curr[2][1] },
				.pos4 = { input_curr[3][0], input_curr[3][1] },
				.repr = 'B',
				.layer = 0,
				.hp = 20
			};

			// [ 배열에 추가 ]
			BAR[bar_count] = new_bar;
			bar_count++;

			// [ 생산 자원 소모 ]
			resource->spice -= 4;
			p_system_message("병영을 건설하였습니다.");
		}
		else {
			p_system_message("병영을 더 이상 지을 수 없습니다.");
		}
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
		int input_curr[4][2] = { 0 };
		// [ 초기값 설정 ]
		int minRow = curr.row;
		int minCol = curr.column;

		// [ 현재 위치 포함 8방향 검사 배열 ]
		int dRow[9] = { 0, -1, -1, -1, 0, 1, 1, 1, 0 }; // 현재 위치 포함 (8방향)
		int dCol[9] = { 0, -1, 0, 1, 1, 1, 0, -1, -1 };

		for (int i = 0; i < 9; ++i) {
			int checkRow = curr.row + dRow[i];
			int checkCol = curr.column + dCol[i];

			// [ P인지 확인 ]
			if (map[0][checkRow][checkCol] == 'P') {
				// 가장 작은 행 또는 같은 행에서 열이 작은 위치 선택
				if (checkRow < minRow || (checkRow == minRow && checkCol < minCol)) {
					minRow = checkRow;
					minCol = checkCol;
				}
			}
		}

		// [ 가장 작은 P의 위치를 input_curr[0]에 저장 ]
		input_curr[0][0] = minRow;
		input_curr[0][1] = minCol;

		// [ pos2, pos3, pos4 설정 ]
		input_curr[1][0] = minRow;        // 위쪽 오른쪽
		input_curr[1][1] = minCol + 1;

		input_curr[2][0] = minRow + 1;    // 아래쪽 왼쪽
		input_curr[2][1] = minCol;

		input_curr[3][0] = minRow + 1;    // 아래쪽 오른쪽
		input_curr[3][1] = minCol + 1;

		// [ 맵과 관련된 데이터 업데이트 ]
		for (int i = 0; i < 4; i++) {
			map[0][input_curr[i][0]][input_curr[i][1]] = 'S';
			shle_sold_check[input_curr[i][0]][input_curr[i][1]] = 1;
		}


		if (she_count < MAX_BAR) {
			OBJECT_BUILDING* new_she = (OBJECT_BUILDING*)malloc(sizeof(OBJECT_BUILDING));
			
			// [ 예외처리 ]
			if (!new_she) {
				p_system_message("메모리 할당 실패");
				return;
			}

			// [ 새로운 은신처 ]
			*new_she = (OBJECT_BUILDING){
				{ input_curr[0][0], input_curr[0][1] },
				{ input_curr[1][0], input_curr[1][1] },
				{ input_curr[2][0], input_curr[2][1] },
				{ input_curr[3][0], input_curr[3][1] },
				'S', 0, 10
			};

			// [ 배열에 은신처 추가 ]
			SHE[she_count] = new_she;

			// [ 카운트 증가 ]
			she_count++;

			// [ 생산 자원 소모 ]
			resource->spice -= 5;
			p_system_message("은신처를 건설하였습니다.");
		}
		else {
			p_system_message("은신처를 더이상 지을 수 없습니다.");
		}

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
				OBJECT_SAMPLE* new_hav = (OBJECT_SAMPLE*)malloc(sizeof(OBJECT_SAMPLE));
				if (!new_hav) {
					p_system_message("메모리 할당 실패");
					return;
				}

				// [ 새로운 하베스터 ]
				*new_hav = (OBJECT_SAMPLE){
					{ 14, 2 }, { 14, 2 }, 'H', 2000, 2000,
					COLOR_DEFAULT + 16, 70, 0, 0, 0, 4000, 4000
				};

				// [ 하베스터를 배열에 추가 ]
				havs[hav_count] = new_hav;

				// [ 맵에 하베스터 위치 반영 ]
				map[1][havs[hav_count]->pos.row][havs[hav_count]->pos.column] = 'H';
				check_friend[havs[hav_count]->pos.row][havs[hav_count]->pos.column] = 1;

				// [ 시스템 메시지 추가 ]
				p_system_message("하베스터가가 생산되었습니다.");
				hav_count++;

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
void press_s_s(CURSOR cursor, RESOURCE* resource, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], bool* space_key_press) {
	POSITION curr = cursor.current;

	// [ 디버깅 ]
	/*char mes[100];
	sprintf_s(mes, sizeof(mes), "space_key_press: %d", *space_key_press);
	p_system_message(mes);*/

	if (*space_key_press) {
		p_system_message("보병 생산 S키를 눌렀습니다.");
		for (int i = 0; i < bar_count; i++) {
			/*char mes[100];
			sprintf_s(mes, sizeof(mes), "pos1 -> %d %d", BAR[0]->pos1.row, BAR[0]->pos1.column);
			p_system_message(mes); */

			if (base_barr_check[curr.row][curr.column] == 2 && \
				((curr.row == BAR[i]->pos1.row && curr.column == BAR[i]->pos1.column) || \
				(curr.row == BAR[i]->pos2.row && curr.column == BAR[i]->pos2.column) || \
				(curr.row == BAR[i]->pos3.row && curr.column == BAR[i]->pos3.column) || \
				(curr.row == BAR[i]->pos4.row && curr.column == BAR[i]->pos4.column))) {
				if (resource->spice >= 1 && resource->population + 1 <= resource->population_max) {
					// [ 보병 생성 ]
					if (sold_count < MAX_SOLD) {
						OBJECT_SAMPLE* new_sold = (OBJECT_SAMPLE*)malloc(sizeof(OBJECT_SAMPLE));
						if (!new_sold) {
							p_system_message("메모리 할당 실패");
							return;
						}
						// [ 자원 감소 & 인구수 증가 ]
						resource->spice -= 1;
						resource->population += 1;

						// [ 새로운 보병 생성 ]
						*new_sold = (OBJECT_SAMPLE){
							.pos = { BAR[i]->pos1.row - 1, BAR[i]->pos1.column },
							.dest = { BAR[i]->pos1.row - 1, BAR[i]->pos1.column },
							.repr = 'S',
							.move_period = 1000,
							.next_move_time = 1000,
							.dps = 5,
							.hp = 15,
							.eyes = 1,
							.attack_period = 800,
							.next_attack_time = 800,
							.patrol_point = {-1, -1},
							.is_patrolling = false,
							.original_dest = {-1, -1},
							.e_base_attack = false,
							.e_dor_attack = false,
							.e_gar_attack = false,
							.e_arena_attack = false,
							.e_factory_attack = false,
							.enemy_found = false
						};

						// 배열에 보병 넣기
						sold[sold_count] = new_sold;

						// 보병 위치에 생성
						shle_sold_check[new_sold->pos.row][new_sold->pos.column] = 2;  // 역참조

						// 보병 위치 맵에 반영
						map[1][new_sold->pos.row][new_sold->pos.column] = new_sold->repr;
						sold_count++;

						// [ 시스템 메시지 ]
						p_system_message("보병을 생산하였습니다.");
						break;
					}
					else {
						p_system_message("보병을 더이상 생산할 수 없습니다.");
					}
				}
				else if (resource->spice < 1) {
					p_system_message("생산 자원이 부족하여 보병을 생산할 수 없습니다.");
					break;
				}
				else if (resource->population + 1 > resource->population_max) {
					p_system_message("인구수가 부족하여 보병을 생산할 수 없습니다.");
					break;
				}
			}
		}
	}
}

// [ F (프레멘 생산) 키를 눌렀을 때 ]
void press_f(CURSOR cursor, RESOURCE* resource, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], bool* space_key_press) {
	POSITION curr = cursor.current;

	if (*space_key_press) {
		for (int i = 0; i < she_count; i++) {
			if (shle_sold_check[curr.row][curr.column] == 1 && \
				((curr.row == SHE[i]->pos1.row && curr.column == SHE[i]->pos1.column) || \
				(curr.row == SHE[i]->pos2.row && curr.column == SHE[i]->pos2.column) || \
				(curr.row == SHE[i]->pos3.row && curr.column == SHE[i]->pos3.column) || \
				(curr.row == SHE[i]->pos4.row && curr.column == SHE[i]->pos4.column))) {
				if (resource->spice >= 5 && resource->population + 5 <= resource->population_max) {
					if (frem_count < MAX_FREM) {

						// [ 프레멘 생성 ]
						OBJECT_SAMPLE* new_frem = (OBJECT_SAMPLE*)malloc(sizeof(OBJECT_SAMPLE));
						if (!new_frem) {
							p_system_message("메모리 할당 실패");
							return;
						}

						// [ 자원 감소, 인구수 증가 ]
						resource->spice -= 5;
						resource->population += 5;

						// [ 새로운 프레멘 생성 ]
						*new_frem = (OBJECT_SAMPLE){
							.pos = { SHE[i]->pos1.row - 1, SHE[i]->pos1.column },
							.dest = { SHE[i]->pos1.row - 1, SHE[i]->pos1.column },
							.repr = 'F',
							.move_period = 400,
							.next_move_time = 400,
							.dps = 15,
							.hp = 25,
							.eyes = 8,
							.attack_period = 200,
							.next_attack_time = 200,
							.patrol_point = {-1, -1},
							.is_patrolling = false,
							.original_dest = {-1, -1},
							.e_base_attack = false,
							.e_dor_attack = false,
							.e_gar_attack = false,
							.e_arena_attack = false,
							.e_factory_attack = false,
							.enemy_found = false
						};

						// [ 배열에 프레멘 넣기 ]
						frem[frem_count] = new_frem;

						// [ 프레멘 위치에 생성 ]
						frem_fight_fact_check[new_frem->pos.row][new_frem->pos.column] = 2;  // 2가 프레멘

						// [ 프레멘 위치 맵에 반영 ]
						map[1][new_frem->pos.row][new_frem->pos.column] = new_frem->repr;
						frem_count++;

						// [ 시스템 메시지 ]
						p_system_message("프레멘을 생산하였습니다.");
						break;
					}
					else {
						p_system_message("프레멘을 더이상 생산할 수 없습니다.");
					}
				}
				else if (resource->spice < 5) {
					p_system_message("생산 자원이 부족하여 프레멘을 생산할 수 없습니다.");
					break;
				}
				else if (resource->population + 5 > resource->population_max) {
					p_system_message("인구수가 부족하여 프레멘을 생산할 수 없습니다.");
					break;
				}
			}
		}	
	}
}






/* -=-=-=-=-=-=-=-=[ 하코넨 진영 AI ]-=-=-=-=-=-=-=-= -*/

// [ 하코넨 건물 초기화 ]
void hako_list_building(void) {
	E_DOR[0] = &e_dor1;
	E_GAR[0] = &e_gar1;
	ARE[0] = &e_are1;
	FAC[0] = &e_fac1;
}

// [ 하코넨 건물 출력 ]
void hako_print_building(OBJECT_BUILDING* bd[], int cnt, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]) {
	for (int i = 0; i < cnt; i++) {
		map[0][bd[i]->pos1.row][bd[i]->pos1.column] = bd[i]->repr;
		map[0][bd[i]->pos2.row][bd[i]->pos2.column] = bd[i]->repr;
		map[0][bd[i]->pos3.row][bd[i]->pos3.column] = bd[i]->repr;
		map[0][bd[i]->pos4.row][bd[i]->pos4.column] = bd[i]->repr;
		check_friend[bd[i]->pos1.row][bd[i]->pos1.column] = 2;
		check_friend[bd[i]->pos2.row][bd[i]->pos2.column] = 2;
		check_friend[bd[i]->pos3.row][bd[i]->pos3.column] = 2;
		check_friend[bd[i]->pos4.row][bd[i]->pos4.column] = 2;
	}
}

// [ 하코넨 초기 건물 배치 ]
void hako_building(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]) {
	// Ai지능이 낮아서 배열에 넣어도 의미가 없지만 그래도 일단 배열에 넣었음.
	hako_list_building();
	
	// [ 하코넨 건물 출력 ]
	hako_print_building(E_DOR, e_dor_count, map, check_friend); // 숙소
	hako_print_building(E_GAR, e_gar_count, map, check_friend); // 창고
	hako_print_building(ARE, are_count, map, check_friend); // 투기장
	hako_print_building(FAC, fac_count, map, check_friend); // 공장
	frem_fight_fact_check[FAC[0]->pos1.row][FAC[0]->pos1.column] = 1;
	frem_fight_fact_check[FAC[0]->pos2.row][FAC[0]->pos2.column] = 1;
	frem_fight_fact_check[FAC[0]->pos3.row][FAC[0]->pos3.column] = 1;
	frem_fight_fact_check[FAC[0]->pos4.row][FAC[0]->pos4.column] = 1;
}


