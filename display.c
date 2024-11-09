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
void display_cursor(CURSOR cursor, int check_friend[MAP_HEIGHT][MAP_WIDTH]);
void set_cursor_color(POSITION pos, char ch);
void highlight_P_area(POSITION pos, int check_friend[MAP_HEIGHT][MAP_WIDTH]);
void clear_cursor_area(POSITION pos, int check_friend[MAP_HEIGHT][MAP_WIDTH]);
void reset_to_original_color(POSITION pos);

void display_state_map(char state_map[STATE_HEIGHT][STATE_WIDTH]);
void state_project(char src[STATE_HEIGHT][STATE_WIDTH], char dest[STATE_HEIGHT][STATE_WIDTH]);

void sysmes_project(char src[SYSMES_HEIGHT][SYSMES_WIDTH], char dest[SYSMES_HEIGHT][SYSMES_WIDTH]);
void display_sysmes_map(char sysmes_map[SYSMES_HEIGHT][SYSMES_WIDTH]);
void clear_sysmes_frame(char sysmes_map[SYSMES_HEIGHT][SYSMES_WIDTH]);

void order_project(char src[ORDER_HEIGHT][ORDER_WIDTH], char dest[ORDER_HEIGHT][ORDER_WIDTH]);
void display_order_map(char order_map[ORDER_HEIGHT][ORDER_WIDTH]);

void reset_order_messages();
void clear_window(char buffer[][STATE_WIDTH], POSITION base_pos, int height, int width);

// [ ���� ���� ]
char save_name_for_order[2];
char* order_message[4];
char* save_system_message[7] = { NULL };
char* send_system_message[1];

const int dRow[] = { -1, -1, 0, 1, 1, 1, 0, -1 };
const int dCol[] = { 0, -1, -1, -1, 0, 1, 1, 1 };

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

// ==== [ �Լ��� �ǹ� ��� ] ==== //

// [ �Ʊ� ���� ��� �Լ� ]
void p_f_base(OBJECT_BUILDING fb, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]) {
	POSITION fb_num[] = { fb.pos1, fb.pos2, fb.pos3, fb.pos4 };
	for (int i = 0; i < 4; i++) {
		map[0][fb_num[i].row][fb_num[i].column] = fb.repr;
		check_friend[fb_num[i].row][fb_num[i].column] = 1;
	}
}

