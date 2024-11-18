/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#include "display.h"
#include "io.h"

// [ ����� ������� �»��(topleft) ��ǥ ]
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

// [ �Լ� ���� ]
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


// [ ���� ���� ]
char save_name_for_order[2];
char* order_message[4];
char* save_system_message[7] = { NULL };
char* send_system_message[1];

const int pRow[] = { -1, -1, 0, 1, 1, 1, 0, -1 };
const int pCol[] = { 0, -1, -1, -1, 0, 1, 1, 1 };
const int bigrow[] = {1, 1, 0}; 
const int bigcol[] = {0, 1, 1};

int base_barr_check[MAP_HEIGHT][MAP_WIDTH] = { 0 }; // ���� - 1, ���� - 2 ����
int shle_sold_check[MAP_HEIGHT][MAP_WIDTH] = { 0 }; // ����ó - 1, ���� - 2 ����
int frem_fight_fact_check[MAP_HEIGHT][MAP_WIDTH] = { 0 }; // ���� - 1, ������ - 2, ���� - 3 ����

bool return_cursor = false;


// =================================== [ �ǹ� ] ======================================= //

// ==== [ ����ü�� �ǹ� ���� ] ==== //

// [ �Ʊ� ���� BASE ]
OBJECT_BUILDING f_base = {
	.pos1 = {16, 1},
	.pos2 = {16, 2},
	.pos3 = {15, 1},
	.pos4 = {15, 2},
	.repr = 'B',
	.layer = 0
};

// [ ���� ���� BASE ]
OBJECT_BUILDING e_base = {
	.pos1 = {1, 57},
	.pos2 = {1, 58},
	.pos3 = {2, 57},
	.pos4 = {2, 58},
	.repr = 'B',
	.layer = 0
};

// [ �ʱ� �����̽� ������ 2�� ]
OBJECT_BUILDING start_spice = {
	.pos1 = {12, 1},
	.pos2 = {5, 58},
	.repr = '5',
	.layer = 0
};

// [ �Ʊ� ���� ]
OBJECT_BUILDING f_plate = {
	.pos1 = {15, 3},
	.pos2 = {15, 4},
	.pos3 = {16, 3},
	.pos4 = {16, 4},
	.repr = 'P',
	.layer = 0
};

// [ ���� ���� ]
OBJECT_BUILDING e_plate = {
	.pos1 = {1, 55},
	.pos2 = {2, 55},
	.pos3 = {1, 56},
	.pos4 = {2, 56},
	.repr = 'P',
	.layer = 0
};

// [ ���� 1 ~ 4 ]
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

// [ �Ʊ� ���� ��� �Լ� ]
void p_f_base(OBJECT_BUILDING fb, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]) {
	POSITION fb_num[] = { fb.pos1, fb.pos2, fb.pos3, fb.pos4 };
	for (int i = 0; i < 4; i++) {
		map[0][fb_num[i].row][fb_num[i].column] = fb.repr;
		check_friend[fb_num[i].row][fb_num[i].column] = 1;
		base_barr_check[fb_num[i].row][fb_num[i].column] = 1;
	}
}

// [ ���� ���� ��� �Լ� ]
void p_e_base(OBJECT_BUILDING eb, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]) {
	POSITION eb_num[] = { eb.pos1, eb.pos2, eb.pos3, eb.pos4 };
	for (int i = 0; i < 4; i++) {
		map[0][eb_num[i].row][eb_num[i].column] = eb.repr;
		check_friend[eb_num[i].row][eb_num[i].column] = 2;
		base_barr_check[eb_num[i].row][eb_num[i].column] = 1;
	}
}

// [ �Ʊ� ���� ��� �Լ� ]
void p_f_plate(OBJECT_BUILDING fp, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]) {
	POSITION fp_num[] = { fp.pos1, fp.pos2, fp.pos3, fp.pos4 };
	for (int i = 0; i < 4; i++) {
		map[0][fp_num[i].row][fp_num[i].column] = fp.repr;
		check_friend[fp_num[i].row][fp_num[i].column] = 1;
	}
}

// [ ���� ���� ��� �Լ� ] 
void p_e_plate(OBJECT_BUILDING ep, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]) {
	POSITION ep_num[] = { ep.pos1, ep.pos2, ep.pos3, ep.pos4 };
	for (int i = 0; i < 4; i++) {
		map[0][ep_num[i].row][ep_num[i].column] = ep.repr;
		check_friend[ep_num[i].row][ep_num[i].column] = 2;
	}
}

// [ �ʱ� �����̽� ������ 2��  �Ʊ�  ���� ��� �Լ� ]
void p_start_spice(OBJECT_BUILDING ss, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	POSITION ss_num[] = { ss.pos1, ss.pos2 };
	for (int i = 0; i < 2; i++) {
		map[0][ss_num[i].row][ss_num[i].column] = ss.repr;
	}
}

// [ ���� 1 ~ 4 ��� �Լ� ]
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

