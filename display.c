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

int colorbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 }; // �ǹ�, ���� �� Ȯ��
 
char state_backbuf[STATE_HEIGHT][STATE_WIDTH] = { 0 };
char state_frontbuf[STATE_HEIGHT][STATE_WIDTH] = { 0 };

char sysmes_backbuf[SYSMES_HEIGHT][SYSMES_WIDTH] = { 0 };
char sysmes_frontbuf[SYSMES_HEIGHT][SYSMES_WIDTH] = { 0 };

char order_backbuf[ORDER_HEIGHT][ORDER_WIDTH] = { 0 };
char order_frontbuf[ORDER_HEIGHT][ORDER_WIDTH] = { 0 };

// == [ �Լ� ���� ] ==
void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor, int check_friend[MAP_HEIGHT][MAP_WIDTH], bool big_cursor, 
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]); // Ŀ�� ��� �Լ�
void set_cursor_color(POSITION pos, char ch); // Ŀ�� �� ���ϱ�
void highlight_P_area(POSITION pos, int check_friend[MAP_HEIGHT][MAP_WIDTH], bool big_cursor, 
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]); // �������� Ŀ�� �÷��� �� Ŀ�� ũ�� ���ϱ�
void clear_cursor_area(POSITION pos, int check_friend[MAP_HEIGHT][MAP_WIDTH], bool big_cursor); // Ŀ�� �ܻ� �ذ�
void reset_to_original_color(POSITION pos); // Ŀ�� �ܻ� �ذ�

void display_state_map(char state_map[STATE_HEIGHT][STATE_WIDTH]); // ����â �׸��� �Լ�
void state_project(char src[STATE_HEIGHT][STATE_WIDTH], char dest[STATE_HEIGHT][STATE_WIDTH]); // ����â �⺻Ʋ

void sysmes_project(char src[SYSMES_HEIGHT][SYSMES_WIDTH], char dest[SYSMES_HEIGHT][SYSMES_WIDTH]); // �ý���â �⺻Ʋ �Լ�
void display_sysmes_map(char sysmes_map[SYSMES_HEIGHT][SYSMES_WIDTH]); // �ý���â �׸��� �Լ�
void clear_sysmes_frame(); // �ý��� �޽��� �ʱ�ȭ �Լ�

void order_project(char src[ORDER_HEIGHT][ORDER_WIDTH], char dest[ORDER_HEIGHT][ORDER_WIDTH]); // ���â �⺻Ʋ �Լ�
void display_order_map(char order_map[ORDER_HEIGHT][ORDER_WIDTH]); // ���â �׸��� �Լ�

void reset_order_messages(); // ��� �޽��� �ʱ�ȭ �Լ�
void clear_window(char buffer[][STATE_WIDTH], POSITION base_pos, int height, int width); // ���� ���â �ʱ�ȭ


// [ ���� ���� ]
char save_name_for_order[2];
char* order_message[4]; // ��� �޽��� 
char* save_system_message[7] = { NULL }; // �ý��� �޽��� ����
char* send_system_message[1]; // �ý��� �޽��� 

const int pRow[] = { -1, -1, 0, 1, 1, 1, 0, -1 }; // 8���� Ȯ��
const int pCol[] = { 0, -1, -1, -1, 0, 1, 1, 1 }; // 8���� Ȯ��
const int bigrow[] = {1, 1, 0}; 
const int bigcol[] = {0, 1, 1};

int base_barr_check[MAP_HEIGHT][MAP_WIDTH] = { 0 }; // ���� - 1, ���� - 2 ����
int shle_sold_check[MAP_HEIGHT][MAP_WIDTH] = { 0 }; // ����ó - 1, ���� - 2 ����
int frem_fight_fact_check[MAP_HEIGHT][MAP_WIDTH] = { 0 }; // ���� - 1, ������ - 2, ���� - 3 ����

bool return_cursor = false; // Ŀ�� �ǵ�����

// [ ���� �ҷ����� ]
extern OBJECT_SAMPLE* sold[MAX_SOLD]; // ���� �迭
extern int sold_count; // ���� ����
extern int selected_sold; // ���� ���õǾ����� Ȯ��

// [ ������ �ҷ����� ]
extern OBJECT_SAMPLE* frem[MAX_FREM]; // ������ �迭
extern int frem_count; // ������ ����
extern int selected_frem; // ������ ���õǾ����� Ȯ��


