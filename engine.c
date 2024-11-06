#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <math.h>
#include "common.h"
#include "io.h"
#include "display.h"

void init(void);
void intro(void);
void outro(void);
void cursor_move(DIRECTION dir, int move);
void sample_obj_move(void);
POSITION sample_obj_next_position(void);

// ����� 1
POSITION sw1_next_pos(void);
void sw1_move(void);

// ����� 2
POSITION sw2_next_pos(void);
void sw2_move(void);

// �縷 ������
POSITION d_eagle_next_pos(void);
void d_eagle_move(void);

// �� ��ǳ
POSITION sand_wind_next_pos(void);
void sand_wind_move(void);

// �ý��� �޽���
char* send_system_message[1];

/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
CURSOR cursor = { { 1, 1 }, {1, 1} };

int move_count = 0;
int move_check[2] = { 0 };
char spice_number[9] = { '1', '2', '3', '4', '5', '6', '7', '8', '9' };

/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };  // 2, 16, 60
char state_map[STATE_HEIGHT][STATE_WIDTH] = { 0 }; // 16, 50
char sysmes_map[SYSMES_HEIGHT][SYSMES_WIDTH] = { 0 }; // 6 60
char order_map[ORDER_HEIGHT][ORDER_WIDTH] = { 0 }; // 6 60
int check_friend[MAP_HEIGHT][MAP_WIDTH] = { 0 }; // 0 --> �ƹ��͵� �ƴ�, 1 --> �Ʊ�, 2 --> ����

RESOURCE resource = {
	.spice = 5,
	.spice_max = 20,
	.population = 0,
	.population_max = 50
};

OBJECT_SAMPLE obj = { 
	.pos = {1, 1},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'o',
	.move_period = 300,
	.next_move_time = 300
};  

// [ �Ʊ� �Ϻ����� ]
OBJECT_SAMPLE f_hav_obj = {
	.pos = {14, 1},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'H',
	.move_period = 2000,
	.next_move_time = 2000,
	.color = COLOR_DEFAULT + 16
};

// [ ���� �Ϻ����� ]
OBJECT_SAMPLE e_hav_obj = {
	.pos = {3, 58},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'H',
	.move_period = 2000,
	.next_move_time = 2000,
	.color = COLOR_DEFAULT + 64
};

// [ ����� 1 ]
OBJECT_SAMPLE sw1_obj = {
	.pos = {2, 4},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'W',
	.move_period = 2500,
	.next_move_time = 2500
};

// [ ����� 2 ]
OBJECT_SAMPLE sw2_obj = {
	.pos = {12, 55},
	.dest = {1, 1},
	.repr = 'W',
	.move_period = 2500,
	.next_move_time = 2500
};

// [ �縷 ������ ]
OBJECT_SAMPLE d_eagle = {
	.pos = {1, 1},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'E',
	.move_period = 500,
	.next_move_time = 500
};

// [ �� ��ǳ ]
SAND_WIND sand_wind = {
	.pos1 = {1, 1},
	.pos2 = {1, 1},
	.pos3 = {1, 1},
	.pos4 = {1, 1},
	.dest1 = {1, 1},
	.dest2 = {1, 1},
	.dest3 = {1, 1},
	.dest4 = {1, 1},
	.repr = 'Z',
	.move_period = 150,
	.next_move_time = 150
};



/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL));

	init();
	intro();
	display(resource, map, cursor, state_map, sysmes_map, order_map, check_friend);

	send_system_message[0] = "������ ���۵Ǿ����ϴ�.";
	p_system_message(send_system_message[0], sysmes_map);

	while (1) {
		// loop �� ������(��, TICK==10ms����) Ű �Է� Ȯ��
		KEY key = get_key();

		// sys_clock���� ���� Ŀ�� ����ĭ �̵� ���� �ʱ�ȭ
		if (sys_clock % 90 == 0) {
			move_count = 0; // Ŀ���� 1ĭ �����̴��� 3ĭ �����̴��� Ȯ��
			for (int i = 0; i < 2; i++) {
				move_check[i] = 0; // 3ĭ �����̴� Ŀ���� ��, ������ ����Ű�� �ι� �������� Ȯ��
			}
		}

		// Ű �Է��� ���� ��, 1ĭ�̵�����, 3ĭ�̵����� Ȯ��
		if (is_arrow_key(key)) {
			move_count++;
			if (move_count == 1) {
				move_check[0] = key;
			}
			else if (move_count == 2) {
				move_check[1] = key;
			}
			// 3ĭ �����̴� ���, ����Ű�� �ٸ��ٸ� ��ĭ �����̰� �۵�
			if (move_count == 2 && move_check[1] != move_check[0]) {
				move_count = 1;
				for (int i = 0; i < 2; i++) {
					move_check[i] = 0; // �ʱ�ȭ
				}
			}
		}
		
		// Ű �Է��� ������ ó��
		if (is_arrow_key(key) && move_count == 2 && move_check[0] == move_check[1]) {
			cursor_move(ktod(key), 1);
			move_count = 0; // 3ĭ �̵��ϰ� ���� ���� �ʱ�ȭ.
		}
		else if (is_arrow_key(key) && move_count == 1) {
			cursor_move(ktod(key), 0);
		}
		else {
			// ����Ű ���� �Է�
			switch (key) {
			case k_quit: outro();
			
			case k_esc: state_esc(state_map, order_map);
				send_system_message[0] = "ESCŰ�� �������ϴ�.";
				p_system_message(send_system_message[0], sysmes_map);
				break;
			
			case k_space: state_spacebar(map, state_map, cursor, check_friend);
				send_system_message[0] = "�����̽��ٸ� �������ϴ�.";
				p_system_message(send_system_message[0], sysmes_map);
				break;

			case k_h: press_h(&resource, map, sysmes_map, check_friend);
				send_system_message[0] = "HŰ�� �������ϴ�.";
				p_system_message(send_system_message[0], sysmes_map);
				break;

			case k_none:
			case k_undef:
			default: break;
			}
		}
	
		// ���� ������Ʈ ����
		d_eagle_move();
		sw1_move();
		sw2_move();
		//sand_wind_move();
		

		// ȭ�� ���
		display(resource, map, cursor, state_map, sysmes_map, order_map, check_friend);
		Sleep(TICK);
		sys_clock += 10;
	}
}