// [ ��� �ǹ� ��� �Լ� ]
void p_building(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]) {
	p_f_base(f_base, map, check_friend); // �Ʊ� ����
	p_e_base(e_base, map, check_friend); // ���� ����
	p_f_plate(f_plate, map, check_friend); // �Ʊ� ����
	p_e_plate(e_plate, map, check_friend); // ���� ����
	p_start_spice(start_spice, map); // �ʱ� �����̽� ������ �Ʊ� 1 ���� 1
	p_rock_1(rock_1, map); // ���� 1 
	p_rock_2(rock_2, map); // ���� 2
	p_rock_3(rock_3, map); // ���� 3
	p_rock_4(rock_4, map); // ���� 4
}


// =================================== [ ���� ] ======================================= //
// [ �Ϻ����� ]


// [ ���� ]


// =================================== [ DISPLAY ] ======================================= //

// [ Display ���� �Լ� ]
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

// [ ȭ�鿡 �ڿ�, �α��� ����ϴ� �Լ� ]
void display_resource(RESOURCE resource) {
	set_color(COLOR_DEFAULT);
	gotoxy(resource_pos);
	printf("spice = %d/%d, population=%d/%d\n",
		resource.spice, resource.spice_max,
		resource.population, resource.population_max
	);
}

// [ Display �⺻ Ʋ �Լ� ]
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

// [ ȭ�鿡 �ǹ� �� ������ �����ִ� �Լ� ]
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]) {
	project(map, backbuf);

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (frontbuf[i][j] == backbuf[i][j]) continue;

			POSITION pos = { i, j };
			char cell = backbuf[i][j];
			int color = COLOR_RESOURCE;

			// === [ �ǹ� �� ���� ���� ���� ] ===
			switch (cell) {
			// [ ���� & ���� ]
			case 'B':
				// [ ���� ]
				if (base_barr_check[i][j] == 1) {
					if (check_friend[i][j] == 1) {
						color = COLOR_DEFAULT + 16;  // �Ʊ� ���� (�Ķ��� ���) 
					}
					else if (check_friend[i][j] == 2) {
						color = COLOR_DEFAULT + 64;  // ���� ���� (������ ���)
					}
				}
				// [ ���� ]
				else if (base_barr_check[i][j] == 2) {
					if (check_friend[i][j] == 1) {
						color = 181;
					}
				}
				break;

			// [ ���� ]
			case 'P':  
				color = COLOR_BLACK;  
				break;

			// [ �����̽� ]
			case '1': case '2': case '3': case '4': case '5':
			case '6': case '7': case '8': case '9': 
				color = COLOR_DEFAULT + 48;  // ��Ȳ�� ���
				break;

			// [ ���� ]
			case 'R': 
				color = COLOR_DEFAULT;
				break;
			
			// [ ���� ]
			case 'D':
				color = 180; // �� ���� ������ 
				break;

			// [ â�� ]
			case 'G':
				color = 180; // �� ���� ������ 
				break;

			// [ ����ó & ���� ]
			case 'S':
				// [ ����ó ]
				if (shle_sold_check[i][j] == 1) {
					color = 180; // �� ���� ������ 
					
				}

				// [ ���� ]
				else if (shle_sold_check[i][j] == 2) {
					color = 150; // �� ���� ������ 

				}
				break;

			// [ ������ ]
			case 'A':

				break;
			
			// [ ���� ]
			case 'F':

				break;


			/* ===== [ ���� ] ===== */

			// [ �Ϻ����� ]
			case 'H':  
				if (check_friend[i][j] == 1) {
					color = COLOR_DEFAULT + 16;  // �Ʊ� �Ϻ����� (�Ķ���)
				}
				else if (check_friend[i][j] == 2) {
					color = COLOR_DEFAULT + 64;  // ���� �Ϻ����� (������)
				}
				break;

			// [ ����� ]
			case 'W': 
				color = COLOR_DEFAULT + 96;
				break;
			
			// [ �縷 ������ ]
			case 'E': 
				color = COLOR_DEFAULT + 17;
				break;

			default:
				break;  // ��Ÿ - COLOR_RESOURCE�� ó����
			}

			printc(padd(map_pos, pos), cell, color);
			colorbuf[i][j] = color;
			frontbuf[i][j] = cell;
		}
	}
}


/* ======================================= [ Ŀ�� ���� �Լ� ] ======================================= */

// [ �ǹ�, ���� Ŀ�� ���� ���� �Լ� ]
void set_cursor_color(POSITION pos, char ch) {
	printc(padd(map_pos, pos), ch, B_CURSOR_COLOR);
}

// [ Ŀ�� �ܻ���� �ذ� �Լ� ]
void reset_to_original_color(POSITION pos) {
	int color = colorbuf[pos.row][pos.column];
	char ch = frontbuf[pos.row][pos.column];
	printc(padd(map_pos, pos), ch, color);
}