// =================================== [ �ǹ� ] ======================================= //

// ==== [ ����ü�� �ǹ� ���� ] ==== //

// [ �Ʊ� ���� BASE ]
OBJECT_BUILDING f_base = {
	.pos1 = {16, 1},
	.pos2 = {16, 2},
	.pos3 = {15, 1},
	.pos4 = {15, 2},
	.repr = 'B',
	.layer = 0,
	.hp = 50
};

// [ ���� ���� BASE ]
OBJECT_BUILDING e_base = {
	.pos1 = {1, 57},
	.pos2 = {1, 58},
	.pos3 = {2, 57},
	.pos4 = {2, 58},
	.repr = 'B',
	.layer = 0,
	.hp = 50
};

// [ �ʱ� �����̽� ������ �Ʊ� ]
OBJECT_BUILDING start_spice1 = {
	.pos1 = {12, 1},
	.repr = '5',
	.layer = 0
};

// [ �ʱ� �����̽� ������ ���� ]
OBJECT_BUILDING start_spice2 = {
	.pos1 = {5, 58},
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
void p_start_spice1(OBJECT_BUILDING ss, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	POSITION ss_num[] = { ss.pos1 };
	map[0][ss_num[0].row][ss_num[0].column] = ss.repr;
}

void p_start_spice2(OBJECT_BUILDING ss, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	POSITION ss_num[] = { ss.pos1 };
	map[0][ss_num[0].row][ss_num[0].column] = ss.repr;
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

OBJECT_BUILDING* SPICE[MAX_SPICE]; // �����̽� �迭
int spi_count = 2; // �����̽� ���� 

OBJECT_BUILDING* DOR[MAX_DOR]; // ���� �迭
int dor_count = 0; // ���� ����

OBJECT_BUILDING* GAR[MAX_GAR]; // â�� �迭
int gar_count = 0; // â�� ����

OBJECT_BUILDING* BAR[MAX_BAR]; // ���� �迭
int bar_count = 0; // ���� ����

OBJECT_BUILDING* SHE[MAX_SHE]; // ����ó �迭
int she_count = 0; // ����ó ����



// [ ��� �ǹ� ��� �Լ� ]
void p_building(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]) {
	p_f_base(f_base, map, check_friend); // �Ʊ� ����
	p_e_base(e_base, map, check_friend); // ���� ����
	p_f_plate(f_plate, map, check_friend); // �Ʊ� ����
	p_e_plate(e_plate, map, check_friend); // ���� ����
	p_start_spice1(start_spice1, map); // �ʱ� �����̽� ������ �Ʊ� 
	p_start_spice1(start_spice2, map); // �ʱ� �����̽� ������ ���� 
	p_rock_1(rock_1, map); // ���� 1 
	p_rock_2(rock_2, map); // ���� 2
	p_rock_3(rock_3, map); // ���� 3
	p_rock_4(rock_4, map); // ���� 4
	SPICE[0] = &start_spice1;
	SPICE[1] = &start_spice2;
}


// [ ���� �ǹ� ]
OBJECT_BUILDING* E_DOR[MAX_DOR]; // ���� ���� �迭
int e_dor_count = 1; // ���� ���� ����

OBJECT_BUILDING* E_GAR[MAX_GAR]; // ���� â�� �迭
int e_gar_count = 1; // ���� â�� ����

OBJECT_BUILDING* ARE[MAX_ARE]; // ���� ������ �迭
int are_count = 1; // ���� ������ ����

OBJECT_BUILDING* FAC[MAX_FAC]; // ���� ���� �迭
int fac_count = 1; // ���� ���� ����



/* -=-=-=-=-=-=-=-=[ ���ڳ� ]-=-=-=-=-=-=-=-= -*/

// [ ���ڳ� ���� 1 ]
OBJECT_BUILDING e_dor1 = {
	 .pos1 = { 1, 50 },
	 .pos2 = { 1, 51 },
	 .pos3 = { 2, 50 },
	 .pos4 = { 2, 51 },
	 .repr = 'D',
	 .hp = 10,
	 .layer = 0
};

// [ ���ڳ� â�� 1 ]
OBJECT_BUILDING e_gar1 = {
	 .pos1 = { 1, 47 },
	 .pos2 = { 1, 46 },
	 .pos3 = { 2, 47 },
	 .pos4 = { 2, 46 },
	 .repr = 'G',
	 .hp = 10,
	 .layer = 0
};

// [ ���ڳ� ������ 1 ]
OBJECT_BUILDING e_are1 = {
	 .pos1 = { 5, 51 },
	 .pos2 = { 5, 52 },
	 .pos3 = { 6, 51 },
	 .pos4 = { 6, 52 },
	 .repr = 'A',
	 .hp = 15,
	 .layer = 0
};


// [ ���ڳ� ���� 1 ]
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
	set_color(COLOR_DEFAULT);// �ڿ� ���
	gotoxy(resource_pos);
	printf("                                                      "); // ���� ���ڿ� �����
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
				if (check_friend[i][j] == 1) {
					color = 180;
				}
				else if (check_friend[i][j] == 2) {
					color = COLOR_DEFAULT + 64;
				}
				break;

			// [ â�� ]
			case 'G':
				if (check_friend[i][j] == 1) {
					color = 180;
				}
				else if (check_friend[i][j] == 2) {
					color = COLOR_DEFAULT + 64;
				}
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
				color = COLOR_DEFAULT + 64;
				break;
			
			// [ ����, ������, ���� ]
			case 'F':
				// [ ���� - 1 ]
				if (frem_fight_fact_check[i][j] == 1) {
					color = COLOR_DEFAULT + 64;
				}
				// [ ������ - 2 ]
				else if (frem_fight_fact_check[i][j] == 2) {
					color = 60; // �� ���� ������
				}
				// [ ���� - 3 ]
				else if (frem_fight_fact_check[i][j] == 3) {
					color = COLOR_DEFAULT + 64;
				}

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

			// [ ������ ]
			case 'T':
				color = COLOR_DEFAULT + 64;
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

// [ �ý��� �޽��� ���� ��� �Լ� ]
void p_system_message(char* new_message) {
	// �迭�� ������ �޽��� ���� (���� �Ҵ�� ���)
	if (save_system_message[6] != NULL) {
		free(save_system_message[6]);
		save_system_message[6] = NULL;
	}

	// �޽����� ���� �� ĭ�� �б�
	for (int i = 6; i > 0; i--) {
		save_system_message[i] = save_system_message[i - 1];
	}

	// �� �޽��� �߰� (������ ����)
	save_system_message[0] = malloc(strlen(new_message) + 1);
	if (save_system_message[0] != NULL) {
		strncpy_s(save_system_message[0], strlen(new_message) + 1, new_message, strlen(new_message));
		save_system_message[0][strlen(new_message)] = '\0'; // �� ���� ����
	}

	// �ý��� �޽��� �ʱ�ȭ �� �޽��� ���
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
	bool b_b_key_press, bool s_key_press, OBJECT_SAMPLE* havs[MAX_HAV], bool* space_key_press) {

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
			order_message[0] = "�̵�, ���� ��ɾ ����� �� �����ϴ�.";
			prints(padd(order_mes_pos[0], pos_order), order_message[0]);
		}

		break;


	/* ===== [����] ====== */
	// [ �Ϻ����� ]
	case 'H':
		for (int i = 0; i < MAX_HAV; i++) {
			if (havs[i] != NULL && havs[i]->pos.row == curr.row && havs[i]->pos.column == curr.column && colorbuf[curr.row][curr.column] == COLOR_DEFAULT + 16) {
				char mes_buf[100];
				// [ ���� �޽��� ]
				prints(padd(state_mes_pos, pos_state), "���� : �Ʊ� �Ϻ�����");
				sprintf_s(mes_buf, sizeof(mes_buf), "ü��: %d, ���ݷ�: 0, �̵��ֱ�: 2��, �þ�: 0", havs[i]->hp);
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
		}

		break;

	// [ ����� ]
	case 'W':
		prints(padd(state_mes_pos, pos_state), "���� : �����");
		*space_key_press = false;
		break;

	// [ ������ ]
	case 'T':
		prints(padd(state_mes_pos, pos_state), "���� : ������");
		order_message[0] = "�̵�, ���� ��ɾ ����� �� �����ϴ�.";
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
		int input_curr[4][2] = { 0 };

		// [ �ʱⰪ ���� ]
		int minRow = curr.row;
		int minCol = curr.column;

		// [ ���� ��ġ ���� 8���� �˻� �迭 ]
		int dRow[9] = { 0, -1, -1, -1, 0, 1, 1, 1, 0 }; // ���� ��ġ ���� (8����)
		int dCol[9] = { 0, -1, 0, 1, 1, 1, 0, -1, -1 };

		for (int i = 0; i < 9; ++i) {
			int checkRow = curr.row + dRow[i];
			int checkCol = curr.column + dCol[i];

			// [ P���� Ȯ�� ]
			if (map[0][checkRow][checkCol] == 'P') {
				// ���� ���� �� �Ǵ� ���� �࿡�� ���� ���� ��ġ ����
				if (checkRow < minRow || (checkRow == minRow && checkCol < minCol)) {
					minRow = checkRow;
					minCol = checkCol;
				}
			}
		}

		// [ ���� ���� P�� ��ġ�� input_curr[0]�� ���� ]
		input_curr[0][0] = minRow;
		input_curr[0][1] = minCol;

		// [ pos2, pos3, pos4 ���� ]
		input_curr[1][0] = minRow;        // ���� ������
		input_curr[1][1] = minCol + 1;

		input_curr[2][0] = minRow + 1;    // �Ʒ��� ����
		input_curr[2][1] = minCol;

		input_curr[3][0] = minRow + 1;    // �Ʒ��� ������
		input_curr[3][1] = minCol + 1;

		// [ �ʰ� ���õ� ������ ������Ʈ ]
		for (int i = 0; i < 4; i++) {
			map[0][input_curr[i][0]][input_curr[i][1]] = 'D';
			check_friend[input_curr[i][0]][input_curr[i][1]] = 1;
		}

		if (dor_count < MAX_DOR) {
			OBJECT_BUILDING* new_dor = (OBJECT_BUILDING*)malloc(sizeof(OBJECT_BUILDING));
			if (!new_dor) {
				p_system_message("�޸� �Ҵ� ����");
				return;
			}

			// [ ���ο� ���� ]
			*new_dor = (OBJECT_BUILDING){
				{ input_curr[0][0], input_curr[0][1] },
				{ input_curr[1][0], input_curr[1][1] },
				{ input_curr[2][0], input_curr[2][1] },
				{ input_curr[3][0], input_curr[3][1] },
				'D', 0, 10
			};

			// [ �迭�� �߰� ]
			DOR[dor_count] = new_dor;
			dor_count++;

			// [ ���� �ڿ� �Ҹ� ]
			resource->spice -= 2;
			resource->population_max += 10;
			p_system_message("���Ҹ� �Ǽ��Ͽ����ϴ�.");
			char mes[100];
			sprintf_s(mes, sizeof(mes), "���� �Ǽ� �� �ִ� �α���: %d", resource->population_max);
			p_system_message(mes);
		}
		else {
			p_system_message("���Ҹ� ���̻� �Ǽ��� �� �����ϴ�.");
		}

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
		int input_curr[4][2] = { 0 };

		// [ �ʱⰪ ���� ]
		int minRow = curr.row;
		int minCol = curr.column;

		// [ ���� ��ġ ���� 8���� �˻� �迭 ]
		int dRow[9] = { 0, -1, -1, -1, 0, 1, 1, 1, 0 }; // ���� ��ġ ���� (8����)
		int dCol[9] = { 0, -1, 0, 1, 1, 1, 0, -1, -1 };

		for (int i = 0; i < 9; ++i) {
			int checkRow = curr.row + dRow[i];
			int checkCol = curr.column + dCol[i];

			// [ P���� Ȯ�� ]
			if (map[0][checkRow][checkCol] == 'P') {
				// ���� ���� �� �Ǵ� ���� �࿡�� ���� ���� ��ġ ����
				if (checkRow < minRow || (checkRow == minRow && checkCol < minCol)) {
					minRow = checkRow;
					minCol = checkCol;
				}
			}
		}

		// [ ���� ���� P�� ��ġ�� input_curr[0]�� ���� ]
		input_curr[0][0] = minRow;
		input_curr[0][1] = minCol;

		// [ pos2, pos3, pos4 ���� ]
		input_curr[1][0] = minRow;        // ���� ������
		input_curr[1][1] = minCol + 1;

		input_curr[2][0] = minRow + 1;    // �Ʒ��� ����
		input_curr[2][1] = minCol;

		input_curr[3][0] = minRow + 1;    // �Ʒ��� ������
		input_curr[3][1] = minCol + 1;

		// [ �ʰ� ���õ� ������ ������Ʈ ]
		for (int i = 0; i < 4; i++) {
			map[0][input_curr[i][0]][input_curr[i][1]] = 'G';
			check_friend[input_curr[i][0]][input_curr[i][1]] = 1;
		}

		if (gar_count < MAX_GAR) {
			OBJECT_BUILDING* new_gar = (OBJECT_BUILDING*)malloc(sizeof(OBJECT_BUILDING));
			if (!new_gar) {
				p_system_message("�޸� �Ҵ� ����");
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
				
			// [ ���� �ڿ� �Ҹ� ]
			resource->spice -= 2;
			p_system_message("���Ҹ� �Ǽ��Ͽ����ϴ�.");
		}
		else {
			p_system_message("���Ҹ� ���̻� �Ǽ��� �� �����ϴ�.");
		}
		

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
		int input_curr[4][2] = { 0 };

		// [ �ʱⰪ ���� ]
		int minRow = curr.row;
		int minCol = curr.column;

		// [ ���� ��ġ ���� 8���� �˻� �迭 ]
		int dRow[9] = { 0, -1, -1, -1, 0, 1, 1, 1, 0 }; // ���� ��ġ ���� (8����)
		int dCol[9] = { 0, -1, 0, 1, 1, 1, 0, -1, -1 };

		for (int i = 0; i < 9; ++i) {
			int checkRow = curr.row + dRow[i];
			int checkCol = curr.column + dCol[i];

			// [ P���� Ȯ�� ]
			if (map[0][checkRow][checkCol] == 'P') {
				// ���� ���� �� �Ǵ� ���� �࿡�� ���� ���� ��ġ ����
				if (checkRow < minRow || (checkRow == minRow && checkCol < minCol)) {
					minRow = checkRow;
					minCol = checkCol;
				}
			}
		}

		// [ ���� ���� P�� ��ġ�� input_curr[0]�� ���� ]
		input_curr[0][0] = minRow;
		input_curr[0][1] = minCol;

		// [ pos2, pos3, pos4 ���� ]
		input_curr[1][0] = minRow;        // ���� ������
		input_curr[1][1] = minCol + 1;

		input_curr[2][0] = minRow + 1;    // �Ʒ��� ����
		input_curr[2][1] = minCol;

		input_curr[3][0] = minRow + 1;    // �Ʒ��� ������
		input_curr[3][1] = minCol + 1;

		// [ �ʰ� ���õ� ������ ������Ʈ ]
		for (int i = 0; i < 4; i++) {
			map[0][input_curr[i][0]][input_curr[i][1]] = 'B';
			base_barr_check[input_curr[i][0]][input_curr[i][1]] = 2;
		}

		// ���� �޸� �Ҵ�
		if (bar_count < MAX_BAR) {
			OBJECT_BUILDING* new_bar = (OBJECT_BUILDING*)malloc(sizeof(OBJECT_BUILDING));
			if (!new_bar) {
				p_system_message("�޸� �Ҵ� ����");
				return;
			}
			// [ ���ο� ���� ]
			*new_bar = (OBJECT_BUILDING){
				.pos1 = { input_curr[0][0], input_curr[0][1] },
				.pos2 = { input_curr[1][0], input_curr[1][1] },
				.pos3 = { input_curr[2][0], input_curr[2][1] },
				.pos4 = { input_curr[3][0], input_curr[3][1] },
				.repr = 'B',
				.layer = 0,
				.hp = 20
			};

			// [ �迭�� �߰� ]
			BAR[bar_count] = new_bar;
			bar_count++;

			// [ ���� �ڿ� �Ҹ� ]
			resource->spice -= 4;
			p_system_message("������ �Ǽ��Ͽ����ϴ�.");
		}
		else {
			p_system_message("������ �� �̻� ���� �� �����ϴ�.");
		}
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
		int input_curr[4][2] = { 0 };
		// [ �ʱⰪ ���� ]
		int minRow = curr.row;
		int minCol = curr.column;

		// [ ���� ��ġ ���� 8���� �˻� �迭 ]
		int dRow[9] = { 0, -1, -1, -1, 0, 1, 1, 1, 0 }; // ���� ��ġ ���� (8����)
		int dCol[9] = { 0, -1, 0, 1, 1, 1, 0, -1, -1 };

		for (int i = 0; i < 9; ++i) {
			int checkRow = curr.row + dRow[i];
			int checkCol = curr.column + dCol[i];

			// [ P���� Ȯ�� ]
			if (map[0][checkRow][checkCol] == 'P') {
				// ���� ���� �� �Ǵ� ���� �࿡�� ���� ���� ��ġ ����
				if (checkRow < minRow || (checkRow == minRow && checkCol < minCol)) {
					minRow = checkRow;
					minCol = checkCol;
				}
			}
		}

		// [ ���� ���� P�� ��ġ�� input_curr[0]�� ���� ]
		input_curr[0][0] = minRow;
		input_curr[0][1] = minCol;

		// [ pos2, pos3, pos4 ���� ]
		input_curr[1][0] = minRow;        // ���� ������
		input_curr[1][1] = minCol + 1;

		input_curr[2][0] = minRow + 1;    // �Ʒ��� ����
		input_curr[2][1] = minCol;

		input_curr[3][0] = minRow + 1;    // �Ʒ��� ������
		input_curr[3][1] = minCol + 1;

		// [ �ʰ� ���õ� ������ ������Ʈ ]
		for (int i = 0; i < 4; i++) {
			map[0][input_curr[i][0]][input_curr[i][1]] = 'S';
			shle_sold_check[input_curr[i][0]][input_curr[i][1]] = 1;
		}


		if (she_count < MAX_BAR) {
			OBJECT_BUILDING* new_she = (OBJECT_BUILDING*)malloc(sizeof(OBJECT_BUILDING));
			
			// [ ����ó�� ]
			if (!new_she) {
				p_system_message("�޸� �Ҵ� ����");
				return;
			}

			// [ ���ο� ����ó ]
			*new_she = (OBJECT_BUILDING){
				{ input_curr[0][0], input_curr[0][1] },
				{ input_curr[1][0], input_curr[1][1] },
				{ input_curr[2][0], input_curr[2][1] },
				{ input_curr[3][0], input_curr[3][1] },
				'S', 0, 10
			};

			// [ �迭�� ����ó �߰� ]
			SHE[she_count] = new_she;

			// [ ī��Ʈ ���� ]
			she_count++;

			// [ ���� �ڿ� �Ҹ� ]
			resource->spice -= 5;
			p_system_message("����ó�� �Ǽ��Ͽ����ϴ�.");
		}
		else {
			p_system_message("����ó�� ���̻� ���� �� �����ϴ�.");
		}

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
				OBJECT_SAMPLE* new_hav = (OBJECT_SAMPLE*)malloc(sizeof(OBJECT_SAMPLE));
				if (!new_hav) {
					p_system_message("�޸� �Ҵ� ����");
					return;
				}

				// [ ���ο� �Ϻ����� ]
				*new_hav = (OBJECT_SAMPLE){
					{ 14, 2 }, { 14, 2 }, 'H', 2000, 2000,
					COLOR_DEFAULT + 16, 70, 0, 0, 0, 4000, 4000
				};

				// [ �Ϻ����͸� �迭�� �߰� ]
				havs[hav_count] = new_hav;

				// [ �ʿ� �Ϻ����� ��ġ �ݿ� ]
				map[1][havs[hav_count]->pos.row][havs[hav_count]->pos.column] = 'H';
				check_friend[havs[hav_count]->pos.row][havs[hav_count]->pos.column] = 1;

				// [ �ý��� �޽��� �߰� ]
				p_system_message("�Ϻ����Ͱ��� ����Ǿ����ϴ�.");
				hav_count++;

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
void press_s_s(CURSOR cursor, RESOURCE* resource, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], bool* space_key_press) {
	POSITION curr = cursor.current;

	// [ ����� ]
	/*char mes[100];
	sprintf_s(mes, sizeof(mes), "space_key_press: %d", *space_key_press);
	p_system_message(mes);*/

	if (*space_key_press) {
		p_system_message("���� ���� SŰ�� �������ϴ�.");
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
					// [ ���� ���� ]
					if (sold_count < MAX_SOLD) {
						OBJECT_SAMPLE* new_sold = (OBJECT_SAMPLE*)malloc(sizeof(OBJECT_SAMPLE));
						if (!new_sold) {
							p_system_message("�޸� �Ҵ� ����");
							return;
						}
						// [ �ڿ� ���� & �α��� ���� ]
						resource->spice -= 1;
						resource->population += 1;

						// [ ���ο� ���� ���� ]
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

						// �迭�� ���� �ֱ�
						sold[sold_count] = new_sold;

						// ���� ��ġ�� ����
						shle_sold_check[new_sold->pos.row][new_sold->pos.column] = 2;  // ������

						// ���� ��ġ �ʿ� �ݿ�
						map[1][new_sold->pos.row][new_sold->pos.column] = new_sold->repr;
						sold_count++;

						// [ �ý��� �޽��� ]
						p_system_message("������ �����Ͽ����ϴ�.");
						break;
					}
					else {
						p_system_message("������ ���̻� ������ �� �����ϴ�.");
					}
				}
				else if (resource->spice < 1) {
					p_system_message("���� �ڿ��� �����Ͽ� ������ ������ �� �����ϴ�.");
					break;
				}
				else if (resource->population + 1 > resource->population_max) {
					p_system_message("�α����� �����Ͽ� ������ ������ �� �����ϴ�.");
					break;
				}
			}
		}
	}
}