/* ================= subfunctions =================== */
void intro(void) {
	printf("DUNE 1.5\n");
	Sleep(2000);
	system("cls");
}

void outro(void) {
	printf("exiting...\n");
	exit(0);
}

void init(void) {
	// layer 0(map[0])�� ���� ����
	for (int j = 0; j < MAP_WIDTH; j++) {
		map[0][0][j] = '#';
		map[0][MAP_HEIGHT - 1][j] = '#';
	}

	for (int i = 1; i < MAP_HEIGHT - 1; i++) {
		map[0][i][0] = '#';
		map[0][i][MAP_WIDTH - 1] = '#';
		for (int j = 1; j < MAP_WIDTH - 1; j++) {
			map[0][i][j] = ' ';
		}
	}

	// layer 1(map[1])�� ��� �α�(-1�� ä��)
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[1][i][j] = -1;
		}
	}

	// [ ����â �⺻ Ʋ ]
	for (int i = 0; i < STATE_WIDTH; i++) {
		state_map[0][i] = '#';
		state_map[STATE_HEIGHT - 1][i] = '#';
	}

	for (int i = 1; i < STATE_HEIGHT - 1; i++) {
		state_map[i][0] = '#';
		state_map[i][STATE_WIDTH - 1] = '#';
		for (int j = 1; j < STATE_WIDTH - 1; j++) {
			state_map[i][j] = ' ';
		}
	}
	
	// [ �ý��� �⺻ Ʋ ]
	for (int i = 0; i < SYSMES_WIDTH; i++) {
		sysmes_map[0][i] = '#';
		sysmes_map[SYSMES_HEIGHT - 1][i] = '#';
	}

	for (int i = 1; i < SYSMES_HEIGHT - 1; i++) {
		sysmes_map[i][0] = '#';
		sysmes_map[i][SYSMES_WIDTH - 1] = '#';
		for (int j = 1; j < SYSMES_WIDTH - 1; j++) {
			sysmes_map[i][j] = ' ';
		}
	}

	// [ ���â �⺻ Ʋ ]
	for (int i = 0; i < ORDER_WIDTH; i++) {
		order_map[0][i] = '#';
		order_map[ORDER_HEIGHT - 1][i] = '#';
	}

	for (int i = 1; i < ORDER_HEIGHT - 1; i++) {
		order_map[i][0] = '#';
		order_map[i][ORDER_WIDTH - 1] = '#';
		for (int j = 1; j < ORDER_WIDTH - 1; j++) {
			order_map[i][j] = ' ';
		}
	}

	// layer 2(map[2])�� ��� �α�(-1�� ä��)
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[2][i][j] = -1;
		}
	}


	// [ ���� ������Ʈ ]
	map[2][d_eagle.pos.row][d_eagle.pos.column] = d_eagle.repr; // �縷 ������
	map[1][f_hav_obj.pos.row][f_hav_obj.pos.column] = f_hav_obj.repr; // �Ʊ� �Ϻ�����
	check_friend[f_hav_obj.pos.row][f_hav_obj.pos.column] = 1;
	map[1][e_hav_obj.pos.row][e_hav_obj.pos.column] = e_hav_obj.repr; // ���� �Ϻ����� 
	check_friend[e_hav_obj.pos.row][e_hav_obj.pos.column] = 2;
	map[1][sw1_obj.pos.row][sw1_obj.pos.column] = sw1_obj.repr; // ����� 1
	map[1][sw2_obj.pos.row][sw2_obj.pos.column] = sw2_obj.repr; // ����� 2
}



// (�����ϴٸ�) ������ �������� Ŀ�� �̵�
void cursor_move(DIRECTION dir, int move) {
	
	POSITION curr = cursor.current;
	POSITION new_pos = pmove(curr, dir);
	if (move == 1) {
		new_pos = pmove2(curr, dir);  

	}
	// validation check
	if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 && \
		1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {

		cursor.previous = cursor.current;
		cursor.current = new_pos;
	}
	else { // Ŀ���� ��ġ�� �������� ������� �ϸ� ��� �� ��ġ�� ��ġ�ϰ� �ϱ�.
		cursor.previous = cursor.previous;
		cursor.current = cursor.current;
	}
}