// [ Ŀ�� �ܻ� ���� �ذ� �Լ� ]
void clear_cursor_area(POSITION pos, int check_friend[MAP_HEIGHT][MAP_WIDTH], bool big_cursor) {

	// [ ������ �� 2 X 2 ũ�� ]
	if (frontbuf[pos.row][pos.column] == 'P' && check_friend[pos.row][pos.column] == 1) {
		
		// [ ���� ��ġ ]
		reset_to_original_color(pos);  
		
		// [ 8 ���� �˻� ]
		for (int i = 0; i < 8; ++i) {  
			int newRow = pos.row + pRow[i];
			int newCol = pos.column + pCol[i];
			if (frontbuf[newRow][newCol] == 'P' && check_friend[pos.row][pos.column] == 1) {
				POSITION newPos = { newRow, newCol };
				reset_to_original_color(newPos);
			}
		}
	}
	// [ 2 X 2 Ŀ�� ]
	else if (big_cursor == true) {
		
		// [ ���� ��ġ ]
		reset_to_original_color(pos);  

		// [ 2 X 2 �˻� ]
		for (int i = 0; i < 3; ++i) {  
			int newRow = pos.row + bigrow[i];
			int newCol = pos.column + bigcol[i];
			
			POSITION newPos = { newRow, newCol };
			reset_to_original_color(newPos);
			
		}
	}

	// [ 1 X 1 Ŀ�� ]
	else {
		reset_to_original_color(pos);  
		
	}
}

