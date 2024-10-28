/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#include "display.h"
#include "io.h"

// ����� ������� �»��(topleft) ��ǥ
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

// ==== [ �Լ��� �ǹ� ��� ] ==== //

// [ �Ʊ� ���� ��� �Լ� ]
void p_f_base(OBJECT_BUILDING fb, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	POSITION fb_num[] = { fb.pos1, fb.pos2, fb.pos3, fb.pos4 };
	for (int i = 0; i < 4; i++) {
		map[0][fb_num[i].row][fb_num[i].column] = fb.repr;
	}
}

// [ ���� ���� ��� �Լ� ]
void p_e_base(OBJECT_BUILDING eb, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	POSITION eb_num[] = { eb.pos1, eb.pos2, eb.pos3, eb.pos4 };
	for (int i = 0; i < 4; i++) {
		map[0][eb_num[i].row][eb_num[i].column] = eb.repr;
	}
}

// [ �Ʊ� ���� ��� �Լ� ]
void p_f_plate(OBJECT_BUILDING fp, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	POSITION fp_num[] = { fp.pos1, fp.pos2, fp.pos3, fp.pos4 };
	for (int i = 0; i < 4; i++) {
		map[0][fp_num[i].row][fp_num[i].column] = fp.repr;
	}
}

// [ ���� ���� ��� �Լ� ] 
void p_e_plate(OBJECT_BUILDING ep, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	POSITION ep_num[] = { ep.pos1, ep.pos2, ep.pos3, ep.pos4 };
	for (int i = 0; i < 4; i++) {
		map[0][ep_num[i].row][ep_num[i].column] = ep.repr;
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
void p_building(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {

	p_f_base(f_base, map); // �Ʊ� ����

	p_e_base(e_base, map); // ���� ����

	p_f_plate(f_plate, map); // �Ʊ� ����

	p_e_plate(e_plate, map); // ���� ����

	p_start_spice(start_spice, map); // �ʱ� �����̽� ������ �Ʊ� 1 ���� 1

	p_rock_1(rock_1, map); // ���� 1 
	p_rock_2(rock_2, map); // ���� 2
	p_rock_3(rock_3, map); // ���� 3
	p_rock_4(rock_4, map); // ���� 4
	
}


// =================================== [ ���� ] ======================================= //

// ==== [ ����ü�� ���� ���� ] ==== //

// [ ����� ]
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

// [ �Ʊ� �Ϻ����� ]
OBJECT_SAMPLE f_hav = {
	.pos = {14, 1},
	.dest = {12, 1}, // �����̽� ��ġ�� �ٲ����
	.repr = 'H',
	.move_period = 2000,
	.next_move_time = 2000,
	.layer = 1
};


// [ ���� �Ϻ����� ]
OBJECT_SAMPLE e_hav = {
	.pos = {3, 58},
	.dest = {5, 58}, // �����̽� ��ġ�� �ٲ����
	.repr = 'H',
	.move_period = 2000,
	.next_move_time = 2000,
	.layer = 1
};

// ==== [ �Լ��� ���� ��� ] ==== //

// [ �Ʊ� �Ϻ����� ��� �Լ� ]
void p_f_hav(OBJECT_SAMPLE f_hav, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	map[1][f_hav.pos.row][f_hav.pos.column] = f_hav.repr;
}
// [ ���� �Ϻ����� ��� �Լ� ]
void p_e_hav(OBJECT_SAMPLE e_hav, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	map[1][e_hav.pos.row][e_hav.pos.column] = e_hav.repr;
}

// [ ����� 1 ��� �Լ� ]
void p_sandworm_1(OBJECT_SAMPLE sandworm_1, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	map[1][sandworm_1.pos.row][sandworm_1.pos.column] = sandworm_1.repr;
}

// [����� 2 ��� �Լ� ]
void p_sandworm_2(OBJECT_SAMPLE sandworm_2, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	map[1][sandworm_2.pos.row][sandworm_2.pos.column] = sandworm_2.repr;
}

// [ ��� ���� ��� �Լ� ]
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
				
				// === [ �ǹ� ] === //
				
				// [ �Ʊ� ���� ]
				if (backbuf[i][j] == 'B' && (i == 16 || i == 15) && (j == 1 || j == 2)) {
					// < �Ķ��� ��� >
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT + 16);
					colorbuf[i][j] = COLOR_DEFAULT + 16;
				}

				// [ ���� ���� ] 
				else if (backbuf[i][j] == 'B' && (i == 1 || i == 2) && (j == 57 || j == 58)) {
					// < ������ ��� >
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT + 64);
					colorbuf[i][j] = COLOR_DEFAULT + 64;
				}

				// [ �Ʊ� ���� ���� ]
				else if (backbuf[i][j] == 'P') {
					// < ������ ��� >
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_BLACK);
					colorbuf[i][j] = COLOR_BLACK;
				}

				// [ �����̽� ]
				else if (backbuf[i][j] == '5') {
					// < ��Ȳ�� ��� >    ���� �� �� ã��
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT + 48);
					colorbuf[i][j] = COLOR_DEFAULT + 48;
				}

				// [ ���� ]
				else if (backbuf[i][j] == 'R') {
					// < ȸ�� ��� >      ���� �� �� ã��
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT);
					colorbuf[i][j] = COLOR_DEFAULT;
				}

				// ==== [ ���� ] ====

				// < �Ʊ� �Ϻ����� >
				else if (backbuf[i][j] == 'H' && i == 14 && j == 1) {
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT + 16);
					colorbuf[i][j] = COLOR_DEFAULT + 16;
				}

				// < ���� �Ϻ����� >
				else if (backbuf[i][j] == 'H' && i == 3 && j == 58) {
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT + 64);
					colorbuf[i][j] = COLOR_DEFAULT + 64;
				}

				// < ����� 1 >
				else if (backbuf[i][j] == 'W' && i == 2 && j == 4) {
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT + 200); // ���� ���� ������
					colorbuf[i][j] = COLOR_DEFAULT + 200;
				}

				// < ����� 2 >
				else if (backbuf[i][j] == 'W' && i == 12 && j == 55) {
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT + 200); // ���� ���� ������
					colorbuf[i][j] = COLOR_DEFAULT + 200;
				}

				// < ��Ÿ >
				else {
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_RESOURCE);
					colorbuf[i][j] = COLOR_RESOURCE;
				}
			}
			frontbuf[i][j] = backbuf[i][j];
		}
	}
}