/* ================= sample object movement =================== */
POSITION sample_obj_next_position(void) {
	// ���� ��ġ�� �������� ���ؼ� �̵� ���� ����	
	POSITION diff = psub(obj.dest, obj.pos);
	DIRECTION dir;

	// ������ ����. ������ �ܼ��� ���� �ڸ��� �պ�
	if (diff.row == 0 && diff.column == 0) {
		if (obj.dest.row == 1 && obj.dest.column == 1) {
			// topleft --> bottomright�� ������ ����
			POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
			obj.dest = new_dest;
		}
		else {
			// bottomright --> topleft�� ������ ����
			POSITION new_dest = { 1, 1 };
			obj.dest = new_dest;
		}
		return obj.pos;
	}

	// ������, ������ �Ÿ��� ���ؼ� �� �� �� ������ �̵�
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// validation check
	// next_pos�� ���� ����� �ʰ�, (������ ������)��ֹ��� �ε����� ������ ���� ��ġ�� �̵�
	// ������ �浹 �� �ƹ��͵� �� �ϴµ�, ���߿��� ��ֹ��� ���ذ��ų� ���� ������ �ϰų�... ���
	POSITION next_pos = pmove(obj.pos, dir);
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] < 0) {

		return next_pos;
	}
	else {
		return obj.pos;  // ���ڸ�
	}
}

void sample_obj_move(void) {
	if (sys_clock <= obj.next_move_time) {
		// ���� �ð��� �� ����
		return;
	}

	// ������Ʈ(�ǹ�, ���� ��)�� layer1(map[1])�� ����
	map[1][obj.pos.row][obj.pos.column] = -1;
	obj.pos = sample_obj_next_position();
	map[1][obj.pos.row][obj.pos.column] = obj.repr;

	obj.next_move_time = sys_clock + obj.move_period;
}


// [ �縷 ������ ]
POSITION d_eagle_next_pos(void) {
	// ���� ��ġ�� �������� ���ؼ� �̵� ���� ����	
	POSITION diff = psub(d_eagle.dest, d_eagle.pos);
	DIRECTION dir;

	// ������ ����. ������ �ܼ��� ���� �ڸ��� �պ�
	if (diff.row == 0 && diff.column == 0) {
		if (d_eagle.dest.row == 1 && d_eagle.dest.column == 1) {
			// topleft --> bottomright�� ������ ����
			POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
			d_eagle.dest = new_dest;
		}
		else {
			// bottomright --> topleft�� ������ ����
			POSITION new_dest = { 1, 1 };
			d_eagle.dest = new_dest;
		}
		return d_eagle.pos;
	}

	// ������, ������ �Ÿ��� ���ؼ� �� �� �� ������ �̵�
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// validation check
	POSITION next_pos = pmove(d_eagle.pos, dir);
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[2][next_pos.row][next_pos.column] < 0) {

		return next_pos;
	}
	else {
		return d_eagle.pos;  // ���ڸ�
	}
}

void d_eagle_move(void) {
	if (sys_clock <= d_eagle.next_move_time) {
		return;
	}

	map[2][d_eagle.pos.row][d_eagle.pos.column] = -1;

	d_eagle.pos = d_eagle_next_pos();
	map[2][d_eagle.pos.row][d_eagle.pos.column] = d_eagle.repr;
	d_eagle.next_move_time = sys_clock + d_eagle.move_period;
}