// [ Ŀ�� ���� ���� �� ����ϴ� �Լ� ]
void highlight_P_area(POSITION pos, int check_friend[MAP_HEIGHT][MAP_WIDTH], bool big_cursor,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {

	// [ ������ �� 2 X 2 ũ�� ]
	if (frontbuf[pos.row][pos.column] == 'P' && check_friend[pos.row][pos.column] == 1) {
		set_cursor_color(pos, frontbuf[pos.row][pos.column]);  // ���� ��ġ
		for (int i = 0; i < 8; ++i) {  // 8���� �˻�
			int newRow = pos.row + pRow[i];
			int newCol = pos.column + pCol[i];
			if (frontbuf[newRow][newCol] == 'P' && check_friend[pos.row][pos.column] == 1) {
				POSITION newPos = { newRow, newCol };
				set_cursor_color(newPos, frontbuf[newRow][newCol]);
			}
		}
	}
	// [ �ǹ��� 2 X 2�� �� Ŀ�� 2 X 2 ũ�� ]
	else if (big_cursor == true) {

		// [ ���� ��ġ ]
		// [ ���� �� ���� ���� ������ �� ����� ]
		if (map[0][pos.row][pos.column] == ' ' &&  map[1][pos.row][pos.column] == -1 && \
			map[2][pos.row][pos.column] == -1) {
			set_cursor_color(pos, frontbuf[pos.row][pos.column]);
		}
		
		// [ 2 X 2 �˻� ]
		for (int i = 0; i < 3; ++i) {  
			int newRow = pos.row + bigrow[i];
			int newCol = pos.column + bigcol[i];
			// [ ���� �� ���� ���� ������ �� ����� ]
			if (newRow != MAP_HEIGHT - 1 && newCol != MAP_WIDTH - 1 && map[0][newRow][newCol] == ' ' && \
				map[1][newRow][newCol] == -1 && map[2][newRow][newCol] == -1) {
				POSITION newPos = { newRow, newCol };
				set_cursor_color(newPos, frontbuf[newRow][newCol]);
			}
		}
	}

	// [ 1 X 1 Ŀ�� ]
	else {
		// [ 1 X 1 Ŀ�� ��� ]
		printc(padd(map_pos, pos), frontbuf[pos.row][pos.column], COLOR_BLACK);
		
		// [ �����ڿ��� ��� �ǹ��� ��ġ�� ���� ���� ����ó�� 2 X 2 �ܻ� ����� ]
		if (return_cursor) {
			for (int i = 0; i < 3; ++i) {
				int newRow = pos.row + bigrow[i];
				int newCol = pos.column + bigcol[i];

				// [ 2 X 2 �ܻ��� �����ִٸ� ���� �������� �ǵ����� ]
				if (newRow < MAP_HEIGHT && newCol < MAP_WIDTH) {
					POSITION newPos = { newRow, newCol };
					reset_to_original_color(newPos);
				}
			}
		}
	}
}

// [ ���� Ŀ�� ��� �Լ� ]
void display_cursor(CURSOR cursor, int check_friend[MAP_HEIGHT][MAP_WIDTH], bool big_cursor,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;
	
	// [ Ŀ�� ���� ������ �ǵ����� & ������ ���� ������ �ǵ����� ] 
	clear_cursor_area(prev, check_friend, big_cursor);

	// [ Ŀ�� ��� & ���� �� �ٲٱ� ]
	highlight_P_area(curr, check_friend, big_cursor, map);
}	


/* ======================================= [ �⺻ Ʋ ��� �Լ� ] ======================================= */

// [ ����â �⺻ Ʋ �Լ� ] 
void state_project(char src[STATE_HEIGHT][STATE_WIDTH], char dest[STATE_HEIGHT][STATE_WIDTH]) {
	for (int i = 0; i < STATE_HEIGHT; i++) {
		for (int j = 0; j < STATE_WIDTH; j++) {
			if (src[i][j] >= 0) {
				dest[i][j] = src[i][j];
			}
		}
	}
}

// [ ����â �׸��� �Լ� ]
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

// [ �ý���â �⺻ Ʋ �Լ� ]
void sysmes_project(char src[SYSMES_HEIGHT][SYSMES_WIDTH], char dest[SYSMES_HEIGHT][SYSMES_WIDTH]) {
	for (int i = 0; i < SYSMES_HEIGHT; i++) {
		for (int j = 0; j < SYSMES_WIDTH; j++) {
			if (src[i][j] >= 0) {
				dest[i][j] = src[i][j];
			}
		}
	}
}

// [ �ý��� â �׸��� �Լ� ]
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

// [ ���â �⺻ Ʋ �Լ� ]
void order_project(char src[ORDER_HEIGHT][ORDER_WIDTH], char dest[ORDER_HEIGHT][ORDER_WIDTH]) {
	for (int i = 0; i < ORDER_HEIGHT; i++) {
		for (int j = 0; j < ORDER_WIDTH; j++) {
			if (src[i][j] >= 0) {
				dest[i][j] = src[i][j];
			}
		}
	}
}

// [ ���â �׸��� �Լ� ]
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


/* ======================================= [ �ý��� �޽��� �Լ� & �ʱ�ȭ �Լ� ] ======================================= */

// [ ��� �޽��� �ʱ�ȭ �� ��� ���� �ʱ�ȭ ]
void reset_order_messages() {
	for (int i = 0; i < 4; i++) order_message[i] = 0;
}

// [ ����, ���â �ʱ�ȭ �Լ� ]
void clear_window(char buffer[][STATE_WIDTH], POSITION base_pos, int height, int width) {
	for (int i = 1; i < height - 1; i++) {
		for (int j = 1; j < width - 1; j++) {
			POSITION pos = { i, j };
			printc(padd(base_pos, pos), buffer[i][j], COLOR_BLACK);
		}
	}
}

// // [ �ý��� �޽��� �ʱ�ȭ �Լ� ]
void clear_sysmes_frame() {
	for (int i = 1; i < SYSMES_HEIGHT - 1; i++) {
		for (int j = 1; j < SYSMES_WIDTH - 1; j++) {
			POSITION pos = { i, j };
			printc(padd(sysmes_pos, pos), sysmes_backbuf[i][j], COLOR_BLACK);
		}
	}
}

// [ �ý��� �޽��� ��� ]
void p_system_message(char* new_message) {
	// [ �޽����� ���� �� ĭ�� �б� ]
	for (int i = 6; i > 0; i--) {
		save_system_message[i] = save_system_message[i - 1];
	}
	save_system_message[0] = new_message;

	// [ �ý��� �޽��� �ʱ�ȭ �� �޽��� ��� ]
	clear_sysmes_frame();
	POSITION pos_system = { 0, 1 };
	for (int i = 0; i < 7; i++) {
		if (save_system_message[i] != NULL) {
			prints(padd(system_mes_pos[i], pos_system), save_system_message[i]);
		}
	}
}


/* ======================================= [ ���� Ű ��ɾ� �Լ� ] ======================================= */

// [ �����̽� �� Ű�� ������ �� ]
void state_spacebar(RESOURCE* resource, CURSOR cursor, OBJECT_SAMPLE* f_hav_obj, OBJECT_SAMPLE* e_hav_obj, 
	int check_friend[MAP_HEIGHT][MAP_WIDTH], bool p_key_press, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], bool d_key_press, bool g_key_press,
	bool b_b_key_press, bool s_key_press) {

	// [ ���� ���� ]
	POSITION curr = cursor.current;
	POSITION pos_state = { 0, 1 };
	POSITION pos_order = { 0, 1 };
	char cell = backbuf[curr.row][curr.column];

	// [ �ʱ�ȭ ]
	reset_order_messages();
	clear_window(state_backbuf, state_pos, STATE_HEIGHT, STATE_WIDTH);
	clear_window(order_backbuf, order_pos, ORDER_HEIGHT, ORDER_WIDTH);
	return_cursor = false;

	
	switch (cell) {
	case ' ':
		prints(padd(state_mes_pos, pos_state), "(��)���� : �縷 ����");
		break;

	// [ ���� & ���� ]
	case 'B':
		// [ ���� ]
		if (base_barr_check[curr.row][curr.column] == 1) {
			if (check_friend[curr.row][curr.column] == 1) {
				save_name_for_order[0] = 'B';
				save_name_for_order[1] = 'F';
				prints(padd(state_mes_pos, pos_state), "�ǹ� : �Ʊ� ����");
				prints(padd(state_mes2_pos, pos_state), "�Ϻ����͸� ������ �� �ִ�.");
				prints(padd(order_mes_pos[0], pos_order), "�Ϻ����� ���� (H / ESC)");
				prints(padd(order_mes_pos[1], pos_order), "������ : 5, �ʿ� �α��� : 5");
			}
			else if (check_friend[curr.row][curr.column] == 2) {
				save_name_for_order[0] = 'B';
				save_name_for_order[1] = 'E';
				prints(padd(state_mes_pos, pos_state), "�ǹ� : ���� ����");
			}
		}
		// [ ���� ]
		else if (base_barr_check[curr.row][curr.column]	 == 2) {
			prints(padd(state_mes_pos, pos_state), "�ǹ� : ����");
			prints(padd(state_mes2_pos, pos_state), "������ ������ �� �ִ�.");
			order_message[0] = "���� ����(S / ESC)";
			prints(padd(order_mes_pos[0], pos_order), order_message[0]);
		}
		break;

	// [ ���� ] 
	case 'P':
		if (check_friend[curr.row][curr.column] == 1) {
			prints(padd(state_mes_pos, pos_state), "�ǹ� : �Ʊ� ����");
			prints(padd(state_mes2_pos, pos_state), "����, â��, ����, ����ó�� ��ġ�� �� �ִ�.");
		}
		else if (check_friend[curr.row][curr.column] == 2) {
			prints(padd(state_mes_pos, pos_state), "�ǹ� : ���� ����");
		}
		break;

	// [ �����̽� ]
	case '1': case '2': case '3': case '4': case '5':
	case '6': case '7': case '8': case '9':
		prints(padd(state_mes_pos, pos_state), "�ǹ� : �����̽�");
		break;

	// [ ���� ]
	case 'R':
		prints(padd(state_mes_pos, pos_state), "�ǹ� : ����");
		break;

	// [ ���� ]
	case 'D':
		prints(padd(state_mes_pos, pos_state), "�ǹ� : ����");
		prints(padd(state_mes2_pos, pos_state), "�α� �ִ�ġ�� 10 �����Ѵ�.");
		break;

	// [ â�� ]
	case 'G':
		prints(padd(state_mes_pos, pos_state), "�ǹ� : â��");
		prints(padd(state_mes2_pos, pos_state), "�����̽� ���� �ִ�ġ�� 10 �����Ѵ�.");
		break;

	// [ ����ó & ���� ]
	case 'S':
		// [ ����ó ]
		if (shle_sold_check[curr.row][curr.column] == 1) {
			prints(padd(state_mes_pos, pos_state), "�ǹ� : ����ó");
			prints(padd(state_mes2_pos, pos_state), "�������� ������ �� �ִ�.");
			order_message[0] = "������ ����(F / ESC)";
			prints(padd(order_mes_pos[0], pos_order), order_message[0]);
		}

		// [ ���� ]
		else if (shle_sold_check[curr.row][curr.column] == 2) {
			prints(padd(state_mes_pos, pos_state), "���� : ����");
			order_message[0] = "M : �̵�, P : ����";
			prints(padd(order_mes_pos[0], pos_order), order_message[0]);
		}

		break;

	// [ ������ ]
	case 'A':
		prints(padd(state_mes_pos, pos_state), "�ǹ� : ������");
		prints(padd(state_mes2_pos, pos_state), "���縦 ������ �� �ִ�.");
		order_message[0] = "������ ���縦 ������ �� �ֽ��ϴ�.";
		prints(padd(order_mes_pos[0], pos_order), order_message[0]);
		break;

	// [ ���� & ������ & ���� ]
	case 'F':
		// [ ���� ]
		if (frem_fight_fact_check[curr.row][curr.column] == 1) {
			prints(padd(state_mes_pos, pos_state), "�ǹ� : ����");
			prints(padd(state_mes2_pos, pos_state), "�������� ������ �� �ִ�.");
			order_message[0] = "������ �������� ������ �� �ֽ��ϴ�.";
			prints(padd(order_mes_pos[0], pos_order), order_message[0]);
		}

		// [ ������ ]
		else if (frem_fight_fact_check[curr.row][curr.column] == 2) {
			prints(padd(state_mes_pos, pos_state), "Ư�� ���� : ������");
			order_message[0] = "M : �̵�, P : ����";
			prints(padd(order_mes_pos[0], pos_order), order_message[0]);
		}

		// [ ���� ]
		else if (frem_fight_fact_check[curr.row][curr.column] == 3) {
			prints(padd(state_mes_pos, pos_state), "���� ���� : ����");
			order_message[0] = "���� ������ �̵�, ���� ��ɾ ����� �� �����ϴ�.";
			prints(padd(order_mes_pos[0], pos_order), order_message[0]);
		}

		break;


	/* ===== [����] ====== */
	// [ �Ϻ����� ]
	case 'H':
		if (colorbuf[curr.row][curr.column] == COLOR_DEFAULT + 16) {
			char mes_buf[100];
			// [ ���� �޽��� ]
			prints(padd(state_mes_pos, pos_state), "���� : �Ʊ� �Ϻ�����");
			sprintf_s(mes_buf, sizeof(mes_buf), "ü��: %d, ���ݷ�: 0, �̵��ֱ�: 2��, �þ�: 0", f_hav_obj->hp);
			prints(padd(state_mes2_pos, pos_state), mes_buf);

			// [ �Ϻ����� �ʻ�ȭ ]
			prints(padd(state_mes4_pos, pos_state), "     -----------------");
			prints(padd(state_mes5_pos, pos_state), "     |   _________   |");
			prints(padd(state_mes6_pos, pos_state), "     |  /[=] | [=]\\  |");
			prints(padd(state_mes7_pos, pos_state), "     |  |[=] | [=]|  |");
			prints(padd(state_mes8_pos, pos_state), "     |  -----------  |");
			prints(padd(state_mes9_pos, pos_state), "     |   O       O   | ");
			prints(padd(state_mes10_pos, pos_state), "     |               |");
			prints(padd(state_mes11_pos, pos_state), "     -----------------");
			// [ ��� �޽��� ]
			prints(padd(order_mes_pos[0], pos_order), "H : ��Ȯ, M : �̵�");
		}
		else if (colorbuf[curr.row][curr.column] == COLOR_DEFAULT + 64) {
			char mes_buf[100];
			prints(padd(state_mes_pos, pos_state), "���� : ���� �Ϻ�����");
			sprintf_s(mes_buf, sizeof(mes_buf), "ü��: %d, ���ݷ�: 0, �̵��ֱ�: 2��, �þ�: 0", e_hav_obj->hp);
			prints(padd(state_mes2_pos, pos_state), mes_buf);
		}
		break;

	// [ ����� ]
	case 'W':
		prints(padd(state_mes_pos, pos_state), "���� : �����");
		break;

	// [ ������ ]
	case 'T':
		prints(padd(state_mes_pos, pos_state), "���� : ������");
		order_message[0] = "���� ������ �̵�, ���� ��ɾ ����� �� �����ϴ�.";
		prints(padd(order_mes_pos[0], pos_order), order_message[0]);
		break;
	}


	// [ �����̽��ٷ� ���� ��ġ ]
	if (p_key_press && resource->spice >= 1) {

		// [ Ŀ�� �ڸ� P ]
		map[0][curr.row][curr.column] = 'P';
		check_friend[curr.row][curr.column] = 1;

		// [ Ŀ�� �ֺ� �ڸ� P ]
		for (int i = 0; i < 3; i++) {
			int b_row = curr.row + bigrow[i];
			int b_col = curr.column + bigcol[i];
			map[0][b_row][b_col] = 'P';
			check_friend[b_row][b_col] = 1;
		}

		// [ ���� �ڿ� �Ҹ� ]
		resource->spice -= 1;
		p_system_message("������ �Ǽ��Ͽ����ϴ�.");

		// [ �ʱ�ȭ ]
		clear_window(state_backbuf, state_pos, STATE_HEIGHT, STATE_WIDTH);
	}

	// [ ������ ��ġ�� �� ���� �� ]
	else if (p_key_press && resource->spice < 1) {
		p_system_message("�����ڿ��� �����Ͽ� �Ǽ��� �� �����ϴ�.");
		return_cursor = true;
	}


	// [ �����̽��ٷ� ���� ��ġ ]
	if (d_key_press && resource->spice >= 2) {

		// [ Ŀ�� �ڸ� P ]
		map[0][curr.row][curr.column] = 'D';
		check_friend[curr.row][curr.column] = 1;

		// [ Ŀ�� �ֺ� �ڸ� P ]
		for (int i = 0; i < 8; ++i) {  // 8���� �˻�
			int newRow = curr.row + pRow[i];
			int newCol = curr.column + pCol[i];
			if (frontbuf[newRow][newCol] == 'P' && check_friend[newRow][newCol] == 1) {
				map[0][newRow][newCol] = 'D';
			}
		}

		// [ ���� �ڿ� �Ҹ� ]
		resource->spice -= 2;
		p_system_message("���Ҹ� �Ǽ��Ͽ����ϴ�.");

		// [ �ʱ�ȭ ]
		clear_window(state_backbuf, state_pos, STATE_HEIGHT, STATE_WIDTH);
	}
	
	// [ ���Ҹ� ��ġ�� �� ���� �� ]
	else if (d_key_press && resource->spice < 2) {
		p_system_message("�����ڿ��� �����Ͽ� �Ǽ��� �� �����ϴ�.");
		return_cursor = true;
	}


	// [ �����̽��ٷ� â�� ��ġ ]
	if (g_key_press && resource->spice >= 4) {
		// [ Ŀ�� �ڸ� P ]
		map[0][curr.row][curr.column] = 'G';
		check_friend[curr.row][curr.column] = 1;

		// [ Ŀ�� �ֺ� �ڸ� P ]
		for (int i = 0; i < 8; ++i) {  // 8���� �˻�
			int newRow = curr.row + pRow[i];
			int newCol = curr.column + pCol[i];
			if (frontbuf[newRow][newCol] == 'P' && check_friend[newRow][newCol] == 1) {
				map[0][newRow][newCol] = 'G';
			}
		}

		// [ ���� �ڿ� �Ҹ� ]
		resource->spice -= 2;
		p_system_message("���Ҹ� �Ǽ��Ͽ����ϴ�.");

		// [ �ʱ�ȭ ]
		clear_window(state_backbuf, state_pos, STATE_HEIGHT, STATE_WIDTH);
	}
	
	// [ â�� ��ġ�� �� ���� �� ]
	else if (d_key_press && resource->spice < 4) {
		p_system_message("�����ڿ��� �����Ͽ� �Ǽ��� �� �����ϴ�.");
		return_cursor = true;
	}


	// [ �����̽��ٷ� ���� ��ġ ]
	if (b_b_key_press && resource->spice >= 4) {
		map[0][curr.row][curr.column] = 'B';
		check_friend[curr.row][curr.column] = 1;
		base_barr_check[curr.row][curr.column] = 2;

		// [ Ŀ�� �ֺ� �ڸ� P ]
		for (int i = 0; i < 8; ++i) {  // 8���� �˻�
			int newRow = curr.row + pRow[i];
			int newCol = curr.column + pCol[i];
			if (frontbuf[newRow][newCol] == 'P' && check_friend[newRow][newCol] == 1) {
				map[0][newRow][newCol] = 'B';
				base_barr_check[newRow][newCol] = 2;
			}
		}

		// [ ���� �ڿ� �Ҹ� ]
		resource->spice -= 4;
		p_system_message("������ �Ǽ��Ͽ����ϴ�.");

		// [ �ʱ�ȭ ]
		clear_window(state_backbuf, state_pos, STATE_HEIGHT, STATE_WIDTH);
	}
	
	// [ ������ ��ġ�� �� ���� �� ]
	else if (b_b_key_press && resource->spice < 4) {
		p_system_message("�����ڿ��� �����Ͽ� �Ǽ��� �� �����ϴ�.");
		return_cursor = true;
	}


	// [ �����̽��ٷ� ����ó ��ġ ]
	if (s_key_press && resource->spice >= 5) {
		map[0][curr.row][curr.column] = 'S';
		check_friend[curr.row][curr.column] = 1;
		shle_sold_check[curr.row][curr.column] = 1; // ����ó - 1, ���� - 2

		// [ Ŀ�� �ֺ� �ڸ� P ]
		for (int i = 0; i < 8; ++i) {  // 8���� �˻�
			int newRow = curr.row + pRow[i];
			int newCol = curr.column + pCol[i];
			if (frontbuf[newRow][newCol] == 'P' && check_friend[newRow][newCol] == 1) {
				map[0][newRow][newCol] = 'S';
				shle_sold_check[newRow][newCol] = 1;
			}
		}

		// [ ���� �ڿ� �Ҹ� ]
		resource->spice -= 5;
		p_system_message("����ó�� �Ǽ��Ͽ����ϴ�.");

		// [ �ʱ�ȭ ]
		clear_window(state_backbuf, state_pos, STATE_HEIGHT, STATE_WIDTH);
	}
	
	// [ ����ó�� ��ġ�� �� ���� �� ]
	else if (s_key_press && resource->spice < 5) {
		p_system_message("�����ڿ��� �����Ͽ� �Ǽ��� �� �����ϴ�.");
		return_cursor = true;
	}


}