// [ F (������ ����) Ű�� ������ �� ]
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

						// [ ������ ���� ]
						OBJECT_SAMPLE* new_frem = (OBJECT_SAMPLE*)malloc(sizeof(OBJECT_SAMPLE));
						if (!new_frem) {
							p_system_message("�޸� �Ҵ� ����");
							return;
						}

						// [ �ڿ� ����, �α��� ���� ]
						resource->spice -= 5;
						resource->population += 5;

						// [ ���ο� ������ ���� ]
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

						// [ �迭�� ������ �ֱ� ]
						frem[frem_count] = new_frem;

						// [ ������ ��ġ�� ���� ]
						frem_fight_fact_check[new_frem->pos.row][new_frem->pos.column] = 2;  // 2�� ������

						// [ ������ ��ġ �ʿ� �ݿ� ]
						map[1][new_frem->pos.row][new_frem->pos.column] = new_frem->repr;
						frem_count++;

						// [ �ý��� �޽��� ]
						p_system_message("�������� �����Ͽ����ϴ�.");
						break;
					}
					else {
						p_system_message("�������� ���̻� ������ �� �����ϴ�.");
					}
				}
				else if (resource->spice < 5) {
					p_system_message("���� �ڿ��� �����Ͽ� �������� ������ �� �����ϴ�.");
					break;
				}
				else if (resource->population + 5 > resource->population_max) {
					p_system_message("�α����� �����Ͽ� �������� ������ �� �����ϴ�.");
					break;
				}
			}
		}	
	}
}