// [ ����� (1) ]
POSITION sw1_next_pos(void) {
	// ���� ��ġ�� �������� ���ؼ� �̵� ���� ����	
	POSITION diff = psub(sw1_obj.dest, sw1_obj.pos);
	DIRECTION dir;

	

	// [ �������� ���� ����� ���� ]
	
	// ���� ����� ������ ã�Ƽ� �� ������ ��İ��� new_dest�� ����
	double check_close = 61.0;
	int move_i = 2, move_j = 4;
	for (int i = 1; i < MAP_HEIGHT - 1; i++) {
		for (int j = 1; j < MAP_WIDTH - 1; j++) {
			if (map[1][i][j] == 'H' || map[1][i][j] == 'T' || map[1][i][j] == 'S' || map[1][i][j] == 'F') {
				if (sqrt(pow(i - sw1_obj.pos.row, 2) + pow(j - sw1_obj.pos.column, 2)) < check_close) {
					check_close = sqrt(pow(i - sw1_obj.pos.row, 2) + pow(j - sw1_obj.pos.column, 2));
					move_i = i, move_j = j;
				}
			}
		}
	}
	POSITION new_dest = { move_i, move_j };
	sw1_obj.dest = new_dest;


	// ������, ������ �Ÿ��� ���ؼ� �� �� �� ������ �̵�
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}


	// [ ����� �̵� ]
	POSITION next_pos = pmove(sw1_obj.pos, dir);

	// [ �ǹ�, ��ֹ�, ���ְ� ������ �� ���ذ��� ]
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[0][next_pos.row][next_pos.column] == 'R' || map[0][next_pos.row][next_pos.column] == '1' || map[0][next_pos.row][next_pos.column] == '2' || \
		map[0][next_pos.row][next_pos.column] == '3' || map[0][next_pos.row][next_pos.column] == '4' || map[0][next_pos.row][next_pos.column] == '5' || \
		map[0][next_pos.row][next_pos.column] == '6' || map[0][next_pos.row][next_pos.column] == '7' || map[0][next_pos.row][next_pos.column] == '8' || \
		map[0][next_pos.row][next_pos.column] == '9' || map[0][next_pos.row][next_pos.column] == 'B' || map[0][next_pos.row][next_pos.column] == 'P' || \
		map[0][next_pos.row][next_pos.column] == 'S' || map[0][next_pos.row][next_pos.column] == 'G' || map[0][next_pos.row][next_pos.column] == 'D' || \
		map[0][next_pos.row][next_pos.column] == 'A' || map[0][next_pos.row][next_pos.column] == 'S' || map[0][next_pos.row][next_pos.column] == 'F') {

		// [ �ǹ�, ��ֹ�, ������ ��, �Ʒ��� ���� �� ]
		if (sw1_obj.pos.row + 1 == next_pos.row || sw1_obj.pos.row - 1 == next_pos.row) {
			// [ ���������� �� ���� ������ �������� �� ���� ������ �� ]
			double move_left = sqrt(pow(sw1_obj.pos.row - new_dest.row, 2) + pow((sw1_obj.pos.column - 1) - new_dest.column, 2));
			double move_right = sqrt(pow(sw1_obj.pos.row - new_dest.row, 2) + pow((sw1_obj.pos.column + 1) - new_dest.column, 2));
			// [ ���������� ���� ]
			if (move_left < move_right) {
				// < ���������� ���� �� ���� ����� �Ǵ����� üũ >
				if (sw1_obj.pos.column + 1 != MAP_WIDTH - 1) { // ���� ����� ������ ������
					next_pos.row += 1;
					next_pos.column = sw1_obj.pos.column + 1;
				}
				else { // ���� ����� ����
					next_pos.row -= 1;
					next_pos.column = sw1_obj.pos.column - 1;
				}
			}
			// [ �������� ���� ]
			else {
				// < �������� ���� �� ���� ����� �Ǵ����� üũ >
				if (sw1_obj.pos.column - 1 != 0) { // ���� ����� ������ ����
					next_pos.row -= 1;
					next_pos.column = sw1_obj.pos.column - 1;
				}
				else { // ���� ����� ������
					next_pos.row += 1;
					next_pos.column = sw1_obj.pos.column + 1;
				}
			}
		}
		// [ �ǹ�, ��ֹ�, ������ ������, ���ʿ� ���� �� ]
		else if (sw1_obj.pos.column + 1 == next_pos.column || sw1_obj.pos.column - 1 == next_pos.column) {
			// [ ���� �� ���� ������ �Ʒ��� �� ���� ������ �� ]
			double move_up = sqrt(pow((sw1_obj.pos.row - 1) - new_dest.row, 2) + pow(sw1_obj.pos.column - new_dest.column, 2));
			double move_down = sqrt(pow((sw1_obj.pos.row + 1) - new_dest.row, 2) + pow(sw1_obj.pos.column - new_dest.column, 2));
			// [ ���� ���� ]
			if (move_up < move_down) {
				// < ���� ���� �� ���� ������� Ȯ�� >
				if (sw1_obj.pos.row + 1 != 0) { // ���� ����� ������ ����
					next_pos.column += 1;
					next_pos.row = sw1_obj.pos.row + 1;
				}
				else { // ���� ����� �Ʒ���
					next_pos.column -= 1;
					next_pos.row = sw1_obj.pos.row - 1;
				}
			}
			// [ �Ʒ��� ���� ]
			else {
				if (sw1_obj.pos.row - 1 != MAP_HEIGHT - 1) { // ���� ����� �Ʒ���
					next_pos.column -= 1;
					next_pos.row = sw1_obj.pos.row - 1;
				}
				else { // ���� ����� ����
					next_pos.column += 1;
					next_pos.row = sw1_obj.pos.row + 1;
				}
			}
		}
		return next_pos;
	}
	return next_pos;
}