// [ ESC Ű�� ������ �� ]
void state_esc(void) {
	// [ �ʱ�ȭ ]
	reset_order_messages();
	clear_window(state_backbuf, state_pos, STATE_HEIGHT, STATE_WIDTH);
	clear_window(order_backbuf, order_pos, ORDER_HEIGHT, ORDER_WIDTH);
}

// [ �Ϻ����� ���� HŰ�� ������ �� ]
void press_h(RESOURCE* resource, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH], CURSOR cursor, OBJECT_SAMPLE* f_hav_obj, int hav_count, OBJECT_SAMPLE* havs[MAX_HAV]) {
	POSITION curr = cursor.current;

	// [ �Ʊ� ���� --> �Ϻ����� ���� ] [ �Ϻ����� ���� ��� 5, �α��� 5 ]
	if (map[0][curr.row][curr.column] == 'B' && check_friend[curr.row][curr.column] == 1) {

		// [ �Ϻ����͸� ������ �� ���� �� ]
		if (resource->spice >= 5 && resource->population + 5 <= resource->population_max) {

			// [ �Ϻ����͸� �߰��� �� ���� ���� ���� ]
			if (hav_count < MAX_HAV) {
				OBJECT_SAMPLE new_hav = { 
					{ 14, 2 }, { 14, 2 }, 'H', 2000, 2000, 
					COLOR_DEFAULT + 16, 70, 0, 0, 0 
				};

				// [ �Ϻ����͸� �迭�� �߰� ]
				havs[0] = f_hav_obj;
				havs[hav_count++] = &new_hav;

				// [ �ʿ� �Ϻ����� ��ġ �ݿ� ]
				map[1][new_hav.pos.row][new_hav.pos.column] = 'H';
				check_friend[new_hav.pos.row][new_hav.pos.column] = 1;

				// [ �ý��� �޽��� �߰� ]
				p_system_message("�Ϻ����Ͱ��� ����Ǿ����ϴ�.");

				// [ �ڿ� ����, �α��� ���� ]
				resource->spice -= 5;
				resource->population += 5;
			}
			else {
				p_system_message("�Ϻ����� �ִ� ���� ������ �ʰ��Ͽ� �� �̻� ������ �� �����ϴ�.");
			}

		}
		// [ �Ϻ����͸� ������ �� ���� �� ]
		else {
			if (resource->spice < 5) {
				// [ �ý��� �޽��� �߰� ]
				p_system_message("���� �ڿ��� �����Ͽ� �Ϻ����͸� ������ �� �����ϴ�.");
			}
			else if (resource->population + 5 > resource->population_max) {
				// [ �ý��� �޽��� �߰� ]
				p_system_message("�α����� �����Ͽ� �Ϻ����͸� ������ �� �����ϴ�.");
			}
		}
	}
	// [ �Ʊ� �Ϻ����� ��Ȯ �ൿ ]
	else if (map[0][curr.row][curr.column] == '1' || map[0][curr.row][curr.column] == '2' || map[0][curr.row][curr.column] == '3' || \
		map[0][curr.row][curr.column] == '4' || map[0][curr.row][curr.column] == '5' || map[0][curr.row][curr.column] == '6' || \
		map[0][curr.row][curr.column] == '7' || map[0][curr.row][curr.column] == '8' || map[0][curr.row][curr.column] == '9') {
		// [ �Ϻ����� �������� �����̽� ��ġ�� ���� ]
		POSITION new_dest = { curr.row, curr.column };
		
	}
}