/* -=-=-=-=-=-=-=-=[ ���ڳ� ���� AI ]-=-=-=-=-=-=-=-= -*/

// [ ���ڳ� �ǹ� �ʱ�ȭ ]
void hako_list_building(void) {
	E_DOR[0] = &e_dor1;
	E_GAR[0] = &e_gar1;
	ARE[0] = &e_are1;
	FAC[0] = &e_fac1;
}

// [ ���ڳ� �ǹ� ��� ]
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

// [ ���ڳ� �ʱ� �ǹ� ��ġ ]
void hako_building(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], int check_friend[MAP_HEIGHT][MAP_WIDTH]) {
	// Ai������ ���Ƽ� �迭�� �־ �ǹ̰� ������ �׷��� �ϴ� �迭�� �־���.
	hako_list_building();
	
	// [ ���ڳ� �ǹ� ��� ]
	hako_print_building(E_DOR, e_dor_count, map, check_friend); // ����
	hako_print_building(E_GAR, e_gar_count, map, check_friend); // â��
	hako_print_building(ARE, are_count, map, check_friend); // ������
	hako_print_building(FAC, fac_count, map, check_friend); // ����
	frem_fight_fact_check[FAC[0]->pos1.row][FAC[0]->pos1.column] = 1;
	frem_fight_fact_check[FAC[0]->pos2.row][FAC[0]->pos2.column] = 1;
	frem_fight_fact_check[FAC[0]->pos3.row][FAC[0]->pos3.column] = 1;
	frem_fight_fact_check[FAC[0]->pos4.row][FAC[0]->pos4.column] = 1;
}