void sw1_move(void) {
	// �����̴� �ֱ�
	if (sys_clock <= sw1_obj.next_move_time) {
		return;
	}
	// �����(1) layer1 (map[1])�� ����
	map[1][sw1_obj.pos.row][sw1_obj.pos.column] = -1;

	// 10/300 Ȯ���� �輳 --> �����̽� ������ ���� (���差 1 ~ 9 ����)
	int r = rand() % 299;
	if (r < 9) {
		send_system_message[0] = "������� �����̽� �������� �����߽��ϴ�.";
		p_system_message(send_system_message[0], sysmes_map);

		map[0][sw1_obj.pos.row][sw1_obj.pos.column] = spice_number[r];
	}

	sw1_obj.pos = sw1_next_pos();


	// [ ����� ��ƸԱ� ]
	// [ �Ʊ� �Ϻ����Ϳ� ������ �� (�̿ϼ�) ]
	if (map[1][sw1_obj.pos.row][sw1_obj.pos.column] == 'H' && check_friend[sw1_obj.pos.row][sw1_obj.pos.column] == 1) {
		send_system_message[0] = "������� �Ʊ� �Ϻ����͸� ��ƸԾ����ϴ�.";
		p_system_message(send_system_message[0], sysmes_map);

		// �ӽ÷� repr ����
		f_hav_obj.repr = ' ';
	}

	// [ ���� �Ϻ����Ϳ� ������ �� (�̿ϼ�) ]
	else if (map[1][sw1_obj.pos.row][sw1_obj.pos.column] == 'H' && check_friend[sw1_obj.pos.row][sw1_obj.pos.column] == 2) {
		send_system_message[0] = "������� ���� �Ϻ����͸� ��ƸԾ����ϴ�.";
		p_system_message(send_system_message[0], sysmes_map);

		// �ӽ÷� repr ����
		e_hav_obj.repr = ' ';
	}

	// [ ������� ������ �� (���� ���� X) ]
	else if (map[1][sw1_obj.pos.row][sw1_obj.pos.column] == 'F') {
		send_system_message[0] = "������� �������� ��ƸԾ����ϴ�.";
		p_system_message(send_system_message[0], sysmes_map);


	}

	// [ ����� ������ �� (���� ���� X) ]
	else if (map[1][sw1_obj.pos.row][sw1_obj.pos.column] == 'F') {
		send_system_message[0] = "������� ���縦 ��ƸԾ����ϴ�.";
		p_system_message(send_system_message[0], sysmes_map);


	}

	// [ ���� ������ �� (���� ���� X) ]
	else if (map[1][sw1_obj.pos.row][sw1_obj.pos.column] == 'S') {
		send_system_message[0] = "������� ������ ��ƸԾ����ϴ�.";
		p_system_message(send_system_message[0], sysmes_map);


	}

	// [ ������ ������ �� (���� ���� X) ]
	else if (map[1][sw1_obj.pos.row][sw1_obj.pos.column] == 'T') {
		send_system_message[0] = "������� �������� ��ƸԾ����ϴ�.";
		p_system_message(send_system_message[0], sysmes_map);


	}

	map[1][sw1_obj.pos.row][sw1_obj.pos.column] = sw1_obj.repr;
	sw1_obj.next_move_time = sys_clock + sw1_obj.move_period;
}