// [ B (��ġ ���� �ǹ� ���) Ű�� ������ �� ]
void press_b(RESOURCE* resource, CURSOR cursor, int check_friend[MAP_HEIGHT][MAP_WIDTH]) {

	// [ ���� ���� ]
	POSITION curr = cursor.current;
	POSITION pos_order = { 0, 1 };
	char cell = backbuf[curr.row][curr.column];

	// [ �ʱ�ȭ ]
	reset_order_messages();
	clear_window(order_backbuf, order_pos, ORDER_HEIGHT, ORDER_WIDTH);

	switch (cell) {

	// [ �縷 ���� (�� ����) ]
	case ' ':
		prints(padd(order_mes_pos[0], pos_order), "������ ��ġ�Ͻðڽ��ϱ�? (P / ESC)");
		break;

	// [ ���� ]
	case 'P':
		if (check_friend[curr.row][curr.column] == 1) {
			// [ ��ɾ� ��� ]
			char* messages[] = {
			"���Ҹ� ��ġ�Ͻðڽ��ϱ�?   (D / ESC)",
			"â�� ��ġ�Ͻðڽ��ϱ�?   (G / ESC)",
			"������ ��ġ�Ͻðڽ��ϱ�?   (B / ESC)",
			"����ó�� ��ġ�Ͻðڽ��ϴ�? (S / ESC)"
			};

			for (int i = 0; i < 4; i++) {
				order_message[i] = messages[i];
				prints(padd(order_mes_pos[i], pos_order), order_message[i]);
			}
		}
		break;
	}
}