// [ ���� ���� ��� �Լ� ]
void p_e_base(OBJECT_BUILDING eb, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]) {
	POSITION eb_num[] = { eb.pos1, eb.pos2, eb.pos3, eb.pos4 };
	for (int i = 0; i < 4; i++) {
		map[0][eb_num[i].row][eb_num[i].column] = eb.repr;
		check_friend[eb_num[i].row][eb_num[i].column] = 2;
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


// =================================== [ DISPLAY ] ======================================= //

// [ Display ���� �Լ� ]
void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor,
	char state_map[STATE_HEIGHT][STATE_WIDTH],
	char sysmes_map[SYSMES_HEIGHT][SYSMES_WIDTH],
	char order_map[ORDER_HEIGHT][ORDER_WIDTH],
	int check_friend[MAP_HEIGHT][MAP_WIDTH])
{
	display_resource(resource);
	display_map(map, check_friend);
	display_cursor(cursor, check_friend);
	p_building(map, check_friend);
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
			// [ ���� ]
			case 'B':  
				if (check_friend[i][j] == 1) {
					color = COLOR_DEFAULT + 16;  // �Ʊ� ���� (�Ķ��� ���) 
				}
				else if (check_friend[i][j] == 2) {
					color = COLOR_DEFAULT + 64;  // ���� ���� (������ ���)
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

// [ Ŀ�� ���� ���� �Լ� ]
void set_cursor_color(POSITION pos, char ch) {
	printc(padd(map_pos, pos), ch, CURSOR_COLOR);
}

// [ Ŀ�� �ܻ���� �ذ� �Լ� ]
void reset_to_original_color(POSITION pos) {
	int color = colorbuf[pos.row][pos.column];
	char ch = frontbuf[pos.row][pos.column];
	printc(padd(map_pos, pos), ch, color);
}

// [ Ŀ�� �ܻ���� �ذ� �Լ� (����(P) ]
void clear_cursor_area(POSITION pos, int check_friend[MAP_HEIGHT][MAP_WIDTH]) {
	if (frontbuf[pos.row][pos.column] == 'P' && check_friend[pos.row][pos.column] == 1) {
		reset_to_original_color(pos);  // ���� ��ġ
		for (int i = 0; i < 8; ++i) {  // 8���� �˻�
			int newRow = pos.row + dRow[i];
			int newCol = pos.column + dCol[i];
			if (frontbuf[newRow][newCol] == 'P' && check_friend[pos.row][pos.column] == 1) {
				POSITION newPos = { newRow, newCol };
				reset_to_original_color(newPos);
			}
		}
	}
	else {
		reset_to_original_color(pos);  // 'P'�� �ƴ� ��� ���� ��ġ�� ����
	}
}

// [ 'P' ���� ���� �����ϴ� �Լ� ]
void highlight_P_area(POSITION pos, int check_friend[MAP_HEIGHT][MAP_WIDTH]) {

	// ���� ��ġ�� 'P'��� 8���� Ž��
	if (frontbuf[pos.row][pos.column] == 'P' && check_friend[pos.row][pos.column] == 1) {
		set_cursor_color(pos, frontbuf[pos.row][pos.column]);  // ���� ��ġ
		for (int i = 0; i < 8; ++i) {  // 8���� �˻�
			int newRow = pos.row + dRow[i];
			int newCol = pos.column + dCol[i];
			if (frontbuf[newRow][newCol] == 'P' && check_friend[pos.row][pos.column] == 1) {
				POSITION newPos = { newRow, newCol };
				set_cursor_color(newPos, frontbuf[newRow][newCol]);
			}
		}
	}
	else {
		// 'P'�� �ƴϸ� ���� ��ġ�� Ŀ�� ������ ����
		printc(padd(map_pos, pos), frontbuf[pos.row][pos.column], COLOR_BLACK);
	}
}

// [ ���� Ŀ�� ��� �Լ� ]
void display_cursor(CURSOR cursor, int check_friend[MAP_HEIGHT][MAP_WIDTH]) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;
	
	// [ Ŀ�� ���� ������ �ǵ����� & ������ ���� ������ �ǵ����� ] 
	clear_cursor_area(prev, check_friend); 

	// [ Ŀ�� ��� & ���� �� �ٲٱ� ]
	highlight_P_area(curr, check_friend);
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
	for (int i = 0; i < 2; i++) save_name_for_order[i] = ' ';
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
void clear_sysmes_frame(char sysmes_map[SYSMES_HEIGHT][SYSMES_WIDTH]) {
	for (int i = 1; i < SYSMES_HEIGHT - 1; i++) {
		for (int j = 1; j < SYSMES_WIDTH - 1; j++) {
			POSITION pos = { i, j };
			printc(padd(sysmes_pos, pos), sysmes_backbuf[i][j], COLOR_BLACK);
		}
	}
}

// [ �ý��� �޽��� ��� ]
void p_system_message(char* new_message, char sysmes_map[SYSMES_HEIGHT][SYSMES_WIDTH]) {
	// [ �޽����� ���� �� ĭ�� �б� ]
	for (int i = 6; i > 0; i--) {
		save_system_message[i] = save_system_message[i - 1];
	}
	save_system_message[0] = new_message;

	// [ �ý��� �޽��� �ʱ�ȭ �� �޽��� ��� ]
	clear_sysmes_frame(sysmes_map);
	POSITION pos_system = { 0, 1 };
	for (int i = 0; i < 7; i++) {
		if (save_system_message[i] != NULL) {
			prints(padd(system_mes_pos[i], pos_system), save_system_message[i]);
		}
	}
}


/* ======================================= [ ���� Ű ��ɾ� �Լ� ] ======================================= */

// [ �����̽� �� Ű�� ������ �� ]
void state_spacebar(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	char state_map[STATE_HEIGHT][STATE_WIDTH], CURSOR cursor,
	int check_friend[MAP_HEIGHT][MAP_WIDTH]) {

	// [ ���� ���� ]
	POSITION curr = cursor.current;
	POSITION pos_state = { 0, 1 };
	POSITION pos_order = { 0, 1 };
	char cell = backbuf[curr.row][curr.column];

	// [ �ʱ�ȭ ]
	reset_order_messages();
	clear_window(state_backbuf, state_pos, STATE_HEIGHT, STATE_WIDTH);
	clear_window(order_backbuf, order_pos, ORDER_HEIGHT, ORDER_WIDTH);

	// [ ���� �� ���â �޽��� ��� ]
	switch (cell) {
	case ' ':
		prints(padd(state_mes_pos, pos_state), "(��)���� : �縷 ����");
		break;

	case 'B':
		if (check_friend[curr.row][curr.column] == 1) {
			save_name_for_order[0] = 'B';
			save_name_for_order[1] = 'F';
			prints(padd(state_mes_pos, pos_state), "�ǹ� : �Ʊ� ����");
			prints(padd(state_mes2_pos, pos_state), "�Ϻ����͸� ������ �� �ִ�.");
			order_message[0] = "�Ϻ����� ���� (H / ESC)";
			prints(padd(order_mes_pos[0], pos_order), order_message[0]);
		}
		else if (check_friend[curr.row][curr.column] == 2) {
			save_name_for_order[0] = 'B';
			save_name_for_order[1] = 'E';
			prints(padd(state_mes_pos, pos_state), "�ǹ� : ���� ����");
		}
		break;

	case 'P':
		if (check_friend[curr.row][curr.column] == 1) {
			save_name_for_order[0] = 'P';
			save_name_for_order[1] = 'F';
			prints(padd(state_mes_pos, pos_state), "�ǹ� : �Ʊ� ����");
			prints(padd(state_mes2_pos, pos_state), "����, â��, ����, ����ó�� ��ġ�� �� �ִ�.");
			const char* messages[] = {
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
		else if (check_friend[curr.row][curr.column] == 2) {
			save_name_for_order[0] = 'P';
			save_name_for_order[1] = 'E';
			prints(padd(state_mes_pos, pos_state), "�ǹ� : ���� ����");
		}
		break;

	case '1': case '2': case '3': case '4': case '5':
	case '6': case '7': case '8': case '9':
		prints(padd(state_mes_pos, pos_state), "�ǹ� : �����̽�");
		break;

	case 'R':
		prints(padd(state_mes_pos, pos_state), "�ǹ� : ����");
		break;

	case 'H':
		if (colorbuf[curr.row][curr.column] == COLOR_DEFAULT + 16) {
			prints(padd(state_mes_pos, pos_state), "���� : �Ʊ� �Ϻ�����");
		}
		else if (colorbuf[curr.row][curr.column] == COLOR_DEFAULT + 64) {
			prints(padd(state_mes_pos, pos_state), "���� : ���� �Ϻ�����");
		}
		break;

	case 'W':
		prints(padd(state_mes_pos, pos_state), "���� : �����");
		break;
	}
}

// [ ESC Ű�� ������ �� ]
void state_esc(char state_map[STATE_HEIGHT][STATE_WIDTH], char order_map[ORDER_HEIGHT][ORDER_WIDTH]) {
	// [ �ʱ�ȭ ]
	reset_order_messages();
	clear_window(state_backbuf, state_pos, STATE_HEIGHT, STATE_WIDTH);
	clear_window(order_backbuf, order_pos, ORDER_HEIGHT, ORDER_WIDTH);
}

// [ �Ϻ����� ���� HŰ�� ������ �� ]
void press_h(RESOURCE *resource, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char sysmes_map[SYSMES_HEIGHT][SYSMES_WIDTH],
	int check_friend[MAP_HEIGHT][MAP_WIDTH]) {
	// [ �Ʊ� ���� --> �Ϻ����� ���� ] [ �Ϻ����� ���� ��� 5, �α��� 5 ]
	if (save_name_for_order[0] == 'B' && save_name_for_order[1] == 'F') {

		// [ �Ϻ����͸� ������ �� ���� �� ]
		if (resource->spice >= 5 && resource->population + 5 <= resource->population_max) {

			// [ �Ϻ����� ���� ] 
			OBJECT_SAMPLE f_hav2_obj = { { 14, 2 }, { MAP_HEIGHT - 2, MAP_WIDTH - 2 }, 'H', 2000, 2000, COLOR_DEFAULT + 16 };
			map[1][f_hav2_obj.pos.row][f_hav2_obj.pos.column] = 'H';
			check_friend[f_hav2_obj.pos.row][f_hav2_obj.pos.column] = 1;

			// [ �ý��� �޽��� �߰� ]
			add_system_message("�Ϻ����Ͱ� 1�Ⱑ ����Ǿ����ϴ�.", sysmes_map);
				
			// [ �ڿ�����, �α��� ���� ]
			resource->spice -= 5;
			resource->population += 5;
				
		}
		// [ �Ϻ����͸� ������ �� ���� �� ]
		else {
			if (resource->spice < 5) {
				// [ �ý��� �޽��� �߰� ]
				add_system_message("���� �ڿ��� �����Ͽ� �Ϻ����͸� ������ �� �����ϴ�.", sysmes_map);
			}
			else if (resource->population  + 5 > resource->population_max) {
				// [ �ý��� �޽��� �߰� ]
				add_system_message("�α����� �����Ͽ� �Ϻ����͸� ������ �� �����ϴ�.", sysmes_map);
		}
	}
}

// [ BŰ�� ������ �� ]
void press_b(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char sysmes_map[SYSMES_HEIGHT][SYSMES_WIDTH]) {
	
}