// [ ����� (2) ]
POSITION sw2_next_pos(void) {
	// ���� ��ġ�� �������� ���ؼ� �̵� ���� ����	
	POSITION diff = psub(sw2_obj.dest, sw2_obj.pos);
	DIRECTION dir;

	// [ �������� ���� ����� ���� ]

	// ���� ����� ������ ã�Ƽ� �� ������ ��İ��� new_dest�� ����
	double check_close = 61.0;
	int move_i = 12, move_j = 55;
	for (int i = 1; i < MAP_HEIGHT - 1; i++) {
		for (int j = 1; j < MAP_WIDTH - 1; j++) {
			if (map[1][i][j] == 'H' || map[1][i][j] == 'T' || map[1][i][j] == 'S' || map[1][i][j] == 'F') {
				if (sqrt(pow(i - sw2_obj.pos.row, 2) + pow(j - sw2_obj.pos.column, 2)) < check_close) {
					check_close = sqrt(pow(i - sw2_obj.pos.row, 2) + pow(j - sw2_obj.pos.column, 2));
					move_i = i, move_j = j;
				}
			}
		}
	}
	POSITION new_dest = { move_i, move_j };
	sw2_obj.dest = new_dest;


	// ������, ������ �Ÿ��� ���ؼ� �� �� �� ������ �̵�
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// [ ����� �̵� ]
	POSITION next_pos = pmove(sw2_obj.pos, dir);

	// [ �ǹ�, ��ֹ�, ���� ������ �� ���ذ��� ]
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[0][next_pos.row][next_pos.column] == 'R' || map[0][next_pos.row][next_pos.column] == '1' || map[0][next_pos.row][next_pos.column] == '2' || \
		map[0][next_pos.row][next_pos.column] == '3' || map[0][next_pos.row][next_pos.column] == '4' || map[0][next_pos.row][next_pos.column] == '5' || \
		map[0][next_pos.row][next_pos.column] == '6' || map[0][next_pos.row][next_pos.column] == '7' || map[0][next_pos.row][next_pos.column] == '8' || \
		map[0][next_pos.row][next_pos.column] == '9' || map[0][next_pos.row][next_pos.column] == 'B' || map[0][next_pos.row][next_pos.column] == 'P' || \
		map[0][next_pos.row][next_pos.column] == 'S' || map[0][next_pos.row][next_pos.column] == 'G' || map[0][next_pos.row][next_pos.column] == 'D' || \
		map[0][next_pos.row][next_pos.column] == 'A' || map[0][next_pos.row][next_pos.column] == 'S' || map[0][next_pos.row][next_pos.column] == 'F') {

		// [ �ǹ�, ��ֹ�, ������ ��, �Ʒ��� ���� �� ]
		if (sw2_obj.pos.row + 1 == next_pos.row || sw2_obj.pos.row - 1 == next_pos.row) {
			// [ ���������� �� ���� ������ �������� �� ���� ������ �� ]
			double move_left = sqrt(pow(sw2_obj.pos.row - new_dest.row, 2) + pow((sw2_obj.pos.column - 1) - new_dest.column, 2));
			double move_right = sqrt(pow(sw2_obj.pos.row - new_dest.row, 2) + pow((sw2_obj.pos.column + 1) - new_dest.column, 2));
			// [ ���������� ���� ]
			if (move_left < move_right) {
				// < ���������� ���� �� ���� ����� �Ǵ����� üũ >
				if (sw2_obj.pos.column + 1 != MAP_WIDTH - 1) { // ���� ����� ������ ������
					next_pos.row += 1;
					next_pos.column = sw2_obj.pos.column + 1;
				}
				else { // ���� ����� ����
					next_pos.row -= 1;
					next_pos.column = sw2_obj.pos.column - 1;
				}
			}
			// [ �������� ���� ]
			else {
				// < �������� ���� �� ���� ����� �Ǵ����� üũ >
				if (sw2_obj.pos.column - 1 != 0) { // ���� ����� ������ ����
					next_pos.row -= 1;
					next_pos.column = sw2_obj.pos.column - 1;
				}
				else { // ���� ����� ������
					next_pos.row += 1;
					next_pos.column = sw2_obj.pos.column + 1;
				}
			}
		}
		// [ �ǹ�, ��ֹ�, ������ ������, ���ʿ� ���� �� ]
		else if (sw2_obj.pos.column + 1 == next_pos.column || sw2_obj.pos.column - 1 == next_pos.column) {
			// [ ���� �� ���� ������ �Ʒ��� �� ���� ������ �� ]
			double move_up = sqrt(pow((sw2_obj.pos.row - 1) - new_dest.row, 2) + pow(sw2_obj.pos.column - new_dest.column, 2));
			double move_down = sqrt(pow((sw2_obj.pos.row + 1) - new_dest.row, 2) + pow(sw2_obj.pos.column - new_dest.column, 2));
			// [ ���� ���� ]
			if (move_up < move_down) {
				// < ���� ���� �� ���� ������� Ȯ�� >
				if (sw2_obj.pos.row + 1 != 0) { // ���� ����� ������ ����
					next_pos.column += 1;
					next_pos.row = sw2_obj.pos.row + 1;
				}
				else { // ���� ����� �Ʒ���
					next_pos.column -= 1;
					next_pos.row = sw2_obj.pos.row - 1;
				}
			}
			// [ �Ʒ��� ���� ]
			else {
				if (sw2_obj.pos.row - 1 != MAP_HEIGHT - 1) { // ���� ����� �Ʒ���
					next_pos.column -= 1;
					next_pos.row = sw2_obj.pos.row - 1;
				}
				else { // ���� ����� ����
					next_pos.column += 1;
					next_pos.row = sw2_obj.pos.row + 1;
				}
			}
		}
		return next_pos;
	}
	return next_pos;
}

void sw2_move(void) {
	// �����̴� �ֱ�
	if (sys_clock <= sw2_obj.next_move_time) {
		return;
	}
	// �����(2) layer1 (map[1])�� ����
	map[1][sw2_obj.pos.row][sw2_obj.pos.column] = -1;

	// 10/300 Ȯ���� �輳 --> �����̽� ������ ���� (���差 1 ~ 9 ����)
	int r = rand() % 299;
	if (r < 9) {
		send_system_message[0] = "������� �����̽� �������� �����߽��ϴ�.";
		p_system_message(send_system_message[0], sysmes_map);

		map[0][sw2_obj.pos.row][sw2_obj.pos.column] = spice_number[r];
	}

	sw2_obj.pos = sw2_next_pos();


	// [ ����� ��ƸԱ� ]
	// [ �Ʊ� �Ϻ����Ϳ� ������ �� (�̿ϼ�) ]
	if (map[1][sw2_obj.pos.row][sw2_obj.pos.column] == 'H' && check_friend[sw2_obj.pos.row][sw2_obj.pos.column] == 1) {
		send_system_message[0] = "������� �Ʊ� �Ϻ����͸� ��ƸԾ����ϴ�.";
		p_system_message(send_system_message[0], sysmes_map);

		// �ӽ÷� repr ����
		f_hav_obj.repr = ' ';
	}

	// [ ���� �Ϻ����Ϳ� ������ �� (�̿ϼ�) ]
	else if (map[1][sw2_obj.pos.row][sw2_obj.pos.column] == 'H' && check_friend[sw2_obj.pos.row][sw2_obj.pos.column] == 2) {
		send_system_message[0] = "������� ���� �Ϻ����͸� ��ƸԾ����ϴ�.";
		p_system_message(send_system_message[0], sysmes_map);

		// �ӽ÷� repr ����
		e_hav_obj.repr = ' ';
	}

	// [ ������� ������ �� (���� ���� X) ]
	else if (map[1][sw2_obj.pos.row][sw2_obj.pos.column] == 'F') {
		send_system_message[0] = "������� �������� ��ƸԾ����ϴ�.";
		p_system_message(send_system_message[0], sysmes_map);


	}

	// [ ����� ������ �� (���� ���� X) ]
	else if (map[1][sw2_obj.pos.row][sw2_obj.pos.column] == 'F') {
		send_system_message[0] = "������� ���縦 ��ƸԾ����ϴ�.";
		p_system_message(send_system_message[0], sysmes_map);


	}

	// [ ���� ������ �� (���� ���� X) ]
	else if (map[1][sw2_obj.pos.row][sw2_obj.pos.column] == 'S') {
		send_system_message[0] = "������� ������ ��ƸԾ����ϴ�.";
		p_system_message(send_system_message[0], sysmes_map);


	}

	// [ ������ ������ �� (���� ���� X) ]
	else if (map[1][sw2_obj.pos.row][sw2_obj.pos.column] == 'T') {
		send_system_message[0] = "������� �������� ��ƸԾ����ϴ�.";
		p_system_message(send_system_message[0], sysmes_map);


	}

	map[1][sw2_obj.pos.row][sw2_obj.pos.column] = sw2_obj.repr;
	sw2_obj.next_move_time = sys_clock + sw2_obj.move_period;
}