// [ P (���� ��ġ) Ű�� ������ �� ]
void press_p(void) {
	POSITION pos_order = { 0, 1 };
	clear_window(order_backbuf, order_pos, ORDER_HEIGHT, ORDER_WIDTH);
	prints(padd(order_mes_pos[0], pos_order), "�����̽��ٸ� ���� ���� ��ġ�� �Ϸ��ϼ���.");
}

// [ D (���� ��ġ) Ű�� ������ �� ]
void press_d(void) {
	POSITION pos_order = { 0, 1 };
	clear_window(order_backbuf, order_pos, ORDER_HEIGHT, ORDER_WIDTH);
	prints(padd(order_mes_pos[0], pos_order), "�����̽��ٸ� ���� ���� ��ġ�� �Ϸ��ϼ���.");
}

// [ G (â�� ��ġ) Ű�� ������ �� ]
void press_g(void) {
	POSITION pos_order = { 0, 1 };
	clear_window(order_backbuf, order_pos, ORDER_HEIGHT, ORDER_WIDTH);
	prints(padd(order_mes_pos[0], pos_order), "�����̽��ٸ� ���� â�� ��ġ�� �Ϸ��ϼ���.");
}

// [ B - B (���� ��ġ) Ű�� ������ �� ]
void press_b_b(void) {
	POSITION pos_order = { 0, 1 };
	clear_window(order_backbuf, order_pos, ORDER_HEIGHT, ORDER_WIDTH);
	prints(padd(order_mes_pos[0], pos_order), "�����̽��ٸ� ���� ���� ��ġ�� �Ϸ��ϼ���.");
}