// frontbuf[][]���� Ŀ�� ��ġ�� ���ڸ� ���� �ٲ㼭 �״�� �ٽ� ���
void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;

	char ch = frontbuf[prev.row][prev.column]; 
	printc(padd(map_pos, prev), ch, COLOR_BLACK);

	// [ Ŀ���� ������ �ڸ��� ���� �������� �ʰ� ���� ]
	int pre_color = colorbuf[prev.row][prev.column]; // �� ��ġ�� ������ ���
	printc(padd(map_pos, prev), ch, pre_color); // ������ �ڸ��� �ٽ� ���� ������ ����

	ch = frontbuf[curr.row][curr.column];
	printc(padd(map_pos, curr), ch, COLOR_BLACK);
}	


// [ ���� & ���â �⺻ Ʋ �Լ� ] 
void state_project(char src[STATE_HEIGHT][STATE_WIDTH], char dest[STATE_HEIGHT][STATE_WIDTH]) {
	for (int i = 0; i < STATE_HEIGHT; i++) {
		for (int j = 0; j < STATE_WIDTH; j++) {
			if (src[i][j] >= 0) {
				dest[i][j] = src[i][j];
			}
		}
	}
}


// [ ���� & ���â �׸��� �Լ� ]
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


// [ �����̽��ٸ� ������ �� ]
void state_spacebar(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], 
	char state_map[STATE_HEIGHT][STATE_WIDTH], CURSOR cursor) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;
	POSITION pos_state = { 0, 1 };

	// [ ���� & ���â �ʱ�ȭ ]
	for (int i = 1; i < STATE_HEIGHT - 1; i++) {
		for (int j = 1; j < STATE_WIDTH - 1; j++) {
			POSITION pos = { i, j }; 
			prints(padd(state_pos, pos), " ");
		}
	}
	
	// [ ���� & ��� ��� ]
	// < �� ���� >
	if (backbuf[curr.row][curr.column] == ' ') {
		char state_message[100] = "���� : �縷 ����";
		prints(padd(state_mes, pos_state), state_message);
	}
	// < �Ʊ� ���� >
	else if (backbuf[curr.row][curr.column] == 'B' && \
		(curr.row == 16 || curr.row == 15) && (curr.column == 1 || curr.column == 2)) {
		char state_message[100] = "�ǹ� : �Ʊ� ����";
		prints(padd(state_mes, pos_state), state_message);
	}


	
}

// [ ESC Ű�� ������ �� ]
void state_esc(char state_map[STATE_HEIGHT][STATE_WIDTH]) { 
	
}