// [ �� ��ǳ ]

POSITION sand_wind_next_pos(void) {
	// �� ��ǳ ���� ��ġ
	int np_c = rand() % 14 + 3; // ���� 3 ~ 14 ������ ������ ��
	int np_r = rand() % 55 + 3; // �ʺ� 3 ~ 55 ������ ������ ��

	// �� ��ǳ ������ ��ġ
	int nd_c = rand() % 14 + 3; // ���� 3 ~ 14 ������ ������ ��
	int nd_r = rand() % 53 + 3; // �ʺ� 3 ~ 55 ������ ������ ��

	// �� ��ǳ ���� ��ġ ����
	POSITION random_pos1 = { np_c - 1, np_r - 1 };
	POSITION random_pos2 = { np_c, np_r - 1 };
	POSITION random_pos3 = { np_c - 1, np_r };
	POSITION random_pos4 = { np_c, np_r };
	sand_wind.pos1 = random_pos1;
	sand_wind.pos2 = random_pos2;
	sand_wind.pos3 = random_pos3;
	sand_wind.pos4 = random_pos4;
	
	// �� ��ǳ ������ ��ġ ����
	POSITION random_dest1 = { nd_c - 1, nd_r - 1 };
	POSITION random_dest2 = { nd_c, nd_r - 1 };
	POSITION random_dest3 = { nd_c - 1, nd_r };
	POSITION random_dest4 = { nd_c, nd_r };
	sand_wind.dest1 = random_dest1;
	sand_wind.dest2 = random_dest2;
	sand_wind.dest3 = random_dest3;
	sand_wind.dest4 = random_dest4;

	// ���� ��ġ�� �������� ���ؼ� �̵� ���� ����	
	POSITION diff = psub(sand_wind.dest4, sand_wind.pos4);
	DIRECTION dir;

	// ������, ������ �Ÿ��� ���ؼ� �� �� �� ������ �̵�
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// �� ��ǳ �̵�
	POSITION next_pos1 = pmove(sand_wind.pos1, dir);
	POSITION next_pos2 = pmove(sand_wind.pos2, dir);
	POSITION next_pos3 = pmove(sand_wind.pos3, dir);
	POSITION next_pos4 = pmove(sand_wind.pos4, dir);

	// �� ��ǳ�� 'T' �϶��� �̵� (�������� �����ϸ� ' '���� �������� ��� �����Ұű� ����)
	if (1 <= next_pos4.row && next_pos4.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos4.column && next_pos4.column <= MAP_WIDTH - 2 && sand_wind.repr == 'Z') { // pos4�� ���� �����̱� ������ pos4�� �˻�.
		
		return next_pos1, next_pos2, next_pos3, next_pos4;
	}
	

	return next_pos1, next_pos2, next_pos3, next_pos4;

}