// [ S (����ó ��ġ) Ű�� ������ �� ]
void press_s(void) {
	POSITION pos_order = { 0, 1 };
	clear_window(order_backbuf, order_pos, ORDER_HEIGHT, ORDER_WIDTH);
	prints(padd(order_mes_pos[0], pos_order), "�����̽��ٸ� ���� ����ó ��ġ�� �Ϸ��ϼ���.");
}

// [ S (���� ����) Ű�� ������ �� ]
void press_s_s(CURSOR cursor, RESOURCE* resource, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	POSITION curr = cursor.current;
	if (map[0][curr.row][curr.column] == 'B' && base_barr_check[curr.row][curr.column] == 2 && \
		resource->spice >= 1 && resource->population + 1 <= resource->population_max) {
		
	}
	else if (resource->spice < 1) {
		p_system_message("���� �ڿ��� �����Ͽ� �Ϻ����͸� ������ �� �����ϴ�.");
	}
	else if (resource->population + 1 > resource->population_max) {
		p_system_message("�α����� �����Ͽ� �Ϻ����͸� ������ �� �����ϴ�.");
	}
}

// [ F (������ ����) Ű�� ������ �� ]
void press_f() {

}

// [ M (�̵�) Ű�� ������ �� ]
void press_m() {

}

// [ P (����) Ű�� ������ �� ]
void press_p_p() {

}