void sand_wind_move(void) {
	// �����̴� �ֱ�
	if (sys_clock <= sand_wind.next_move_time) {
		return;
	}
	// [ �� ��ǳ layer2 (map[2])�� ���� ]
	map[2][sand_wind.pos1.row][sand_wind.pos1.column] = -1;
	map[2][sand_wind.pos2.row][sand_wind.pos2.column] = -1;
	map[2][sand_wind.pos3.row][sand_wind.pos3.column] = -1;
	map[2][sand_wind.pos4.row][sand_wind.pos4.column] = -1;

	sand_wind.pos1 = sand_wind_next_pos();
	sand_wind.pos2 = sand_wind_next_pos();
	sand_wind.pos3 = sand_wind_next_pos();
	sand_wind.pos4 = sand_wind_next_pos();

	sand_wind.repr = 'Z';
	map[1][sand_wind.pos1.row][sand_wind.pos1.column] = sand_wind.repr;
	map[1][sand_wind.pos2.row][sand_wind.pos2.column] = sand_wind.repr;
	map[1][sand_wind.pos3.row][sand_wind.pos3.column] = sand_wind.repr;
	map[1][sand_wind.pos4.row][sand_wind.pos4.column] = sand_wind.repr;

	// [ �� ��ǳ�� ���� �ǹ�, ���� �ൿ ]
	// < �Ʊ� �Ϻ����� > (�̿ϼ�)
	if ((map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'H' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'H' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'H' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'H') && \
		check_friend[sand_wind.pos4.row][sand_wind.pos4.column] == 1) {
		
		map[1][sand_wind.pos1.row][sand_wind.pos1.column] = ' ';
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] = ' ';
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] = ' ';
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] = ' ';
		
		// �ý��� �޽���
		send_system_message[0] = "�� ��ǳ�� ���� �Ʊ� �Ϻ����Ͱ� ����߽��ϴ�.";
		p_system_message(send_system_message[0], sysmes_map);

	}
	// < ���� �Ϻ����� > (�̿ϼ�)
	if ((map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'H' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'H' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'H' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'H') && \
		check_friend[sand_wind.pos4.row][sand_wind.pos4.column] == 2) {

		map[1][sand_wind.pos1.row][sand_wind.pos1.column] = ' ';
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] = ' ';
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] = ' ';
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] = ' ';
		
		// �ý��� �޽���
		send_system_message[0] = "�� ��ǳ�� ���� ���� �Ϻ����Ͱ� ����߽��ϴ�.";
		p_system_message(send_system_message[0], sysmes_map);

	}
	// < ������ > (���� ���� X)
	else if (map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'F' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'F' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'F' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'F') {



		// �ý��� �޽���
		send_system_message[0] = "�� ��ǳ�� ���� �������� ����߽��ϴ�.";
		p_system_message(send_system_message[0], sysmes_map);

	}
	// < ���� > (���� ���� X)
	else if (map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'S' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'S' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'S' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'S') {



		// �ý��� �޽���
		send_system_message[0] = "�� ��ǳ�� ���� ������ ����߽��ϴ�.";
		p_system_message(send_system_message[0], sysmes_map);

	}
	// < ���� > (���� ���� X)
	else if (map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'F' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'F' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'F' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'F') {


		// �ý��� �޽���
		send_system_message[0] = "�� ��ǳ�� ���� ���簡 ����߽��ϴ�.";
		p_system_message(send_system_message[0], sysmes_map);

	}
	// < ������ > (���� ���� X)
	else if (map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'T' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'T' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'T' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'T') {

		// �ý��� �޽���
		send_system_message[0] = "�� ��ǳ�� ���� �������� �μ������ϴ�.";
		p_system_message(send_system_message[0], sysmes_map);

	}
	// < ���� > (���� ���� X)
	else if (map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'D' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'D' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'D' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'D') {


		// �ý��� �޽���
		send_system_message[0] = "�� ��ǳ�� ���� ���Ұ� �ڻ쳵���ϴ�.";
		p_system_message(send_system_message[0], sysmes_map);

	}
	// < â�� > (���� ���� X)
	else if (map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'G' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'G' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'G' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'G') {


		// �ý��� �޽���
		send_system_message[0] = "�� ��ǳ�� ���� â�� �ڻ쳵���ϴ�.";
		p_system_message(send_system_message[0], sysmes_map);

	}
	// < ���� > (���� ���� X)
	else if (map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'B' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'B' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'B' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'B') {


		// �ý��� �޽���
		send_system_message[0] = "�� ��ǳ�� ���� ������ �ڻ쳵���ϴ�.";
		p_system_message(send_system_message[0], sysmes_map);

	}
	// < ����ó > (���� ���� X)
	else if (map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'S' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'S' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'S' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'S') {


		// �ý��� �޽���
		send_system_message[0] = "�� ��ǳ�� ���� ����ó�� �ڻ쳵���ϴ�.";
		p_system_message(send_system_message[0], sysmes_map);

	}
	// < ������ > (���� ���� X)
	else if (map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'A' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'A' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'A' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'A') {


		// �ý��� �޽���
		send_system_message[0] = "�� ��ǳ�� ���� �������� �ڻ쳵���ϴ�.";
		p_system_message(send_system_message[0], sysmes_map);

	}
	// < ���� > (���� ���� X)
	else if (map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'F' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'F' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'F' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'F') {
		


		// �ý��� �޽���
		send_system_message[0] = "�� ��ǳ�� ���� ������ �ڻ쳵���ϴ�.";
		p_system_message(send_system_message[0], sysmes_map);

	}


	// [ �� ��ǳ�� �������� �������� �� (����ǳ�� ȭ�鿡�� ���־���) ]
	if (sand_wind.pos4.row == sand_wind.dest4.row && sand_wind.pos4.column == sand_wind.dest4.column) {
		sand_wind.repr = ' ';
		map[1][sand_wind.pos1.row][sand_wind.pos1.column] = sand_wind.repr;
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] = sand_wind.repr;
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] = sand_wind.repr;
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] = sand_wind.repr;
		
		// �ý��� �޽���
		send_system_message[0] = "�� ��ǳ�� ��������ϴ�..";
		p_system_message(send_system_message[0], sysmes_map);
	}

	sand_wind.next_move_time = sys_clock + sand_wind.move_period;
}




