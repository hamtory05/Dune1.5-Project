// ���� �� �ڵ�� 6�� ���� �� �� �����Դϴ�. �� �ǹ��� ���� ����â�� ��ɾ �־��ָ� �˴ϴ�. 
// 5�������� �ϼ��߽��ϴ�.


#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include "common.h"
#include "io.h"
#include "display.h"

// [ INIT ]
void init_map(void);
void init_state(void);
void init_sysmes(void);
void init_order(void);
void init_unit(void);
void init(void);

// [ INTRO, OUTRO ]
void intro(void);
void outro(void);

// [ CURSOR ]
void cursor_move(DIRECTION dir, int move);

// [ SAMPLE OBJ ]
void sample_obj_move(void);
POSITION sample_obj_next_position(void);

// [ ����� ]
POSITION sw1_next_pos(void);
void sw1_move(void);

POSITION sw2_next_pos(void);
void sw2_move(void);

// [ �縷 ������ ]
POSITION d_eagle_next_pos(void);
void d_eagle_move(void);

// [ �� ��ǳ (X) ]
POSITION sand_wind_next_pos(void);
void sand_wind_move(void);

// [ �ý��� �޽��� ]
char* send_system_message[1];

// [ KEY RESET ]
void reset_key_false();

// ==== [ �Ϻ����� & �����̽� ���� ] ====
// [ �Ʊ� ]
OBJECT_SAMPLE* havs[MAX_HAV]; // �Ϻ����� �迭
int hav_count = 1; // �Ϻ����� ����
int selected_harvester = -1; // ���õ� �Ϻ����� �ε��� (-1: ���� �ȵ�)

// [ ���� ]
OBJECT_SAMPLE* e_havs[MAX_HAV]; // �Ϻ����� �迭
int e_hav_count = 1; // �Ϻ����� ����


void hav_reset(void); // �Ϻ����� �ʱ�ȭ
void harvest_move_all(OBJECT_SAMPLE havs[MAX_HAV], int hav_count, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], OBJECT_BUILDING* SPICE); // �Ϻ����� �̵� ����
int harvest_gather_spice(OBJECT_BUILDING* spice, OBJECT_SAMPLE* hav, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]); // �Ϻ����� �����̽� ��Ȯ
POSITION harvest_next_move(OBJECT_SAMPLE* hav, POSITION new_dest); // �Ϻ����� �̵�
POSITION harvest_set_dest(POSITION curr, OBJECT_SAMPLE* hav, OBJECT_BUILDING* SPICE); // �Ϻ����� ������ ���ϱ�
void handle_harvester_input(CURSOR cursor, OBJECT_SAMPLE havs[MAX_HAV], int hav_count, OBJECT_BUILDING* SPICE); // �Ϻ����� ���� �� �����̽� ����

int spice_in_progress[MAX_SPICE] = { -1 }; // �� �����̽� �������� ���� �۾� ���� (-1: �۾� ����)


// ==== [ ���� ] ====
OBJECT_SAMPLE* sold[MAX_SOLD]; // ���� �迭
int sold_count = 0; // ���� ���� 
int selected_sold = -1; // ������ ���õǾ����� Ȯ��

extern int base_barr_check[MAP_HEIGHT][MAP_WIDTH]; 

bool sold_p_key_press = false; // ���� ���� pŰ bool
bool sold_e_hav_find = false;
bool sold_fighter_find = false;
bool sold_tank_find = false;


void select_sold_act(void); // ���� �ൿ ���� 
void sold_attack_building(void); // ���� �ǹ� ����
void sold_attack_unit(void); // ���� ���� ����
void process_enemy_unit(int row, int col, OBJECT_SAMPLE* soldier); // ���� ���� ó��
void handle_enemy_harvester(OBJECT_SAMPLE* soldier, OBJECT_SAMPLE* harvester); // ���� �Ϻ����� ����
void handle_enemy_fighter(OBJECT_SAMPLE* soldier, OBJECT_SAMPLE* fighter); // ���� ���� ����
void handle_enemy_tank(OBJECT_SAMPLE* soldier, OBJECT_SAMPLE* tank); // ���� ������ ����
POSITION sold_move(OBJECT_SAMPLE* sold_unit, POSITION target_pos); // ���� �̵� 
void main_sold_func(void); // ���� ���� �Լ�


// ==== [ ������ ] ====
OBJECT_SAMPLE* frem[MAX_FREM]; // ������ �迭
int frem_count = 0; // ������ ���� 
int selected_frem = -1; // ������ ���õǾ����� Ȯ��
extern int frem_fight_fact_check[MAP_HEIGHT][MAP_WIDTH];

bool frem_p_key_press = false; // ������ ���� pŰ �������� Ȯ���ϴ� bool
bool frem_e_hav_find = false;
bool frem_fighter_find = false;
bool frem_tank_find = false;
bool frem_find_enemy = false;

void select_frem_act(void); // ������ �ൿ ���� 
void frem_attack_building(void); // ������ �ǹ� ���� 
void frem_attack_unit(void); // ������ ���� ����
void frem_kill_enemy_hav(OBJECT_SAMPLE* frem, OBJECT_SAMPLE* harvester); // ������ �Ϻ����� ����
void frem_kill_enemy_fighter(OBJECT_SAMPLE* frem, OBJECT_SAMPLE* fighter); // ������ ���� ����
void frem_kill_enemy_tank(OBJECT_SAMPLE* frem, OBJECT_SAMPLE* tank); // ������ ������ ����
POSITION frem_move(OBJECT_SAMPLE* frem, POSITION target_pos); // ������ �̵�
void frem_main_func(void); // ������ �̵� ���� �Լ�


// [ ���� ]
OBJECT_SAMPLE* fighter[MAX_FIGHTER]; // ���� �迭
int fighter_count = 1; // ���� ����

OBJECT_SAMPLE fighter1 = {
	.pos = { 7, 51 },
	.dest = {-1, -1},
	.repr = 'F',
	.move_period = 1200,
	.next_move_time = 1200,
	.dps = 6,
	.attack_period = 600,
	.next_attack_time = 600,
	.hp = 10,
	.eyes = 1
};


// [ ������ ]
OBJECT_SAMPLE* tank[MAX_TANK]; // ������ �迭
int tank_count = 1; // ������ ����

OBJECT_SAMPLE tank1 = {
	.pos = {7, 46},
	.dest = {-1, -1},
	.repr = 'T',
	.move_period = 3000,
	.next_move_time = 3000,
	.hp = 60,
	.eyes = 4,
	.dps = 40,
	.attack_period = 4000,
	.next_attack_time = 4000
};

// [ �ǹ� �ҷ����� ]
extern OBJECT_BUILDING e_base; // ���� ����

extern OBJECT_BUILDING* SPICE; // �����̽�
extern int spi_count; // �����̽� ����

extern OBJECT_BUILDING* DOR[MAX_DOR]; // ����
extern int dor_count; // ���� ����

extern OBJECT_BUILDING* GAR[MAX_GAR]; // â�� 
extern int gar_count; // â�� ����

extern OBJECT_BUILDING* BAR[MAX_BAR]; // ����
extern int bar_count; // ���� ����

extern OBJECT_BUILDING* SHE[MAX_SHE]; // ����ó
extern int she_count; // ����ó ����

extern OBJECT_BUILDING* ARE[MAX_ARE]; // �Ʒ���
extern int are_count; // �Ʒ��� ����

extern OBJECT_BUILDING* FAC[MAX_FAC]; // ����
extern int fac_count; // ���� ����

extern OBJECT_BUILDING* E_DOR[MAX_DOR]; // ���� ����
extern int e_dor_count; // ���� ���� ����

extern OBJECT_BUILDING* E_GAR[MAX_GAR]; // ���� â��
extern int e_gar_count; // ���� â�� ����

extern shle_sold_check[MAP_HEIGHT][MAP_WIDTH]; // ����ó  - 1, ���� - 2 ����


/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
CURSOR cursor = { { 1, 1 }, {1, 1} };

int move_count = 0;
int move_check[2] = { 0 };
char spice_number[9] = { '1', '2', '3', '4', '5', '6', '7', '8', '9' }; // �����̽� ����
int spice_plus_value = 0; // ���� �����̽� ����
int* spice_plus = &spice_plus_value; // �����̽� ���� �� ���� ����

/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };  // 2, 16, 60
char state_map[STATE_HEIGHT][STATE_WIDTH] = { 0 }; // 16, 50
char sysmes_map[SYSMES_HEIGHT][SYSMES_WIDTH] = { 0 }; // 6 60
char order_map[ORDER_HEIGHT][ORDER_WIDTH] = { 0 }; // 6 60
int check_friend[MAP_HEIGHT][MAP_WIDTH] = { 0 }; // 0 --> �ƹ��͵� �ƴ�, 1 --> �Ʊ�, 2 --> ����
int hav_gather_spice[MAX_HAV] = { 0 }; // 0 �����̽��� ĳ�� ���� / 1�� �����̽��� ĵ ��
POSITION pre_hav_dest[MAX_HAV] = { 0 };

bool b_key_press = false; // ���� ��ġ �� �� bŰ�� �������� Ȯ��
bool big_cursor = false; // 2X2 Ŀ�� Ȱ��ȭ, ��Ȱ��ȭ
bool p_key_press = false; // ���� ��ġ Ȯ��
bool d_key_press = false; // ���� ��ġ Ȯ��
bool g_key_press = false; // â�� ��ġ Ȯ��
bool b_b_key_press = false; // ���� ��ġ Ȯ��
bool s_key_press = false;  // ����ó ��ġ Ȯ��
bool space_key_press = false; // �����̽��ٰ� ���ȴ��� Ȯ�� / false = ��Ȱ��ȭ, true = Ȱ��ȭ
bool hav_move = false; // �Ϻ����� �̵� Ȯ��
bool h_key_press = false; // hŰ ���ȴ��� Ȯ��
bool space_and_h_key = false; // �����̽� ������ hŰ �������� Ȯ��
bool space_and_m_key = false; // �����̽� ������ mŰ �������� Ȯ��
bool m_key_press = false; // �̵� Ȯ��
bool p_p_key_press = false; // ���� Ȯ��


// [ �ڿ� ]
RESOURCE resource = {
	.spice = 10,
	.spice_max = 50,
	.population = 5,
	.population_max = 30
};

// [ obj �⺻ Ʋ ]
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
	.dest = {14, 1},
	.repr = 'H',
	.move_period = 2000,
	.next_move_time = 2000,
	.color = COLOR_DEFAULT + 16,
	.hp = 70,
	.dps = 0,
	.attack_period = 0,
	.eyes = 0, 
	.gather_time = 4000,
	.gather_next_time = 4000
}; 

// [ ���� �Ϻ����� ]
OBJECT_SAMPLE e_hav_obj = {
	.pos = {3, 58},
	.dest = {3, 58},
	.repr = 'H',
	.move_period = 2000,
	.next_move_time = 2000,
	.color = COLOR_DEFAULT + 64,
	.hp = 70,
	.dps = 0,
	.attack_period = 0,
	.eyes = 0,
	.gather_time = 4000,
	.gather_next_time = 4000
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

// [ ���ڳ� ]
void hako_unit(void);
void hako_unit_reset(void);

/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL));

	hav_reset(); // �Ϻ����� �ʱ�ȭ
	init();
	intro();
	display(resource, map, cursor, state_map, sysmes_map, order_map, check_friend, big_cursor);
	p_building(map, check_friend); // �ǹ� ���
	// [ ���ڳ� ]
	hako_building(map, check_friend); 
	hako_unit();

	// [ �ý��� �޽��� �߰� ]
	p_system_message("������ ���۵Ǿ����ϴ�.");

	while (1) {
		// [ loop �� ������(��, TICK==10ms����) Ű �Է� Ȯ�� ]
		KEY key = get_key();

		// [ sys_clock ���� ���� Ŀ�� ����ĭ �̵� ���� �ʱ�ȭ ]
		if (sys_clock % 110 == 0) {
			move_count = 0; // Ŀ���� 1ĭ �����̴��� 3ĭ �����̴��� Ȯ��
			for (int i = 0; i < 2; i++) {
				move_check[i] = 0; // 3ĭ �����̴� Ŀ���� ��, ������ ����Ű�� �ι� �������� Ȯ��
			}
		}

		// [ Ű �Է��� ���� ��, 1ĭ�̵�����, 3ĭ�̵����� Ȯ�� ]
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
		
		// [ Ű �Է��� ������ ó�� ]
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
			// [ Q ]
			case k_quit: outro();
			
			// [ ESC ]
			case k_esc: state_esc();
				// [ �ý��� �޽��� �߰� ]
				p_system_message("ESCŰ�� �������ϴ�.");
				b_key_press = false;
				space_key_press = false;

				reset_key_false();
				break;

			// [ SPACE ]
			case k_space: 
				space_key_press = true;
				state_spacebar(&resource, cursor, &f_hav_obj, &e_hav_obj, check_friend, p_key_press, map, d_key_press, g_key_press, 
				b_b_key_press, s_key_press, havs, &space_key_press);
				// [ �ý��� �޽��� �߰� ]
				p_system_message("�����̽��ٸ� �������ϴ�.");

				// [ KEY FALSE ]
				reset_key_false();
				break;

			// [ H ]
			case k_h: press_h(&resource, map, check_friend, cursor, &f_hav_obj, hav_count, havs);
				h_key_press = true;
				// [ �ý��� �޽��� �߰� ]
				p_system_message("HŰ�� �������ϴ�.");
				break;

			// [ B ]
			case k_b: press_b(&resource, cursor, check_friend);

				// [ ���� ]
				if (map[0][cursor.current.row][cursor.current.column] == 'P') {
					if (b_key_press) { // bŰ�� ������ ��
						press_b_b();
						b_b_key_press = true; // ���� ��ġ Ȱ��ȭ 
						b_key_press = false;  // bŰ ��Ȱ��ȭ
					}
				}

				// [ �ý��� �޽��� �߰� ]
				p_system_message("BŰ�� �������ϴ�.");
				b_key_press = true; // �ǹ� Ű �Է�
				break;

			// [ P ]
			case k_p: 
				// [ �ý��� �޽��� �߰� ]
				p_system_message("PŰ�� �������ϴ�.");
				
				// [ ���� �Ǽ� ]
				if (map[0][cursor.current.row][cursor.current.column] == ' ') {
					if (b_key_press) {
						big_cursor = true;
						p_key_press = true;
						press_p();
					}
				}
				// [ ����, ������ ���� ���� ]
				if (map[0][cursor.current.row][cursor.current.column] == ' ') {
					sold_p_key_press = true;
					frem_p_key_press = true;
				}
				
				b_key_press = false;
				break;

			// [ D ]
			case k_d: 
				// [ �ý��� �޽��� �߰� ]
				p_system_message("DŰ�� �������ϴ�.");
				
				if (map[0][cursor.current.row][cursor.current.column] == 'P') {
					if (b_key_press) {
						press_d();
						big_cursor = true;
						d_key_press = true;
					}
				}
				b_key_press = false;
				break;

			// [ G ]
			case k_g: 
				// [ �ý��� �޽��� �߰� ]
				p_system_message("GŰ�� �������ϴ�.");
				if (map[0][cursor.current.row][cursor.current.column] == 'P') {
					if (b_key_press) {
						press_g(); 
						big_cursor = true;
						g_key_press = true;
					}
				}
				b_key_press = false;
				break;

			// [ S ]
			case k_s: 
				// [ �ý��� �޽��� �߰� ]
				p_system_message("SŰ�� �������ϴ�.");

				// [ ����ó ]
				if (map[0][cursor.current.row][cursor.current.column] == 'P') {
					if (b_key_press) {
						press_s();
						big_cursor = true;
						s_key_press = true;
					}
				}

				// [ ���� ]
				press_s_s(cursor, &resource, map, &space_key_press);
				
				space_key_press = false;
				b_key_press = false;
				break;

			// [ M ]
			case k_m:
				m_key_press = true;
				p_system_message("MŰ�� �������ϴ�.");
				break;

			// [ F ]
			case k_f:
				p_system_message("FŰ�� �������ϴ�.");
				press_f(cursor, &resource, map, &space_key_press);
				break;

			case k_none:
			case k_undef:
			default: break;
			}
		}
	
		// [ ������Ʈ ���� ]
		//d_eagle_move();
		//sw1_move();
		//sw2_move(); 

		// [ �Ϻ����� �����̱� ]
		harvest_move_all(*havs, hav_count, map, SPICE);

		// [ ���� �����̱� ]
		main_sold_func();
		
		// [ ������ �����̱� ]
		frem_main_func();

		// [ ȭ�� ��� ]
		display(resource, map, cursor, state_map, sysmes_map, order_map, check_friend, big_cursor);
		Sleep(TICK);
		sys_clock += 10;
	}
}


/* ================= [ subfunctions ] =================== */

// [ �ʱ� �Ϻ����� �ʱ�ȭ ]
void hav_reset(void) {
	for (int i = 0; i < MAX_HAV; i++) {
		havs[i] = NULL; // ������ �ʱ�ȭ
	}

	for (int i = 0; i < MAX_HAV; i++) {
		e_havs[i] = NULL; // ������ �ʱ�ȭ
	}

	e_havs[0] = &e_hav_obj; // ù ��° �Ϻ����� �ʱ�ȭ
	havs[0] = &f_hav_obj; // ù ��° �Ϻ����� �ʱ�ȭ
}

// [ ��Ʈ�� ]
void intro(void) {
	printf("DUNE 1.5\n");
	Sleep(2000);
	system("cls");
}

// [ �ƿ�Ʈ�� ]
void outro(void) {
	// ȭ�� ��ü�� �ϴ� ��ĭ���� ���� �� ���� ������� ����ϰ� �ϱ�
	printf("exiting...\n");
	exit(0);
}

// [ RESET KEY FALSE ]
void reset_key_false() {
	big_cursor = false;
	p_key_press = false;
	d_key_press = false;
	g_key_press = false;
	b_b_key_press = false;
	s_key_press = false;
}

// [ INIT ]
void init_map(void) {
	// [ layer 0(map[0])�� ���� ���� ]
	for (int j = 0; j < MAP_WIDTH; j++) {
		map[0][0][j] = '#';
		map[0][MAP_HEIGHT - 1][j] = '#';
	}

	// [ layer 0(map[0])�� ���� ���� ]
	for (int i = 1; i < MAP_HEIGHT - 1; i++) {
		map[0][i][0] = '#';
		map[0][i][MAP_WIDTH - 1] = '#';
		for (int j = 1; j < MAP_WIDTH - 1; j++) {
			map[0][i][j] = ' ';
		}
	}

	// [ layer 1(map[1])�� ��� �α�(-1�� ä��) ]
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[1][i][j] = -1;
		}
	}

	// [ layer 2(map[2])�� ��� �α�(-1�� ä��) ]
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[2][i][j] = -1;
		}
	}
}

void init_state(void) {
	// [ ����â �⺻ Ʋ ]
	for (int i = 0; i < STATE_WIDTH; i++) {
		state_map[0][i] = '#';
		state_map[STATE_HEIGHT - 1][i] = '#';
	}

	// [ ����â �⺻ Ʋ ]
	for (int i = 1; i < STATE_HEIGHT - 1; i++) {
		state_map[i][0] = '#';
		state_map[i][STATE_WIDTH - 1] = '#';
		for (int j = 1; j < STATE_WIDTH - 1; j++) {
			state_map[i][j] = ' ';
		}
	}
} 

void init_sysmes(void) {
	// [ �ý��� �⺻ Ʋ ]
	for (int i = 0; i < SYSMES_WIDTH; i++) {
		sysmes_map[0][i] = '#';
		sysmes_map[SYSMES_HEIGHT - 1][i] = '#';
	}

	// [ �ý��� �⺻ Ʋ ]
	for (int i = 1; i < SYSMES_HEIGHT - 1; i++) {
		sysmes_map[i][0] = '#';
		sysmes_map[i][SYSMES_WIDTH - 1] = '#';
		for (int j = 1; j < SYSMES_WIDTH - 1; j++) {
			sysmes_map[i][j] = ' ';
		}
	}
} 

void init_order(void) {
	// [ ���â �⺻ Ʋ ]
	for (int i = 0; i < ORDER_WIDTH; i++) {
		order_map[0][i] = '#';
		order_map[ORDER_HEIGHT - 1][i] = '#';
	}

	// [ ���â �⺻ Ʋ ]
	for (int i = 1; i < ORDER_HEIGHT - 1; i++) {
		order_map[i][0] = '#';
		order_map[i][ORDER_WIDTH - 1] = '#';
		for (int j = 1; j < ORDER_WIDTH - 1; j++) {
			order_map[i][j] = ' ';
		}
	}
} 

void init_unit(void) {
	// [ �縷 ������ ]
	//map[2][d_eagle.pos.row][d_eagle.pos.column] = d_eagle.repr;

	// [ �Ʊ� �Ϻ����� ]
	map[1][f_hav_obj.pos.row][f_hav_obj.pos.column] = f_hav_obj.repr; 
	check_friend[f_hav_obj.pos.row][f_hav_obj.pos.column] = 1;

	// [ ���� �Ϻ����� ]
	map[1][e_hav_obj.pos.row][e_hav_obj.pos.column] = e_hav_obj.repr;
	check_friend[e_hav_obj.pos.row][e_hav_obj.pos.column] = 2;

	// [ ����� ]
	map[1][sw1_obj.pos.row][sw1_obj.pos.column] = sw1_obj.repr; 
	map[1][sw2_obj.pos.row][sw2_obj.pos.column] = sw2_obj.repr; 
} 

void init(void) {
	// [ MAP ]
	init_map();

	// [ STATE ]
	init_state();

	// [ SYSMES ]
	init_sysmes();

	// [ ORDER ]
	init_order();

	// [ UNIT ]
	init_unit();
}

// [ Ŀ�� �̵� ]
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


/* ================= [ ���� ���� �Լ� ] =================== */
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


/* ================= [ ���� ������ �Լ� ] =================== */

// [ �縷 ������ (X) ]
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

// [ �縷 ������ ������ �Լ� (X ]
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
	// [ ���� ��ġ�� �������� ���ؼ� �̵� ���� ���� ]
	POSITION diff = psub(sw1_obj.dest, sw1_obj.pos);
	DIRECTION dir;

	// [ ���� ����� ������ ã�Ƽ� �� ������ ��İ��� new_dest�� ���� ]
	double check_close = 61.0;
	int move_i = 2, move_j = 4;
	for (int i = 1; i < MAP_HEIGHT - 1; i++) {
		for (int j = 1; j < MAP_WIDTH - 1; j++) {
			if (map[1][i][j] == 'H' || map[1][i][j] == 'S' || (map[1][i][j] == 'F' && 1 /* 1�� ���߿� ���� ������� ���� ���� */)) {
				if (sqrt(pow(i - sw1_obj.pos.row, 2) + pow(j - sw1_obj.pos.column, 2)) < check_close) {
					check_close = sqrt(pow(i - sw1_obj.pos.row, 2) + pow(j - sw1_obj.pos.column, 2));
					move_i = i, move_j = j;
				}
			}
		}
	}
	POSITION new_dest = { move_i, move_j };
	sw1_obj.dest = new_dest;

	// [ ������, ������ �Ÿ��� ���ؼ� �� �� �� ������ �̵� ]
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// [ ����� �̵� ]
	POSITION next_pos = pmove(sw1_obj.pos, dir);

	// [ �ǹ�, ��ֹ� ������ �� ���ذ��� ]
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[0][next_pos.row][next_pos.column] == 'R' || map[0][next_pos.row][next_pos.column] == '1' || map[0][next_pos.row][next_pos.column] == '2' || \
		map[0][next_pos.row][next_pos.column] == '3' || map[0][next_pos.row][next_pos.column] == '4' || map[0][next_pos.row][next_pos.column] == '5' || \
		map[0][next_pos.row][next_pos.column] == '6' || map[0][next_pos.row][next_pos.column] == '7' || map[0][next_pos.row][next_pos.column] == '8' || \
		map[0][next_pos.row][next_pos.column] == '9' || map[0][next_pos.row][next_pos.column] == 'B' || map[0][next_pos.row][next_pos.column] == 'P' || \
		map[0][next_pos.row][next_pos.column] == 'S' || map[0][next_pos.row][next_pos.column] == 'G' || map[0][next_pos.row][next_pos.column] == 'D' || \
		map[0][next_pos.row][next_pos.column] == 'A' || map[0][next_pos.row][next_pos.column] == 'S' || map[0][next_pos.row][next_pos.column] == 'F') {

		// [ �ǹ�, ��ֹ� ��, �Ʒ��� ���� �� ]
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
		// [ �ǹ�, ��ֹ� ������, ���ʿ� ���� �� ]
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

// [����� (1) ������ �Լ� ]
void sw1_move(void) {
	// [ �����̴� �ֱ� ]
	if (sys_clock <= sw1_obj.next_move_time) {
		return;
	}
	// [ �����(1) layer1 (map[1])�� ���� ]
	map[1][sw1_obj.pos.row][sw1_obj.pos.column] = -1;

	// [ 10/300 Ȯ���� �輳 --> �����̽� ������ ���� (���差 1 ~ 9 ����) ]
	int r = rand() % 299;
	if (r < 9) {
		// [ �ý��� �޽��� �߰� ]
		send_system_message[0] = "������� �����̽� �������� �����߽��ϴ�.";
		p_system_message(send_system_message[0]);

		OBJECT_BUILDING new_spice = {
			.pos1 = {sw1_obj.pos.row, sw1_obj.pos.column},
			.repr = spice_number[r],
			.layer = 0
		};

		SPICE[spi_count] = new_spice;
		map[0][sw1_obj.pos.row][sw1_obj.pos.column] = SPICE[spi_count].repr;
		spi_count++;
	}
	sw1_obj.pos = sw1_next_pos();

	// [ ����� ��ƸԱ� ]
	// [ �Ʊ� �Ϻ����Ϳ� ������ �� ]
	for (int i = 0; i < hav_count; i++) {
		if (map[1][sw1_obj.pos.row][sw1_obj.pos.column] == 'H'  && check_friend[sw1_obj.pos.row][sw1_obj.pos.column] == 1) {
			// [ �ý��� �޽��� �߰� ]
			send_system_message[0] = "������� �Ʊ� �Ϻ����͸� ��ƸԾ����ϴ�.";
			p_system_message(send_system_message[0]);

			// [ �Ϻ����� �ʿ��� ������� ]
			havs[i]->repr = ' ';

			// [ �ӽ÷� ����� ��ġ �ٲٱ� ]
			POSITION new_dest = { 2, 4 };
			sw1_obj.dest = new_dest;

		}
	}
	
	// [ ���� �Ϻ����Ϳ� ������ �� ]
	for (int i = 0; i < e_hav_count; i++) {
		if (map[1][sw1_obj.pos.row][sw1_obj.pos.column] == 'H' && check_friend[sw1_obj.pos.row][sw1_obj.pos.column] == 2) {
			// [ �ý��� �޽��� �߰� ]
			send_system_message[0] = "������� ���� �Ϻ����͸� ��ƸԾ����ϴ�.";
			p_system_message(send_system_message[0]);

			// [ �Ϻ����� �ʿ��� ������� ]
			e_havs[i]->repr = ' ';
			resource.population -= 5;
			// [ �ӽ÷� ����� ��ġ �ٲٱ� ]
			POSITION new_dest = { 2, 4 };
			sw1_obj.dest = new_dest;
		}
	}
	

	// [ ����� ������ �� ]
	for (int i = 0; i < fighter_count; i++) {
		if (map[1][sw1_obj.pos.row][sw1_obj.pos.column] == 'F' && \
			frem_fight_fact_check[sw1_obj.pos.row][sw1_obj.pos.column] == 3) {
			// [ �ý��� �޽��� �߰� ]
			send_system_message[0] = "������� ���縦 ��ƸԾ����ϴ�.";
			p_system_message(send_system_message[0]);

			// [ ���� �ʿ��� ������� ]
			fighter[i]->repr = ' ';
			
			// [ �ӽ÷� ����� ��ġ �ٲٱ� ]
			POSITION new_dest = { 2, 4 };
			sw1_obj.dest = new_dest;
		}
	}
	

	// [ ���� ������ �� ]
	for (int i = 0; i < sold_count; i++) {
		if (map[1][sw1_obj.pos.row][sw1_obj.pos.column] == 'S' && \
			shle_sold_check[sw1_obj.pos.row][sw1_obj.pos.column] == 2) {
			// [ �ý��� �޽��� �߰� ]
			send_system_message[0] = "������� ������ ��ƸԾ����ϴ�.";
			p_system_message(send_system_message[0]);

			// [ ���� �ʿ��� ������� ]
			sold[i]->repr = ' ';
			resource.population -= 1;

			// [ �ӽ÷� ����� ��ġ �ٲٱ� ]
			POSITION new_dest = { 2, 4 };
			sw1_obj.dest = new_dest;
		}
	}
	

	map[1][sw1_obj.pos.row][sw1_obj.pos.column] = sw1_obj.repr;
	sw1_obj.next_move_time = sys_clock + sw1_obj.move_period;
}


// [ ����� (2) ]
POSITION sw2_next_pos(void) {
	// [ ���� ��ġ�� �������� ���ؼ� �̵� ���� ���� ]
	POSITION diff = psub(sw2_obj.dest, sw2_obj.pos);
	DIRECTION dir;

	// [ ���� ����� ������ ã�Ƽ� �� ������ ��İ��� new_dest�� ���� ]
	double check_close = 61.0;
	int move_i = 12, move_j = 55;
	for (int i = 1; i < MAP_HEIGHT - 1; i++) {
		for (int j = 1; j < MAP_WIDTH - 1; j++) {
			if (map[1][i][j] == 'H'  || map[1][i][j] == 'S' || (map[1][i][j] == 'F' && 1 /* 1�� ���߿� ���� ������ ���� �� �ؾߵ� */)) {
				if (sqrt(pow(i - sw2_obj.pos.row, 2) + pow(j - sw2_obj.pos.column, 2)) < check_close) {
					check_close = sqrt(pow(i - sw2_obj.pos.row, 2) + pow(j - sw2_obj.pos.column, 2));
					move_i = i, move_j = j;
				}
			}
		}
	}
	POSITION new_dest = { move_i, move_j };
	sw2_obj.dest = new_dest;

	// [ ������, ������ �Ÿ��� ���ؼ� �� �� �� ������ �̵� ]
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

// [ ����� (2) ������ �Լ� ]
void sw2_move(void) {
	// [ �����̴� �ֱ� ]
	if (sys_clock <= sw2_obj.next_move_time) {
		return;
	}

	// [ �����(2) layer1 (map[1])�� ���� ]
	map[1][sw2_obj.pos.row][sw2_obj.pos.column] = -1;

	// [ 10/300 Ȯ���� �輳 --> �����̽� ������ ���� (���差 1 ~ 9 ����) ]
	int r = rand() % 299;
	if (r < 9) {
		// [ �ý��� �޽��� �߰� ]
		send_system_message[0] = "������� �����̽� �������� �����߽��ϴ�.";
		p_system_message(send_system_message[0]);

		OBJECT_BUILDING new_spice = {
			.pos1 = {sw2_obj.pos.row, sw2_obj.pos.column},
			.repr = spice_number[r],
			.layer = 0
		};

		SPICE[spi_count] = new_spice;
		map[0][sw2_obj.pos.row][sw2_obj.pos.column] = SPICE[spi_count].repr;
		spi_count++;
	}
	sw2_obj.pos = sw2_next_pos();

	// [ ����� ��ƸԱ� ]
	// [ �Ʊ� �Ϻ����Ϳ� ������ �� ]
	for (int i = 0; i < hav_count; i++) {
		if (map[1][sw2_obj.pos.row][sw2_obj.pos.column] == 'H' && check_friend[sw2_obj.pos.row][sw2_obj.pos.column] == 1) {
			// [ �ý��� �޽��� �߰� ]
			send_system_message[0] = "������� �Ʊ� �Ϻ����͸� ��ƸԾ����ϴ�.";
			p_system_message(send_system_message[0]);

			// [ �Ϻ����� �ʿ��� ������� ]
			havs[i]->repr = ' ';
			map[1][sw2_obj.pos.row][sw2_obj.pos.column] = ' ';
			resource.population -= 5;

			// [ �ӽ÷� ����� ��ġ �ٲٱ� ]
			POSITION new_dest = { 12, 55 };
			sw2_obj.dest = new_dest;

		}
	}

	// [ ���� �Ϻ����Ϳ� ������ �� ]
	for (int i = 0; i < e_hav_count; i++) {
		if (map[1][sw2_obj.pos.row][sw2_obj.pos.column] == 'H' && check_friend[sw2_obj.pos.row][sw2_obj.pos.column] == 2) {
			// [ �ý��� �޽��� �߰� ]
			send_system_message[0] = "������� ���� �Ϻ����͸� ��ƸԾ����ϴ�.";
			p_system_message(send_system_message[0]);

			// [ �Ϻ����� �ʿ��� ������� ]
			e_havs[i]->repr = ' ';
			map[1][sw2_obj.pos.row][sw2_obj.pos.column] = ' ';

			// [ �ӽ÷� ����� ��ġ �ٲٱ� ]
			POSITION new_dest = { 12, 55 };
			sw2_obj.dest = new_dest;
		}
	}


	// [ ����� ������ �� ]
	for (int i = 0; i < fighter_count; i++) {
		if (map[1][sw2_obj.pos.row][sw2_obj.pos.column] == 'F' && \
			frem_fight_fact_check[sw2_obj.pos.row][sw2_obj.pos.column] == 3) {
			// [ �ý��� �޽��� �߰� ]
			send_system_message[0] = "������� ���縦 ��ƸԾ����ϴ�.";
			p_system_message(send_system_message[0]);

			// [ ���� �ʿ��� ������� ]
			fighter[i]->repr = ' ';
			map[1][sw2_obj.pos.row][sw2_obj.pos.column] = ' ';

			// [ �ӽ÷� ����� ��ġ �ٲٱ� ]
			POSITION new_dest = { 12, 55 };
			sw2_obj.dest = new_dest;
		}
	}


	// [ ���� ������ �� ]
	for (int i = 0; i < sold_count; i++) {
		if (map[1][sw2_obj.pos.row][sw2_obj.pos.column] == 'S' && \
			shle_sold_check[sw2_obj.pos.row][sw2_obj.pos.column] == 2) {
			// [ �ý��� �޽��� �߰� ]
			send_system_message[0] = "������� ������ ��ƸԾ����ϴ�.";
			p_system_message(send_system_message[0]);

			// [ ���� �ʿ��� ������� ]
			sold[i]->repr = ' ';
			map[1][sw2_obj.pos.row][sw2_obj.pos.column] = ' ';
			resource.population -= 1;

			// [ �ӽ÷� ����� ��ġ �ٲٱ� ]
			POSITION new_dest = { 12, 55 };
			sw2_obj.dest = new_dest;
		}
	}

	map[1][sw2_obj.pos.row][sw2_obj.pos.column] = sw2_obj.repr;
	sw2_obj.next_move_time = sys_clock + sw2_obj.move_period;
}


// [ �� ��ǳ (X) ]
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
		p_system_message(send_system_message[0]);

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
		p_system_message(send_system_message[0]);

	}
	// < ������ > (���� ���� X)
	else if (map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'F' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'F' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'F' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'F') {



		// �ý��� �޽���
		send_system_message[0] = "�� ��ǳ�� ���� �������� ����߽��ϴ�.";
		p_system_message(send_system_message[0]);

	}
	// < ���� > (���� ���� X)
	else if (map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'S' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'S' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'S' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'S') {



		// �ý��� �޽���
		send_system_message[0] = "�� ��ǳ�� ���� ������ ����߽��ϴ�.";
		p_system_message(send_system_message[0]);

	}
	// < ���� > (���� ���� X)
	else if (map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'F' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'F' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'F' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'F') {


		// �ý��� �޽���
		send_system_message[0] = "�� ��ǳ�� ���� ���簡 ����߽��ϴ�.";
		p_system_message(send_system_message[0]);

	}
	// < ������ > (���� ���� X)
	else if (map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'T' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'T' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'T' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'T') {

		// �ý��� �޽���
		send_system_message[0] = "�� ��ǳ�� ���� �������� �μ������ϴ�.";
		p_system_message(send_system_message[0]);

	}
	// < ���� > (���� ���� X)
	else if (map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'D' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'D' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'D' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'D') {


		// �ý��� �޽���
		send_system_message[0] = "�� ��ǳ�� ���� ���Ұ� �ڻ쳵���ϴ�.";
		p_system_message(send_system_message[0]);

	}
	// < â�� > (���� ���� X)
	else if (map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'G' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'G' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'G' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'G') {


		// �ý��� �޽���
		send_system_message[0] = "�� ��ǳ�� ���� â�� �ڻ쳵���ϴ�.";
		p_system_message(send_system_message[0]);

	}
	// < ���� > (���� ���� X)
	else if (map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'B' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'B' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'B' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'B') {


		// �ý��� �޽���
		send_system_message[0] = "�� ��ǳ�� ���� ������ �ڻ쳵���ϴ�.";
		p_system_message(send_system_message[0]);

	}
	// < ����ó > (���� ���� X)
	else if (map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'S' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'S' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'S' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'S') {


		// �ý��� �޽���
		send_system_message[0] = "�� ��ǳ�� ���� ����ó�� �ڻ쳵���ϴ�.";
		p_system_message(send_system_message[0]);

	}
	// < ������ > (���� ���� X)
	else if (map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'A' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'A' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'A' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'A') {


		// �ý��� �޽���
		send_system_message[0] = "�� ��ǳ�� ���� �������� �ڻ쳵���ϴ�.";
		p_system_message(send_system_message[0]);

	}
	// < ���� > (���� ���� X)
	else if (map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'F' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'F' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'F' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'F') {
		


		// �ý��� �޽���
		send_system_message[0] = "�� ��ǳ�� ���� ������ �ڻ쳵���ϴ�.";
		p_system_message(send_system_message[0]);

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
		p_system_message(send_system_message[0]);
	}

	sand_wind.next_move_time = sys_clock + sand_wind.move_period;
}


// [ �Ϻ����� ���� �� �����̽� ���� ]
void handle_harvester_input(CURSOR cursor, OBJECT_SAMPLE havs[MAX_HAV], int hav_count, OBJECT_BUILDING* SPICE) {
	POSITION curr = cursor.current;

	// [ �����̽��ٷ� �Ϻ����� ���� ]
	if (space_key_press && map[1][curr.row][curr.column] == 'H') {
		for (int i = 0; i < hav_count; i++) {
			if (havs[i].pos.row == curr.row && havs[i].pos.column == curr.column) {
				selected_harvester = i; // �Ϻ����� ����
				p_system_message("�Ϻ����� ���� �Ϸ�.");
				space_key_press = false;
				space_and_h_key = true;
				space_and_m_key = true;
				break;
			}
		}
	}

	// [ HŰ�� �����̽� ���� �� ������ ���� ]
	if (space_and_h_key && h_key_press && selected_harvester != -1) {
		if (map[0][curr.row][curr.column] >= '1' && map[0][curr.row][curr.column] <= '9') {
			havs[selected_harvester].dest = harvest_set_dest(curr, &havs[selected_harvester], SPICE);
			p_system_message("�����̽� ��ġ�� �������� ����.");
			selected_harvester = -1;
			h_key_press = false; // ó�� �Ϸ�
			space_and_h_key = false;
			space_and_m_key = false;
		}
		else {
			h_key_press = false;
			p_system_message("�����̽��� ���õ��� �ʾҽ��ϴ�. �ٽ� hŰ�� �����ּ���");
		}
	}

	// [ MŰ�� �̵� ������ ���� ]
	if (space_and_m_key && m_key_press && selected_harvester != -1) {
		// �� ����(�縷)���θ� �̵� ����
		if (map[0][curr.row][curr.column] == ' ') {
			havs[selected_harvester].dest.row = curr.row;
			havs[selected_harvester].dest.column = curr.column;
			hav_gather_spice[selected_harvester] = -1; // �̵� ���� ǥ��
			p_system_message("Ŀ�� ��ġ�� �������� ����.");
			selected_harvester = -1;
			m_key_press = false;
			space_and_h_key = false;
			space_and_m_key = false;
		}
		else {
			m_key_press = false;
			p_system_message("�Ϻ����ʹ� �縷������������ �������� ������ �� �ֽ��ϴ�.");
		}
	}
}

// [ �Ϻ����� ������ ���ϱ� ]
POSITION harvest_set_dest(POSITION curr, OBJECT_SAMPLE* hav, OBJECT_BUILDING* SPICE) {
	POSITION new_dest = hav->dest;

	// [ Ŀ���� �����̽� ���� ���� �� ]
	for (int i = 0; i < spi_count; i++) {
		if (curr.row == SPICE[i].pos1.row && curr.column == SPICE[i].pos1.column) {
			new_dest.row = SPICE[i].pos1.row;
			new_dest.column = SPICE[i].pos1.column;
			pre_hav_dest[i] = new_dest;
			break; 
		}
	}

	return new_dest;
}

// [ �Ϻ����� �̵� ]
POSITION harvest_next_move(OBJECT_SAMPLE* hav, POSITION new_dest) {
	POSITION diff = psub(new_dest, hav->pos);
	DIRECTION dir;

	// ������� ������ �� �� �� ������ �̵� ���� ����
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// �⺻ �̵�
	POSITION next_pos = pmove(hav->pos, dir);

	// �� �� ���� Ȯ��
	if (next_pos.row < 1 || next_pos.row >= MAP_HEIGHT - 1 ||
		next_pos.column < 1 || next_pos.column >= MAP_WIDTH - 1) {
		return hav->pos; // �̵��� �� ������ ���� ��ġ ����
	}

	// �̵��Ϸ��� ��ġ�� ��ֹ����� Ȯ��
	char target_tile = map[0][next_pos.row][next_pos.column];
	if (target_tile != ' ' && (target_tile < '1' || target_tile > '9')) {
		// ��ֹ��� ȸ���ϱ� ���� ��ü ��� ���
		double best_distance = DBL_MAX;
		POSITION best_pos = hav->pos;

		// �����¿�� ������ ��ü ��� Ž��
		DIRECTION alternate_dirs[] = { d_up, d_down, d_left, d_right };
		for (int i = 0; i < 4; i++) {
			POSITION alt_pos = pmove(hav->pos, alternate_dirs[i]);

			// ��ü ��ΰ� �� ���� �ִ��� Ȯ��
			if (alt_pos.row < 1 || alt_pos.row >= MAP_HEIGHT - 1 ||
				alt_pos.column < 1 || alt_pos.column >= MAP_WIDTH - 1) {
				continue; // �� ���̸� ��ŵ
			}

			// ��ü ��ΰ� ��ֹ��� �ƴ��� Ȯ��
			char alt_tile = map[0][alt_pos.row][alt_pos.column];
			if (alt_tile == ' ' || (alt_tile >= '1' && alt_tile <= '9')) {
				double distance = sqrt(pow(new_dest.row - alt_pos.row, 2) + pow(new_dest.column - alt_pos.column, 2));
				if (distance < best_distance) {
					best_distance = distance;
					best_pos = alt_pos;
				}
			}
		}

		return best_pos; // ���� ����� ��ü ��� ��ȯ
	}

	return next_pos; // �⺻ �̵� �����ϸ� ��ȯ
}

// [ �����̽� ��Ȯ ó�� �Լ� ]
int harvest_gather_spice(OBJECT_BUILDING* spice, OBJECT_SAMPLE* hav, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	if (sys_clock <= hav->gather_time) {
		return 0; // ���� ��Ȯ �ð��� ���� �ʾ���
	}

	int spice_int = spice->repr - '0'; // �����̽� ���� ������ ��ȯ
	int harvest_amount = 3; // �Ϻ����Ͱ� �� ���� ��Ȯ�ϴ� ��

	// ��Ȯ ������ �縸ŭ ����
	int collected = (spice_int >= harvest_amount) ? harvest_amount : spice_int;
	spice_int -= collected; // �����̽� �� ����
	char mes[100] = { 0 };
	sprintf_s(mes, sizeof(mes), "�����̽� ���������� �ڿ��� %d�� ĺ���ϴ�.", collected);
	p_system_message(mes);

	// �����̽� ���� 0�̸� ����, �ƴϸ� ����
	if (spice_int <= 0) {
		spice->repr = ' '; // �����̽� ����
		map[0][spice->pos1.row][spice->pos1.column] = ' '; // �ʿ��� ����
		map[1][hav->pos.row][hav->pos.column] = hav->repr; // �� ��ġ ǥ��
		char* mes = "�����̽� �������� ������ϴ�.";
		p_system_message(mes);
	}
	else {
		spice->repr = spice_int + '0'; // ���� �����̽� ���
		map[0][spice->pos1.row][spice->pos1.column] = spice->repr;
		char mes_buf[100] = { 0 };
		sprintf_s(mes_buf, sizeof(mes_buf), "�����̽� �������� �����̽��� %c�� ���ҽ��ϴ�.", spice->repr);
		p_system_message(mes_buf);
	}

	hav->gather_time = sys_clock + hav->gather_next_time; // ���� ��Ȯ �ð� ����
	return collected; // ��Ȯ�� �ڿ� ��ȯ
}

// [ �Ϻ����� ������ ���� �Լ� ]
void harvest_move_all(OBJECT_SAMPLE havs[MAX_HAV], int hav_count, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], OBJECT_BUILDING* SPICE) {
	// [ ��� �Ϻ������� ���� ��ġ ��� ]
	handle_harvester_input(cursor, havs, hav_count, SPICE);

	// [ �Ϻ����� �̵� �ֱ� ]
	for (int i = 0; i < hav_count; i++) {
		if (sys_clock <= havs[i].move_period) {
				return; 
		}
	}
	
	// [ �Ϻ����� �̵� ]
	for (int i = 0; i < hav_count; i++) {
		// [ �Ϻ����� �������� ������ ��� ]
		if (havs[i].pos.row == havs[i].dest.row && havs[i].pos.column == havs[i].dest.column) {
			if (hav_gather_spice[i] == 0) { // �����̽� ��Ȯ
				for (int j = 0; j < spi_count; j++) {
					if (SPICE[j].pos1.row == havs[i].pos.row && SPICE[j].pos1.column == havs[i].pos.column) {
						// �ٸ� �Ϻ����Ͱ� �۾� ������ Ȯ��
						if (spice_in_progress[j] == -1) {
							spice_in_progress[j] = i; // ���� �Ϻ����� �۾� ����
							int spice = harvest_gather_spice(&SPICE[j], &havs[i], map);
							*spice_plus = spice;
							hav_gather_spice[i] = 1; // ���� ����
							havs[i].dest = (POSITION){ 14, 1 }; // �������� �̵�
						}
						else {
							// �۾� ���� �Ϻ����Ͱ� ������ ��� �޽��� ���
							char mes[100] = { 0 };
							sprintf_s(mes, sizeof(mes), "�Ϻ����� %d�� �۾� ���Դϴ�. ��� ��...", spice_in_progress[j]);
							p_system_message(mes);
						}
						break;
					}
				}
			}
			else if (hav_gather_spice[i] == 1) { // �������� �ڿ� ����
				resource.spice += *spice_plus;
				char mes[100] = { 0 };
				sprintf_s(mes, sizeof(mes), "�����̽� �ڿ��� %d�� �����Ͽ����ϴ�.", resource.spice);
				p_system_message(mes);

				// �۾� ���� �ʱ�ȭ
				for (int j = 0; j < spi_count; j++) {
					if (SPICE[j].pos1.row == pre_hav_dest[i].row && SPICE[j].pos1.column == pre_hav_dest[i].column) {
						spice_in_progress[j] = -1; // �۾� ����
						break;
					}
				}

				// ���� �����̽� ��ġ�� ����
				if (map[0][pre_hav_dest[i].row][pre_hav_dest[i].column] != ' ') {
					havs[i].dest = pre_hav_dest[i];
				}

				hav_gather_spice[i] = 0; // ���� �ʱ�ȭ
			}
			else if (hav_gather_spice[i] == -1) { // �ܼ� �̵� �Ϸ�
				hav_gather_spice[i] = 0; // ���� �ʱ�ȭ
				p_system_message("�Ϻ����� �̵� �Ϸ�.");
			}
			continue;
		}

		// [ �Ϻ����� �̵� ]
		if (map[0][havs[i].pos.row][havs[i].pos.column] >= '1' && map[0][havs[i].pos.row][havs[i].pos.column] <= '9') {
			// �����̽� ��ġ�� �״�� ����
		}
		else {
			map[1][havs[i].pos.row][havs[i].pos.column] = ' '; // ���� ��ġ ����
		}

		check_friend[havs[i].pos.row][havs[i].pos.column] = 0;

		// [ ��ֹ� ���ϱ� ���� �߰� ]
		POSITION next_pos = harvest_next_move(&havs[i], havs[i].dest);

		// ��ֹ��� �ִ� ��� ���ϱ�
		if (map[0][next_pos.row][next_pos.column] != ' ' &&
			(map[0][next_pos.row][next_pos.column] < '1' || map[0][next_pos.row][next_pos.column] > '9')) {

			// ��/�Ʒ��� ��ȸ���� ��/��� ��ȸ���� ���
			double move_up = sqrt(pow((havs[i].pos.row - 1) - havs[i].dest.row, 2) +
				pow(havs[i].pos.column - havs[i].dest.column, 2));
			double move_down = sqrt(pow((havs[i].pos.row + 1) - havs[i].dest.row, 2) +
				pow(havs[i].pos.column - havs[i].dest.column, 2));
			double move_left = sqrt(pow(havs[i].pos.row - havs[i].dest.row, 2) +
				pow((havs[i].pos.column - 1) - havs[i].dest.column, 2));
			double move_right = sqrt(pow(havs[i].pos.row - havs[i].dest.row, 2) +
				pow((havs[i].pos.column + 1) - havs[i].dest.column, 2));

			if (move_up < move_down && move_up < move_left && move_up < move_right) {
				if (havs[i].pos.row - 1 > 0) next_pos.row = havs[i].pos.row - 1;
			}
			else if (move_down < move_left && move_down < move_right) {
				if (havs[i].pos.row + 1 < MAP_HEIGHT - 1) next_pos.row = havs[i].pos.row + 1;
			}
			else if (move_left < move_right) {
				if (havs[i].pos.column - 1 > 0) next_pos.column = havs[i].pos.column - 1;
			}
			else {
				if (havs[i].pos.column + 1 < MAP_WIDTH - 1) next_pos.column = havs[i].pos.column + 1;
			}
		}

		// ���� �̵� ó��
		havs[i].pos = next_pos;
		check_friend[havs[i].pos.row][havs[i].pos.column] = 1;

		if (map[0][havs[i].pos.row][havs[i].pos.column] >= '1' && map[0][havs[i].pos.row][havs[i].pos.column] <= '9') {
			// �����̽� ��ġ�� �״�� ����
		}
		else {
			map[1][havs[i].pos.row][havs[i].pos.column] = havs[i].repr; // �� ��ġ ǥ��
		}
	}

	// [ �̵� �ֱ� ���� ]
	for (int i = 0; i < hav_count; i++) {
		havs[i].move_period = sys_clock + havs[i].next_move_time;
	} 
}



// [ ���� ���� --> �̵�, ����, �ǹ� ���� ]
void select_sold_act(void) {
	POSITION curr = cursor.current;

	// [ ���� ���� ]
	if (space_key_press && map[1][curr.row][curr.column] == 'S') {
		for (int i = 0; i < sold_count; i++) {
			if (sold[i]->pos.row == curr.row && sold[i]->pos.column == curr.column) {
				selected_sold = i;
				p_system_message("���� ���� �Ϸ�");
				space_key_press = false;
				space_and_m_key = true; // �̵�
				p_p_key_press = true; // ����
				break;
			}
		}
	}

	// [ ���� �̵�(M) ������ ���ϱ� ]
	if (space_and_m_key && m_key_press && selected_sold != -1) {

		// [ ���� ��Ȱ��ȭ ]
		sold[selected_sold]->is_patrolling = false;

		// [ �縷 ������ �̵� �������� ���ϱ� ]
		if (map[0][curr.row][curr.column] == ' ') {
			sold[selected_sold]->dest.row = curr.row;
			sold[selected_sold]->dest.column = curr.column;
			p_system_message("Ŀ�� ��ġ(�縷����)�� �������� ����");
			selected_sold = -1;
			m_key_press = false;
			p_p_key_press = false;
			space_and_m_key = false;
		}
		// [ ���� ���� �̵� �������� ���ϱ� ]
		else if (map[0][curr.row][curr.column] == 'B' && base_barr_check[curr.row][curr.column] == 1 && check_friend[curr.row][curr.column] == 2) {
			sold[selected_sold]->dest.row = e_base.pos3.row + 1;
			sold[selected_sold]->dest.column = e_base.pos3.column;
			sold[selected_sold]->e_base_attack = true;
			p_system_message("���� ���� ��ġ�� �������� ����.");
			selected_sold = -1;
			m_key_press = false;
			p_p_key_press = false;
			space_and_m_key = false;
		}
		// [ ���� ���� �̵� �������� ���ϱ� ]
		else if (map[0][curr.row][curr.column] == 'D' && check_friend[curr.row][curr.column] == 2) {
			for (int i = 0; i < e_dor_count; i++) {
				if ((E_DOR[i]->pos1.row == curr.row && E_DOR[i]->pos1.column == curr.column) || \
					(E_DOR[i]->pos2.row == curr.row && E_DOR[i]->pos2.column == curr.column) || \
					(E_DOR[i]->pos3.row == curr.row && E_DOR[i]->pos3.column == curr.column) || \
					(E_DOR[i]->pos4.row == curr.row && E_DOR[i]->pos4.column == curr.column)) {
					sold[selected_sold]->dest.row = E_DOR[i]->pos3.row + 1;
					sold[selected_sold]->dest.column = E_DOR[i]->pos3.column;
					sold[selected_sold]->e_dor_attack = true;
					p_system_message("���� ���� ��ġ�� �������� ����.");
					selected_sold = -1;
					m_key_press = false;
					p_p_key_press = false;
					space_and_m_key = false;
					break;
				}
			}
		}
		// [ ���� â�� �̵� �������� ���ϱ� ]
		else if (map[0][curr.row][curr.column] == 'G' && check_friend[curr.row][curr.column] == 2) {
			for (int i = 0; i < e_gar_count; i++) {
				if ((E_GAR[i]->pos1.row == curr.row && E_GAR[i]->pos1.column == curr.column) || \
					(E_GAR[i]->pos2.row == curr.row && E_GAR[i]->pos2.column == curr.column) || \
					(E_GAR[i]->pos3.row == curr.row && E_GAR[i]->pos3.column == curr.column) || \
					(E_GAR[i]->pos4.row == curr.row && E_GAR[i]->pos4.column == curr.column)) {
					sold[selected_sold]->dest.row = E_GAR[i]->pos3.row + 1;
					sold[selected_sold]->dest.column = E_GAR[i]->pos3.column;
					sold[selected_sold]->e_gar_attack = true;
					p_system_message("���� â�� ��ġ�� �������� ����.");
					selected_sold = -1;
					m_key_press = false;
					p_p_key_press = false;
					space_and_m_key = false;
					break;
				}
			}
		}
		// [ ���� ������ �̵� �������� ���ϱ� ]
		else if (map[0][curr.row][curr.column] == 'A' && check_friend[curr.row][curr.column] == 2) {
			for (int i = 0; i < are_count; i++) {
				if ((ARE[i]->pos1.row == curr.row && ARE[i]->pos1.column == curr.column) || \
					(ARE[i]->pos2.row == curr.row && ARE[i]->pos2.column == curr.column) || \
					(ARE[i]->pos3.row == curr.row && ARE[i]->pos3.column == curr.column) || \
					(ARE[i]->pos4.row == curr.row && ARE[i]->pos4.column == curr.column)) {
					sold[selected_sold]->dest.row = ARE[i]->pos3.row + 1;
					sold[selected_sold]->dest.column = ARE[i]->pos3.column;
					sold[selected_sold]->e_arena_attack = true;
					p_system_message("���� ������ ��ġ�� �������� ����.");
					selected_sold = -1;
					m_key_press = false;
					p_p_key_press = false;
					space_and_m_key = false;
					break;
				}
			}
		}

		// [ ���� ���� �̵� �������� ���ϱ� ]
		else if (map[0][curr.row][curr.column] == 'F' && check_friend[curr.row][curr.column] == 2 && \
			frem_fight_fact_check[curr.row][curr.column] == 1) {
			for (int i = 0; i < fac_count; i++) {
				if ((FAC[i]->pos1.row == curr.row && FAC[i]->pos1.column == curr.column) || \
					(FAC[i]->pos2.row == curr.row && FAC[i]->pos2.column == curr.column) || \
					(FAC[i]->pos3.row == curr.row && FAC[i]->pos3.column == curr.column) || \
					(FAC[i]->pos4.row == curr.row && FAC[i]->pos4.column == curr.column)) {
					sold[selected_sold]->dest.row = FAC[i]->pos3.row + 1;
					sold[selected_sold]->dest.column = FAC[i]->pos3.column;
					sold[selected_sold]->e_factory_attack = true;
					p_system_message("���� ���� ��ġ�� �������� ����.");
					selected_sold = -1;
					m_key_press = false;
					p_p_key_press = false;
					space_and_m_key = false;
					break;
				}
			}
		}
	}

	// [ ���� ����(P) ������ ���ϱ� ]
	if (p_p_key_press && sold_p_key_press && selected_sold != -1) {
		if (map[0][curr.row][curr.column] == ' ') {
			sold[selected_sold]->patrol_point.row = sold[selected_sold]->pos.row;
			sold[selected_sold]->patrol_point.column = sold[selected_sold]->pos.column;
			sold[selected_sold]->dest.row = curr.row;
			sold[selected_sold]->dest.column = curr.column;
			sold[selected_sold]->is_patrolling = true; // ���� ��� Ȱ��ȭ
			p_system_message("���� ��ġ ���� �Ϸ�. ���� ����.");
			selected_sold = -1;
			p_key_press = false;
			p_p_key_press = false;
			sold_p_key_press = false;
		}
		else {
			p_system_message("���� ��ġ�� ������ �� ���� �����Դϴ�.");
			p_p_key_press = false;
		}
	}
}

// [ ���� �ǹ� ���� ]
void sold_attack_building(void) {
	for (int i = 0; i < sold_count; i++) {
		// [ �������� �������� �� ]
		if (sold[i]->pos.row == sold[i]->dest.row && sold[i]->pos.column == sold[i]->dest.column) {
			// [ ���� ������ �����϶�� ����� �޾��� �� ]
			if (sold[i]->e_base_attack) {
				if (sys_clock <= sold[i]->attack_period) {
					return;
				}

				if (e_base.hp <= 0) {
					p_system_message("���� ������ ������ ���� �ı��Ǿ����ϴ�.");
					e_base.hp = 0;
					e_base.repr = ' ';
					map[0][e_base.pos1.row][e_base.pos1.column] = ' ';
					map[0][e_base.pos2.row][e_base.pos2.column] = ' ';
					map[0][e_base.pos3.row][e_base.pos3.column] = ' ';
					map[0][e_base.pos4.row][e_base.pos4.column] = ' ';
					sold[i]->e_base_attack = false;

					// [ ���� ���� ]
					outro();
				}
				else {
					e_base.hp -= sold[i]->dps;
					p_system_message("������ ������ �����߽��ϴ�.");
					char mes[100];
					sprintf_s(mes, sizeof(mes), "���� ������ �������� %d�� �����߽��ϴ�.", e_base.hp);
					p_system_message(mes);
				}
				sold[i]->attack_period = sys_clock + sold[i]->next_attack_time;
			}
			// [ ���� ���Ҹ� �����϶�� ����� �޾��� �� ]
			else if (sold[i]->e_dor_attack) {
				if (sys_clock <= sold[i]->attack_period) {
					return;
				}
				
				for (int j = 0; j < e_dor_count; j++) {
					if ((sold[i]->pos.row - 1 == E_DOR[j]->pos1.row && sold[i]->pos.column == E_DOR[j]->pos1.column) || \
						(sold[i]->pos.row - 1 == E_DOR[j]->pos2.row && sold[i]->pos.column == E_DOR[j]->pos2.column) || \
						(sold[i]->pos.row - 1 == E_DOR[j]->pos3.row && sold[i]->pos.column == E_DOR[j]->pos3.column) || \
						(sold[i]->pos.row - 1 == E_DOR[j]->pos4.row && sold[i]->pos.column == E_DOR[j]->pos4.column)) {
						if (E_DOR[j]->hp <= 0) {
							p_system_message("���� ���Ұ� ������ ���� �ı��Ǿ����ϴ�.");
							E_DOR[j]->hp = 0;
							E_DOR[j]->repr = ' ';
							map[0][E_DOR[j]->pos1.row][E_DOR[j]->pos1.column] = ' ';
							map[0][E_DOR[j]->pos2.row][E_DOR[j]->pos2.column] = ' ';
							map[0][E_DOR[j]->pos3.row][E_DOR[j]->pos3.column] = ' ';
							map[0][E_DOR[j]->pos4.row][E_DOR[j]->pos4.column] = ' ';
							sold[i]->e_dor_attack = false;
						}
						else {
							E_DOR[j]->hp -= sold[i]->dps;
							p_system_message("������ ���Ҹ� �����߽��ϴ�.");
							char mes[100];
							sprintf_s(mes, sizeof(mes), "���� ������ �������� %d�� �����߽��ϴ�.", E_DOR[j]->hp);
							p_system_message(mes);
						}
					}
				}
				sold[i]->attack_period = sys_clock + sold[i]->next_attack_time;
			}
			// [ ���� â�� �����϶�� ����� �޾��� �� ]
			else if (sold[i]->e_gar_attack) {
				if (sys_clock <= sold[i]->attack_period) {
					return;
				}

				for (int j = 0; j < e_gar_count; j++) {
					if ((sold[i]->pos.row - 1 == E_GAR[j]->pos1.row && sold[i]->pos.column == E_GAR[j]->pos1.column) || \
						(sold[i]->pos.row - 1 == E_GAR[j]->pos2.row && sold[i]->pos.column == E_GAR[j]->pos2.column) || \
						(sold[i]->pos.row - 1 == E_GAR[j]->pos3.row && sold[i]->pos.column == E_GAR[j]->pos3.column) || \
						(sold[i]->pos.row - 1 == E_GAR[j]->pos4.row && sold[i]->pos.column == E_GAR[j]->pos4.column)) {
						if (E_GAR[j]->hp <= 0) {
							p_system_message("���� â�� ������ ���� �ı��Ǿ����ϴ�.");
							E_GAR[j]->hp = 0;
							E_GAR[j]->repr = ' ';
							map[0][E_GAR[j]->pos1.row][E_GAR[j]->pos1.column] = ' ';
							map[0][E_GAR[j]->pos2.row][E_GAR[j]->pos2.column] = ' ';
							map[0][E_GAR[j]->pos3.row][E_GAR[j]->pos3.column] = ' ';
							map[0][E_GAR[j]->pos4.row][E_GAR[j]->pos4.column] = ' ';
							sold[i]->e_gar_attack = false;
						}
						else {
							E_GAR[j]->hp -= sold[i]->dps;
							p_system_message("������ â�� �����߽��ϴ�.");
							char mes[100];
							sprintf_s(mes, sizeof(mes), "���� â���� �������� %d�� �����߽��ϴ�.", E_GAR[j]->hp);
							p_system_message(mes);
						}
					}
				}
				sold[i]->attack_period = sys_clock + sold[i]->next_attack_time;
			}
			// [ ���� �������� �����϶�� ����� �޾��� �� ]
			else if (sold[i]->e_arena_attack) {
				if (sys_clock <= sold[i]->attack_period) {
					return;
				}


				for (int j = 0; j < are_count; j++) {
					if ((sold[i]->pos.row - 1 == ARE[j]->pos1.row && sold[i]->pos.column == ARE[j]->pos1.column) || \
						(sold[i]->pos.row - 1 == ARE[j]->pos2.row && sold[i]->pos.column == ARE[j]->pos2.column) || \
						(sold[i]->pos.row - 1 == ARE[j]->pos3.row && sold[i]->pos.column == ARE[j]->pos3.column) || \
						(sold[i]->pos.row - 1 == ARE[j]->pos4.row && sold[i]->pos.column == ARE[j]->pos4.column)) {
						if (ARE[j]->hp <= 0) {
							p_system_message("���� ����ó�� ������ ���� �ı��Ǿ����ϴ�.");
							ARE[j]->hp = 0;
							ARE[j]->repr = ' ';
							map[0][ARE[j]->pos1.row][ARE[j]->pos1.column] = ' ';
							map[0][ARE[j]->pos2.row][ARE[j]->pos2.column] = ' ';
							map[0][ARE[j]->pos3.row][ARE[j]->pos3.column] = ' ';
							map[0][ARE[j]->pos4.row][ARE[j]->pos4.column] = ' ';
							sold[i]->e_arena_attack = false;
						}
						else {
							ARE[j]->hp -= sold[i]->dps;
							p_system_message("������ ����ó�� �����߽��ϴ�.");
							char mes[100];
							sprintf_s(mes, sizeof(mes), "���� ����ó�� �������� %d�� �����߽��ϴ�.", ARE[j]->hp);
							p_system_message(mes);
						}
					}
				}
				sold[i]->attack_period = sys_clock + sold[i]->next_attack_time;
			}
			// [ ���� ������ �����϶�� ����� �޾��� �� ]
			else if (sold[i]->e_factory_attack) {
				if (sys_clock <= sold[i]->attack_period) {
					return;
				}

				for (int j = 0; j < fac_count; j++) {
					if ((sold[i]->pos.row - 1 == FAC[j]->pos1.row && sold[i]->pos.column == FAC[j]->pos1.column) || \
						(sold[i]->pos.row - 1 == FAC[j]->pos2.row && sold[i]->pos.column == FAC[j]->pos2.column) || \
						(sold[i]->pos.row - 1 == FAC[j]->pos3.row && sold[i]->pos.column == FAC[j]->pos3.column) || \
						(sold[i]->pos.row - 1 == FAC[j]->pos4.row && sold[i]->pos.column == FAC[j]->pos4.column)) {
						if (FAC[j]->hp <= 0) {
							p_system_message("���� ������ ������ ���� �ı��Ǿ����ϴ�.");
							FAC[j]->hp = 0;
							FAC[j]->repr = ' ';
							map[0][FAC[j]->pos1.row][FAC[j]->pos1.column] = ' ';
							map[0][FAC[j]->pos2.row][FAC[j]->pos2.column] = ' ';
							map[0][FAC[j]->pos3.row][FAC[j]->pos3.column] = ' ';
							map[0][FAC[j]->pos4.row][FAC[j]->pos4.column] = ' ';
							sold[i]->e_factory_attack = false;
						}
						else {
							FAC[j]->hp -= sold[i]->dps;
							p_system_message("������ ������ �����߽��ϴ�.");
							char mes[100];
							sprintf_s(mes, sizeof(mes), "���� ������ �������� %d�� �����߽��ϴ�.", FAC[j]->hp);
							p_system_message(mes);
						}
					}
				}
				sold[i]->attack_period = sys_clock + sold[i]->next_attack_time;
			}
		}
	}
}

// [ ���� ���� ���� ]
void sold_attack_unit(void) {
	for (int i = 0; i < sold_count; i++) {
		// [ ���� �þ� ���� ]
		int sight = sold[i]->eyes;

		// [ ���� ���� ��ġ ]
		int current_row = sold[i]->pos.row;
		int current_col = sold[i]->pos.column;

		// [ ���� �ֺ��� ���� �ִ��� Ȯ�� ]
		for (int row = current_row - sight; row <= current_row + sight; row++) {
			for (int col = current_col - sight; col <= current_col + sight; col++) {
				// [ �� ��� Ȯ�� ]
				if (row < 0 || row >= MAP_HEIGHT || col < 0 || col >= MAP_WIDTH) continue;

				// [ ���� �ڽ� ���� ]
				if (row == current_row && col == current_col) continue;

				// [ ���� ó�� �Լ� ȣ�� ]
				process_enemy_unit(row, col, sold[i]);
			}
		}
	}
}

// Main - [ �� ���� ó�� �Լ� ]
void process_enemy_unit(int row, int col, OBJECT_SAMPLE* soldier) {
	// [ ���� �Ϻ����� Ȯ�� ]
	for (int j = 0; j < e_hav_count; j++) {
		if (row == e_havs[j]->pos.row && col == e_havs[j]->pos.column && check_friend[row][col] == 2 && \
			e_havs[j]->repr == 'H') {
			p_system_message("������ ���� �Ϻ����͸� �߰��߽��ϴ�.");
			p_system_message("������ ���� �Ϻ����͸� ��ǥ�� ��ҽ��ϴ�.");
			handle_enemy_harvester(soldier, e_havs[j]);
			return; // �Ϻ����� ó�� �� ����
		}
	}

	// [ ���� ���� Ȯ�� ]
	for (int j = 0; j < fighter_count; j++) {
		if (row == fighter[j]->pos.row && col == fighter[j]->pos.column && frem_fight_fact_check[row][col] == 3 && \
			fighter[j]->repr == 'F') {
			p_system_message("������ ���� ���縦 �߰��߽��ϴ�.");
			p_system_message("������ ���� ���縦 ��ǥ�� ��ҽ��ϴ�.");
			handle_enemy_fighter(soldier, fighter[j]);
			return; // ���� ó�� �� ����
		}
	}

	// [ ���� ������ Ȯ�� ]
	for (int j = 0; j < tank_count; j++) {
		if (row == tank[j]->pos.row && col == tank[j]->pos.column && tank[j]->repr == 'T') {
			p_system_message("������ ���� ������ �߰��߽��ϴ�.");
			p_system_message("������ ���� ���縦 ��ǥ�� ��ҽ��ϴ�.");
			handle_enemy_tank(soldier, tank[j]);
			return; // ������ ó�� �� ����
		}
	}
}

// 1 - [ �� �Ϻ����� ó�� �Լ� ]
void handle_enemy_harvester(OBJECT_SAMPLE* soldier, OBJECT_SAMPLE* harvester) {

	// [ �� �ֺ��� �� ���� Ȯ�� �� �̵� ]
	int check_row[] = { -1, -1, 0, 1, 1, 1, 0, -1 };
	int check_col[] = { 0, -1, -1, -1, 0, 1, 1, 1 };

	for (int k = 0; k < 8; k++) {
		int dest_row = harvester->pos.row + check_row[k];
		int dest_col = harvester->pos.column + check_col[k];

		if (dest_row >= 0 && dest_row < MAP_HEIGHT && dest_col >= 0 && dest_col < MAP_WIDTH &&
			map[0][dest_row][dest_col] == ' ') {
			soldier->dest.row = dest_row;
			soldier->dest.column = dest_col;

			POSITION next_pos = soldier->pos;

			// �̵� ���� ����
			if (next_pos.row < soldier->dest.row) next_pos.row++;
			else if (next_pos.row > soldier->dest.row) next_pos.row--;

			if (next_pos.column < soldier->dest.column) next_pos.column++;
			else if (next_pos.column > soldier->dest.column) next_pos.column--;

			// ��ֹ� ó��
			next_pos = sold_move(soldier, next_pos);

			// �̵� ����
			soldier->pos = next_pos;

			// ������ ���� �� ����
			
			// [ ���� �ֱ� ]
			if (sys_clock <= soldier->attack_period) {
				return;
			}

			if (soldier->pos.row == soldier->dest.row && soldier->pos.column == soldier->dest.column) {
				if (harvester->hp <= 0) {
					p_system_message("���� �Ϻ������� ������ ���� ����Ͽ����ϴ�.");
					harvester->repr = ' ';
					map[1][harvester->pos.row][harvester->pos.column] = harvester->repr;
				}
				else {
					harvester->hp -= soldier->dps;
					char mes[100];
					sprintf_s(mes, sizeof(mes), "���� �Ϻ����Ͱ� �������� ���ݹ޾� hp�� %d�� �����߽��ϴ�.", harvester->hp);
					p_system_message(mes);
				}
			}

			soldier->attack_period = sys_clock + soldier->next_attack_time;
			break;
		}
	}
}

// 2 - [ �� ���� ó�� �Լ� ]
void handle_enemy_fighter(OBJECT_SAMPLE* soldier, OBJECT_SAMPLE* fighter) {

	// [ �� �ֺ��� �� ���� Ȯ�� �� �̵� ]
	int check_row[] = { -1, -1, 0, 1, 1, 1, 0, -1 };
	int check_col[] = { 0, -1, -1, -1, 0, 1, 1, 1 };

	for (int k = 0; k < 8; k++) {
		int dest_row = fighter->pos.row + check_row[k];
		int dest_col = fighter->pos.column + check_col[k];

		if (dest_row >= 0 && dest_row < MAP_HEIGHT && dest_col >= 0 && dest_col < MAP_WIDTH &&
			map[0][dest_row][dest_col] == ' ') {
			soldier->dest.row = dest_row;
			soldier->dest.column = dest_col;

			POSITION next_pos = soldier->pos;

			// �̵� ���� ����
			if (next_pos.row < soldier->dest.row) next_pos.row++;
			else if (next_pos.row > soldier->dest.row) next_pos.row--;

			if (next_pos.column < soldier->dest.column) next_pos.column++;
			else if (next_pos.column > soldier->dest.column) next_pos.column--;

			// ��ֹ� ó��
			next_pos = sold_move(soldier, next_pos);

			// �̵� ����
			soldier->pos = next_pos;

			// [ ���� �ֱ� ]
			if (sys_clock <= soldier->attack_period) {
				return;
			}

			// ������ ���� �� ����
			if (soldier->pos.row == soldier->dest.row && soldier->pos.column == soldier->dest.column) {
				if (fighter->hp <= 0) {
					p_system_message("���� ���簡 ������ ���� ����Ͽ����ϴ�.");
					fighter->repr = ' ';
					map[1][fighter->pos.row][fighter->pos.column] = fighter->repr;
				}
				else {
					fighter->hp -= soldier->dps;
					char mes[100];
					sprintf_s(mes, sizeof(mes), "���� ���簡 �������� ���ݹ޾� hp�� %d�� �����߽��ϴ�.", fighter->hp);
					p_system_message(mes);
				}
			}
			soldier->attack_period = sys_clock + soldier->next_attack_time;
			break;
		}
	}
}

// 3 - [ �� ������ ó�� �Լ� ]
void handle_enemy_tank(OBJECT_SAMPLE* soldier, OBJECT_SAMPLE* tank) {

	// [ �� �ֺ��� �� ���� Ȯ�� �� �̵� ]
	int check_row[] = { -1, -1, 0, 1, 1, 1, 0, -1 };
	int check_col[] = { 0, -1, -1, -1, 0, 1, 1, 1 };

	for (int k = 0; k < 8; k++) {
		int dest_row = tank->pos.row + check_row[k];
		int dest_col = tank->pos.column + check_col[k];

		if (dest_row >= 0 && dest_row < MAP_HEIGHT && dest_col >= 0 && dest_col < MAP_WIDTH &&
			map[0][dest_row][dest_col] == ' ') {
			soldier->dest.row = dest_row;
			soldier->dest.column = dest_col;

			POSITION next_pos = soldier->pos;

			// �̵� ���� ����
			if (next_pos.row < soldier->dest.row) next_pos.row++;
			else if (next_pos.row > soldier->dest.row) next_pos.row--;

			if (next_pos.column < soldier->dest.column) next_pos.column++;
			else if (next_pos.column > soldier->dest.column) next_pos.column--;

			// ��ֹ� ó��
			next_pos = sold_move(soldier, next_pos);

			// �̵� ����
			soldier->pos = next_pos;

			// ������ ���� �� ����

			// [ ���� �ֱ� ]
			if (sys_clock <= soldier->attack_period) {
				return;
			}

			if (soldier->pos.row == soldier->dest.row && soldier->pos.column == soldier->dest.column) {
				if (tank->hp <= 0) {
					p_system_message("���� �������� ������ ���� �ı��Ǿ����ϴ�.");
					tank->repr = ' ';
					map[1][tank->pos.row][tank->pos.column] = tank->repr;
				}
				else {
					tank->hp -= soldier->dps;
					char mes[100];
					sprintf_s(mes, sizeof(mes), "���� �������� �������� ���ݹ޾� hp�� %d�� �����߽��ϴ�.", tank->hp);
					p_system_message(mes);
				}
			}
			soldier->attack_period = sys_clock + soldier->next_attack_time;
			break;
		}
	}
}

// [ ���� �̵� �Լ� ]
POSITION sold_move(OBJECT_SAMPLE* sold_unit, POSITION target_pos) {
	POSITION diff = psub(target_pos, sold_unit->pos);
	DIRECTION dir;

	// ������� ������ �� �� �� ������ �̵� ���� ����
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// �⺻ �̵� ��ġ ���
	POSITION next_pos = pmove(sold_unit->pos, dir);

	// �� �� ���� Ȯ��
	if (next_pos.row < 1 || next_pos.row >= MAP_HEIGHT - 1 ||
		next_pos.column < 1 || next_pos.column >= MAP_WIDTH - 1) {
		return sold_unit->pos; // �̵��� �� ������ ���� ��ġ ����
	}

	// ��ֹ� Ȯ�� �� ��ü ��� ���
	char target_tile = map[0][next_pos.row][next_pos.column]; // �ǹ�
	char target_tile2 = map[1][next_pos.row][next_pos.column]; // �����

	if (target_tile != ' ' || target_tile2 == 'W') {
		// ��ֹ��� ȸ���ϱ� ���� ��ü ��� ���
		double best_distance = DBL_MAX;
		POSITION best_pos = sold_unit->pos;

		// �����¿�� ������ ��ü ��� Ž��
		DIRECTION alternate_dirs[] = { d_up, d_down, d_left, d_right };
		for (int j = 0; j < 4; j++) {
			POSITION alt_pos = pmove(sold_unit->pos, alternate_dirs[j]);

			// ��ü ��ΰ� �� ���� �ִ��� Ȯ��
			if (alt_pos.row < 1 || alt_pos.row >= MAP_HEIGHT - 1 ||
				alt_pos.column < 1 || alt_pos.column >= MAP_WIDTH - 1) {
				continue; // �� ���̸� ��ŵ
			}

			// ��ü ��ΰ� ��ֹ��� �ƴ��� Ȯ��
			char alt_tile = map[0][alt_pos.row][alt_pos.column];
			if (alt_tile == ' ' && target_tile2 != 'W') {
				double distance = sqrt(pow(target_pos.row - alt_pos.row, 2) +
					pow(target_pos.column - alt_pos.column, 2));
				if (distance < best_distance) {
					best_distance = distance;
					best_pos = alt_pos;
				}
			}
		}
		return best_pos; // ���� ����� ��ü ��� ��ȯ
	}

	return next_pos; // �⺻ �̵� �����ϸ� ��ȯ
}

// [ ���� ���� �Լ� ]
void main_sold_func(void) {
	// [ ���� ���� ]
	select_sold_act();

	// [ �̵� �ֱ� ]
	for (int i = 0; i < sold_count; i++) {
		if (sys_clock <= sold[i]->move_period) {
			return;
		}
	}

	// [ ���� �̵� �� ���� ���� ]
	for (int i = 0; i < sold_count; i++) {
		// ��� �˻� �߰�
		if (i < 0 || i >= MAX_SOLD) {
			continue;  // ��ȿ���� ���� �ε��� �ǳʶٱ�
		}

		POSITION target_pos = { -1, -1 };

		// [ �̵� ]
		if ((sold[i]->dest.row != -1 && sold[i]->dest.column != -1) &&
			(sold[i]->pos.row != sold[i]->dest.row || sold[i]->pos.column != sold[i]->dest.column)) {
			target_pos = sold[i]->dest;
		}

		// [ ���� ]
		else if (sold[i]->is_patrolling) {
			// [ ���� ��ġ���� patrol_point�� �����ߴٸ� ]
			if (sold[i]->pos.row == sold[i]->patrol_point.row &&
				sold[i]->pos.column == sold[i]->patrol_point.column) {
				// [ ���� �������� ���ư��� ]
				sold[i]->dest = sold[i]->original_dest;
				target_pos = sold[i]->dest;
			}
			// [ dest ��ġ�� �����ߴٸ� ]
			else if (sold[i]->pos.row == sold[i]->dest.row &&
				sold[i]->pos.column == sold[i]->dest.column) {
				// [ patrol_point�� �̵� ]
				sold[i]->original_dest = sold[i]->dest;  // ���� ������ ����
				sold[i]->dest = sold[i]->patrol_point;
				target_pos = sold[i]->dest;
			}
			// [ ���� �������� �������� �ʾҴٸ� ���� dest�� �̵� ]
			else {
				target_pos = sold[i]->dest;
			}
		}

		// [ ������ �̵�  ]
		if (target_pos.row != -1 && target_pos.column != -1) {
			// [ ���� ��ġ ����� ]
			if (sold[i]->pos.row >= 0 && sold[i]->pos.row < MAP_HEIGHT &&
				sold[i]->pos.column >= 0 && sold[i]->pos.column < MAP_WIDTH &&
				map[1][sold[i]->pos.row][sold[i]->pos.column] == sold[i]->repr) {
				map[1][sold[i]->pos.row][sold[i]->pos.column] = ' ';
				shle_sold_check[sold[i]->pos.row][sold[i]->pos.column] = 0;
			}

			// [ ���� �̵� ]
			POSITION next_pos = sold_move(sold[i], target_pos);
			sold[i]->pos = next_pos;

			// [ �� ��ġ�� ���� ǥ�� ]
			if (sold[i]->pos.row >= 0 && sold[i]->pos.row < MAP_HEIGHT &&
				sold[i]->pos.column >= 0 && sold[i]->pos.column < MAP_WIDTH) {
				map[1][sold[i]->pos.row][sold[i]->pos.column] = sold[i]->repr;
				shle_sold_check[sold[i]->pos.row][sold[i]->pos.column] = 2;
			}
		}
	}

	// [ ���� �ǹ� ���� ]
	sold_attack_building();

	// [ ���� ���� ���� ]
	for (int i = 0; i < sold_count; i++) {
		if (sold[i]->e_arena_attack == false && sold[i]->e_dor_attack == false && sold[i]->e_gar_attack == false &&\
			sold[i]->e_base_attack == false && sold[i]->e_factory_attack == false) {
			sold_attack_unit();
		}
	}
	
	

	// [ �̵� �ֱ� ]
	for (int i = 0; i < sold_count; i++) {
		sold[i]->move_period = sys_clock + sold[i]->next_move_time;
	}
}




// [ ������ ���� --> �̵�, ����, �ǹ����� ]
void select_frem_act(void) {
	POSITION curr = cursor.current;

	// [ ������ ���� ]
	if (space_key_press && map[1][curr.row][curr.column] == 'F' && \
		frem_fight_fact_check[curr.row][curr.column] == 2) {
		for (int i = 0; i < frem_count; i++) {
			if (frem[i]->pos.row == curr.row && frem[i]->pos.column == curr.column) {
				selected_frem = i;
				p_system_message("������ ���� �Ϸ�");
				space_key_press = false;
				space_and_m_key = true; // �̵�
				p_p_key_press = true; // ����
				break;
			}
		}
	}

	// [ ������ �̵� (M) ������ ���ϱ� ]
	if (space_and_m_key && m_key_press && selected_frem != -1) {
		// [ ���� ��Ȱ��ȭ ]
		frem[selected_frem]->is_patrolling = false;

		// [ �縷 ������ �̵� �������� ���ϱ� ]
		if (map[0][curr.row][curr.column] == ' ') {
			frem[selected_frem]->dest.row = curr.row;
			frem[selected_frem]->dest.column = curr.column;
			p_system_message("Ŀ�� ��ġ(�縷����)�� �������� ����");
			selected_frem = -1;
			m_key_press = false;
			p_p_key_press = false;
			space_and_m_key = false;
		}
		// [ ���� ���� �̵� �������� ���ϱ� ]
		else if (map[0][curr.row][curr.column] == 'B' && base_barr_check[curr.row][curr.column] == 1 && check_friend[curr.row][curr.column] == 2) {
			frem[selected_frem]->dest.row = e_base.pos3.row + 1;
			frem[selected_frem]->dest.column = e_base.pos3.column;
			frem[selected_frem]->e_base_attack = true;
			p_system_message("���� ���� ��ġ�� �������� ����.");
			selected_frem = -1;
			m_key_press = false;
			p_p_key_press = false;
			space_and_m_key = false;
		}
		// [ ���� ���� �̵� �������� ���ϱ� ]
		else if (map[0][curr.row][curr.column] == 'D' && check_friend[curr.row][curr.column] == 2) {
			for (int i = 0; i < e_dor_count; i++) {
				if ((E_DOR[i]->pos1.row == curr.row && E_DOR[i]->pos1.column == curr.column) || \
					(E_DOR[i]->pos2.row == curr.row && E_DOR[i]->pos2.column == curr.column) || \
					(E_DOR[i]->pos3.row == curr.row && E_DOR[i]->pos3.column == curr.column) || \
					(E_DOR[i]->pos4.row == curr.row && E_DOR[i]->pos4.column == curr.column)) {
					frem[selected_frem]->dest.row = E_DOR[i]->pos3.row + 1;
					frem[selected_frem]->dest.column = E_DOR[i]->pos3.column;
					frem[selected_frem]->e_dor_attack = true;
					p_system_message("���� ���� ��ġ�� �������� ����.");
					selected_frem = -1;
					m_key_press = false;
					p_p_key_press = false;
					space_and_m_key = false;
					break;
				}
			}
		}
		// [ ���� â�� �̵� �������� ���ϱ� ]
		else if (map[0][curr.row][curr.column] == 'G' && check_friend[curr.row][curr.column] == 2) {
			for (int i = 0; i < e_gar_count; i++) {
				if ((E_GAR[i]->pos1.row == curr.row && E_GAR[i]->pos1.column == curr.column) || \
					(E_GAR[i]->pos2.row == curr.row && E_GAR[i]->pos2.column == curr.column) || \
					(E_GAR[i]->pos3.row == curr.row && E_GAR[i]->pos3.column == curr.column) || \
					(E_GAR[i]->pos4.row == curr.row && E_GAR[i]->pos4.column == curr.column)) {
					frem[selected_frem]->dest.row = E_GAR[i]->pos3.row + 1;
					frem[selected_frem]->dest.column = E_GAR[i]->pos3.column;
					frem[selected_frem]->e_gar_attack = true;
					p_system_message("���� â�� ��ġ�� �������� ����.");
					selected_frem = -1;
					m_key_press = false;
					p_p_key_press = false;
					space_and_m_key = false;
					break;
				}
			}
		}
		// [ ���� ������ �̵� �������� ���ϱ� ]
		else if (map[0][curr.row][curr.column] == 'A' && check_friend[curr.row][curr.column] == 2) {
			for (int i = 0; i < are_count; i++) {
				if ((ARE[i]->pos1.row == curr.row && ARE[i]->pos1.column == curr.column) || \
					(ARE[i]->pos2.row == curr.row && ARE[i]->pos2.column == curr.column) || \
					(ARE[i]->pos3.row == curr.row && ARE[i]->pos3.column == curr.column) || \
					(ARE[i]->pos4.row == curr.row && ARE[i]->pos4.column == curr.column)) {
					frem[selected_frem]->dest.row = ARE[i]->pos3.row + 1;
					frem[selected_frem]->dest.column = ARE[i]->pos3.column;
					frem[selected_frem]->e_arena_attack = true;
					p_system_message("���� ������ ��ġ�� �������� ����.");
					selected_frem = -1;
					m_key_press = false;
					p_p_key_press = false;
					space_and_m_key = false;
					break;
				}
			}
		}

		// [ ���� ���� �̵� �������� ���ϱ� ]
		else if (map[0][curr.row][curr.column] == 'F' && check_friend[curr.row][curr.column] == 2 && \
			frem_fight_fact_check[curr.row][curr.column] == 1) {
			for (int i = 0; i < fac_count; i++) {
				if ((FAC[i]->pos1.row == curr.row && FAC[i]->pos1.column == curr.column) || \
					(FAC[i]->pos2.row == curr.row && FAC[i]->pos2.column == curr.column) || \
					(FAC[i]->pos3.row == curr.row && FAC[i]->pos3.column == curr.column) || \
					(FAC[i]->pos4.row == curr.row && FAC[i]->pos4.column == curr.column)) {
					frem[selected_frem]->dest.row = FAC[i]->pos3.row + 1;
					frem[selected_frem]->dest.column = FAC[i]->pos3.column;
					frem[selected_frem]->e_factory_attack = true;
					p_system_message("���� ���� ��ġ�� �������� ����.");
					selected_frem = -1;
					m_key_press = false;
					p_p_key_press = false;
					space_and_m_key = false;
					break;
				}
			}
		}

		// [ ������ ����(P) ������ ���ϱ� ]
		if (p_p_key_press && frem_p_key_press && selected_frem != -1) {
			if (map[0][curr.row][curr.column] == ' ') {
				frem[selected_frem]->patrol_point.row = frem[selected_frem]->pos.row;
				frem[selected_frem]->patrol_point.column = frem[selected_frem]->pos.column;
				frem[selected_frem]->dest.row = curr.row;
				frem[selected_frem]->dest.column = curr.column;
				frem[selected_frem]->is_patrolling = true; // ���� ��� Ȱ��ȭ
				p_system_message("���� ��ġ ���� �Ϸ�. ���� ����.");
				selected_frem = -1;
				p_key_press = false;
				p_p_key_press = false;
				frem_p_key_press = false;
			}
			else {
				p_system_message("���� ��ġ�� ������ �� ���� �����Դϴ�.");
				p_p_key_press = false;
			}
		}
	}
}

// [ ������ �ǹ� ���� ]
void frem_attack_building(void) {
	for (int i = 0; i < frem_count; i++) {
		// [ �������� �������� �� ]
		if (frem[i]->pos.row == frem[i]->dest.row && frem[i]->pos.column == frem[i]->dest.column) {
			// [ ���� ������ �����϶�� ����� �޾��� �� ]
			if (frem[i]->e_base_attack) {
				if (sys_clock <= frem[i]->attack_period) {
					return;
				}

				if (e_base.hp <= 0) {
					p_system_message("���� ������ �����࿡ ���� �ı��Ǿ����ϴ�.");
					e_base.hp = 0;
					e_base.repr = ' ';
					map[0][e_base.pos1.row][e_base.pos1.column] = ' ';
					map[0][e_base.pos2.row][e_base.pos2.column] = ' ';
					map[0][e_base.pos3.row][e_base.pos3.column] = ' ';
					map[0][e_base.pos4.row][e_base.pos4.column] = ' ';
					frem[i]->e_base_attack = false;

					// [ ���� ���� ]
					outro();
				}
				else {
					e_base.hp -= frem[i]->dps;
					p_system_message("�������� ������ �����߽��ϴ�.");
					char mes[100];
					sprintf_s(mes, sizeof(mes), "���� ������ �������� %d�� �����߽��ϴ�.", e_base.hp);
					p_system_message(mes);
				}
				frem[i]->attack_period = sys_clock + frem[i]->next_attack_time;
			}
			// [ ���� ���Ҹ� �����϶�� ����� �޾��� �� ]
			else if (frem[i]->e_dor_attack) {
				if (sys_clock <= frem[i]->attack_period) {
					return;
				}

				for (int j = 0; j < e_dor_count; j++) {
					if ((frem[i]->pos.row - 1 == E_DOR[j]->pos1.row && frem[i]->pos.column == E_DOR[j]->pos1.column) || \
						(frem[i]->pos.row - 1 == E_DOR[j]->pos2.row && frem[i]->pos.column == E_DOR[j]->pos2.column) || \
						(frem[i]->pos.row - 1 == E_DOR[j]->pos3.row && frem[i]->pos.column == E_DOR[j]->pos3.column) || \
						(frem[i]->pos.row - 1 == E_DOR[j]->pos4.row && frem[i]->pos.column == E_DOR[j]->pos4.column)) {
						if (E_DOR[j]->hp <= 0) {
							p_system_message("���� ���Ұ� �����࿡ ���� �ı��Ǿ����ϴ�.");
							E_DOR[j]->hp = 0;
							E_DOR[j]->repr = ' ';
							map[0][E_DOR[j]->pos1.row][E_DOR[j]->pos1.column] = ' ';
							map[0][E_DOR[j]->pos2.row][E_DOR[j]->pos2.column] = ' ';
							map[0][E_DOR[j]->pos3.row][E_DOR[j]->pos3.column] = ' ';
							map[0][E_DOR[j]->pos4.row][E_DOR[j]->pos4.column] = ' ';
							frem[i]->e_dor_attack = false;
						}
						else {
							E_DOR[j]->hp -= frem[i]->dps;
							p_system_message("�������� ���Ҹ� �����߽��ϴ�.");
							char mes[100];
							sprintf_s(mes, sizeof(mes), "���� ������ �������� %d�� �����߽��ϴ�.", E_DOR[j]->hp);
							p_system_message(mes);
						}
					}
				}
				frem[i]->attack_period = sys_clock + frem[i]->next_attack_time;
			}
			// [ ���� â�� �����϶�� ����� �޾��� �� ]
			else if (frem[i]->e_gar_attack) {
				if (sys_clock <= frem[i]->attack_period) {
					return;
				}

				for (int j = 0; j < e_gar_count; j++) {
					if ((frem[i]->pos.row - 1 == E_GAR[j]->pos1.row && frem[i]->pos.column == E_GAR[j]->pos1.column) || \
						(frem[i]->pos.row - 1 == E_GAR[j]->pos2.row && frem[i]->pos.column == E_GAR[j]->pos2.column) || \
						(frem[i]->pos.row - 1 == E_GAR[j]->pos3.row && frem[i]->pos.column == E_GAR[j]->pos3.column) || \
						(frem[i]->pos.row - 1 == E_GAR[j]->pos4.row && frem[i]->pos.column == E_GAR[j]->pos4.column)) {
						if (E_GAR[j]->hp <= 0) {
							p_system_message("���� â�� �����࿡ ���� �ı��Ǿ����ϴ�.");
							E_GAR[j]->hp = 0;
							E_GAR[j]->repr = ' ';
							map[0][E_GAR[j]->pos1.row][E_GAR[j]->pos1.column] = ' ';
							map[0][E_GAR[j]->pos2.row][E_GAR[j]->pos2.column] = ' ';
							map[0][E_GAR[j]->pos3.row][E_GAR[j]->pos3.column] = ' ';
							map[0][E_GAR[j]->pos4.row][E_GAR[j]->pos4.column] = ' ';
							frem[i]->e_gar_attack = false;
						}
						else {
							E_GAR[j]->hp -= frem[i]->dps;
							p_system_message("�������� â�� �����߽��ϴ�.");
							char mes[100];
							sprintf_s(mes, sizeof(mes), "���� â���� �������� %d�� �����߽��ϴ�.", E_GAR[j]->hp);
							p_system_message(mes);
						}
					}
				}
				frem[i]->attack_period = sys_clock + frem[i]->next_attack_time;
			}
			// [ ���� �������� �����϶�� ����� �޾��� �� ]
			else if (frem[i]->e_arena_attack) {
				if (sys_clock <= frem[i]->attack_period) {
					return;
				}


				for (int j = 0; j < are_count; j++) {
					if ((frem[i]->pos.row - 1 == ARE[j]->pos1.row && frem[i]->pos.column == ARE[j]->pos1.column) || \
						(frem[i]->pos.row - 1 == ARE[j]->pos2.row && frem[i]->pos.column == ARE[j]->pos2.column) || \
						(frem[i]->pos.row - 1 == ARE[j]->pos3.row && frem[i]->pos.column == ARE[j]->pos3.column) || \
						(frem[i]->pos.row - 1 == ARE[j]->pos4.row && frem[i]->pos.column == ARE[j]->pos4.column)) {
						if (ARE[j]->hp <= 0) {
							p_system_message("���� ����ó�� �����࿡ ���� �ı��Ǿ����ϴ�.");
							ARE[j]->hp = 0;
							ARE[j]->repr = ' ';
							map[0][ARE[j]->pos1.row][ARE[j]->pos1.column] = ' ';
							map[0][ARE[j]->pos2.row][ARE[j]->pos2.column] = ' ';
							map[0][ARE[j]->pos3.row][ARE[j]->pos3.column] = ' ';
							map[0][ARE[j]->pos4.row][ARE[j]->pos4.column] = ' ';
							frem[i]->e_arena_attack = false;
						}
						else {
							ARE[j]->hp -= frem[i]->dps;
							p_system_message("�������� ����ó�� �����߽��ϴ�.");
							char mes[100];
							sprintf_s(mes, sizeof(mes), "���� ����ó�� �������� %d�� �����߽��ϴ�.", ARE[j]->hp);
							p_system_message(mes);
						}
					}
				}
				frem[i]->attack_period = sys_clock + frem[i]->next_attack_time;
			}
			// [ ���� ������ �����϶�� ����� �޾��� �� ]
			else if (frem[i]->e_factory_attack) {
				if (sys_clock <= frem[i]->attack_period) {
					return;
				}

				for (int j = 0; j < fac_count; j++) {
					if ((frem[i]->pos.row - 1 == FAC[j]->pos1.row && frem[i]->pos.column == FAC[j]->pos1.column) || \
						(frem[i]->pos.row - 1 == FAC[j]->pos2.row && frem[i]->pos.column == FAC[j]->pos2.column) || \
						(frem[i]->pos.row - 1 == FAC[j]->pos3.row && frem[i]->pos.column == FAC[j]->pos3.column) || \
						(frem[i]->pos.row - 1 == FAC[j]->pos4.row && frem[i]->pos.column == FAC[j]->pos4.column)) {
						if (FAC[j]->hp <= 0) {
							p_system_message("���� ������ �����࿡ ���� �ı��Ǿ����ϴ�.");
							FAC[j]->hp = 0;
							FAC[j]->repr = ' ';
							map[0][FAC[j]->pos1.row][FAC[j]->pos1.column] = ' ';
							map[0][FAC[j]->pos2.row][FAC[j]->pos2.column] = ' ';
							map[0][FAC[j]->pos3.row][FAC[j]->pos3.column] = ' ';
							map[0][FAC[j]->pos4.row][FAC[j]->pos4.column] = ' ';
							frem[i]->e_factory_attack = false;
						}
						else {
							FAC[j]->hp -= frem[i]->dps;
							p_system_message("�������� ������ �����߽��ϴ�.");
							char mes[100];
							sprintf_s(mes, sizeof(mes), "���� ������ �������� %d�� �����߽��ϴ�.", FAC[j]->hp);
							p_system_message(mes);
						}
					}
				}
				frem[i]->attack_period = sys_clock + frem[i]->next_attack_time;
			}
		}
	}
}


// [ ������ ���� ���� �� �� Ž��/ó�� ]
void frem_attack_unit(void) {
	for (int i = 0; i < frem_count; i++) {
		OBJECT_SAMPLE* current_frem = frem[i];
		if (!current_frem) continue; // ������ �˻�

		// [ ������ �þ� ���� ]
		int sight = current_frem->eyes;

		// [ ������ ���� ��ġ ]
		int current_row = current_frem->pos.row;
		int current_col = current_frem->pos.column;

		// [ Ž�� �÷��� �ʱ�ȭ ]
		bool target_found = false;

		// [ ������ �ֺ��� ���� �ִ��� Ȯ�� ]
		for (int row = current_row - sight; row <= current_row + sight && !target_found; row++) {
			for (int col = current_col - sight; col <= current_col + sight && !target_found; col++) {
				// [ �� ��� Ȯ�� ]
				if (row < 0 || row >= MAP_HEIGHT || col < 0 || col >= MAP_WIDTH) continue;

				// [ ������ �ڽ� ���� ]
				if (row == current_row && col == current_col) continue;

				// [ �� ���� Ž�� �� ó�� ]
				for (int j = 0; j < e_hav_count; j++) {
					if (row == e_havs[j]->pos.row && col == e_havs[j]->pos.column && check_friend[row][col] == 2 && e_havs[j]->repr == 'H') {
						if (!current_frem->enemy_found) { // ���� ó�� �߰����� ���� ���
							p_system_message("�������� ���� �Ϻ����͸� �߰��߽��ϴ�.");
							p_system_message("�������� ���� �Ϻ����͸� ��ǥ�� ��ҽ��ϴ�.");
							current_frem->enemy_found = true; // �߰� ���� ���
						}

						// [ ������ ���� - ����, �Ʒ�, ��, ������ ������ üũ ]
						POSITION potential_destinations[] = {
							{e_havs[j]->pos.row, e_havs[j]->pos.column - 1}, // ����
							{e_havs[j]->pos.row + 1, e_havs[j]->pos.column}, // �Ʒ�
							{e_havs[j]->pos.row - 1, e_havs[j]->pos.column}, // ��
							{e_havs[j]->pos.row, e_havs[j]->pos.column + 1}  // ������
						};

						for (int k = 0; k < 4; k++) {
							int dest_row = potential_destinations[k].row;
							int dest_col = potential_destinations[k].column;

							// [ �������� �� ��� �ȿ� �ְ�, �� �������� Ȯ�� ]
							if (dest_row >= 0 && dest_row < MAP_HEIGHT && dest_col >= 0 && dest_col < MAP_WIDTH &&
								map[0][dest_row][dest_col] == ' ') {
								current_frem->dest.row = dest_row;
								current_frem->dest.column = dest_col;
								break;
							}
						}

						// [ �� ���� ���� ó�� ]
						frem_kill_enemy_hav(current_frem, e_havs[j]);
						target_found = true;
						break;
					}
				}


				for (int j = 0; j < fighter_count && !target_found; j++) {
					if (row == fighter[j]->pos.row && col == fighter[j]->pos.column && frem_fight_fact_check[row][col] == 3 && fighter[j]->repr == 'F') {
						if (!current_frem->enemy_found) { // ���� ó�� �߰����� ���� ���
							p_system_message("�������� ���� ���縦 �߰��߽��ϴ�.");
							p_system_message("�������� ���� ���縦 ��ǥ�� ��ҽ��ϴ�.");
							current_frem->enemy_found = true; // �߰� ���� ���
						}

						// [ ������ ���� - ����, �Ʒ�, ��, ������ ������ üũ ]
						POSITION potential_destinations[] = {
							{fighter[j]->pos.row, fighter[j]->pos.column - 1}, // ����
							{fighter[j]->pos.row + 1, fighter[j]->pos.column}, // �Ʒ�
							{fighter[j]->pos.row - 1, fighter[j]->pos.column}, // ��
							{fighter[j]->pos.row, fighter[j]->pos.column + 1}  // ������
						};

						for (int k = 0; k < 4; k++) {
							int dest_row = potential_destinations[k].row;
							int dest_col = potential_destinations[k].column;

							// [ �������� �� ��� �ȿ� �ְ�, �� �������� Ȯ�� ]
							if (dest_row >= 0 && dest_row < MAP_HEIGHT && dest_col >= 0 && dest_col < MAP_WIDTH &&
								map[0][dest_row][dest_col] == ' ') {
								current_frem->dest.row = dest_row;
								current_frem->dest.column = dest_col;

								char mes[100];
								sprintf_s(mes, 100, "���� ��ġ:%d/%d, ����:%d/%d", fighter[j]->pos.row, fighter[j]->pos.column, dest_row, dest_col);
								p_system_message(mes);



								break;
							}
						}

						// [ �� ���� ���� ó�� ]
						frem_kill_enemy_fighter(current_frem, fighter[j]);
						target_found = true;
						break;
					}
				}

				for (int j = 0; j < tank_count && !target_found; j++) {
					if (row == tank[j]->pos.row && col == tank[j]->pos.column && tank[j]->repr == 'T') {
						if (!current_frem->enemy_found) { // ���� ó�� �߰����� ���� ���
							p_system_message("�������� ���� �������� �߰��߽��ϴ�.");
							p_system_message("�������� ���� �������� ��ǥ�� ��ҽ��ϴ�.");
							current_frem->enemy_found = true; // �߰� ���� ���
						}

						// [ ������ ���� - ����, �Ʒ�, ��, ������ ������ üũ ]
						POSITION potential_destinations[] = {
							{tank[j]->pos.row, tank[j]->pos.column - 1}, // ����
							{tank[j]->pos.row + 1, tank[j]->pos.column}, // �Ʒ�
							{tank[j]->pos.row - 1, tank[j]->pos.column}, // ��
							{tank[j]->pos.row, tank[j]->pos.column + 1}  // ������
						};

						for (int k = 0; k < 4; k++) {
							int dest_row = potential_destinations[k].row;
							int dest_col = potential_destinations[k].column;

							// [ �� �������� Ȯ�� ]
							if (map[0][dest_row][dest_col] == ' ') {
								current_frem->dest.row = dest_row;
								current_frem->dest.column = dest_col;
								break;
							}
						}

						// [ �� ���� ���� ó�� ]
						frem_kill_enemy_tank(current_frem, tank[j]);
						target_found = true;
						break;
					}
				}
			}
		}

		// [ ���� �߰����� ������ �� �߰� �÷��� �ʱ�ȭ ]
		if (!target_found) {
			current_frem->enemy_found = false;
		}
	}
}


// 1 - [ �� �Ϻ����� ó�� �Լ� ]
void frem_kill_enemy_hav(OBJECT_SAMPLE* frem, OBJECT_SAMPLE* harvester) {
	// [ ���� �ֱ� ]
		if (sys_clock <= frem->attack_period) {
			return;
		}

		if (frem->pos.row == frem->dest.row && frem->pos.column == frem->dest.column) {
			if (harvester->hp <= 0) {
				p_system_message("���� �Ϻ������� �����࿡ ���� ����Ͽ����ϴ�");
				harvester->repr = ' ';
				map[1][harvester->pos.row][harvester->pos.column] = harvester->repr;
				frem->enemy_found = false;
				frem_e_hav_find = false;
				frem_find_enemy = false;
			}
			else {
				harvester->hp -= frem->dps;
				char mes[100];
				sprintf_s(mes, sizeof(mes), "���� �Ϻ����Ͱ� �����࿡�� ���ݹ޾� hp�� %d�� �����߽��ϴ�", harvester->hp);
				p_system_message(mes);
			}
		}

		frem->attack_period = sys_clock + frem->next_attack_time;
}

// 2 - [ �� ���� ó�� �Լ� ]
void frem_kill_enemy_fighter(OBJECT_SAMPLE* frem, OBJECT_SAMPLE* fighter) {
		// [ ���� �ֱ� ]
		if (sys_clock <= frem->attack_period) {
			return;
		}
		p_system_message("1");
		// ������ ���� �� ����
		char mes[100];
		sprintf_s(mes, 100, "������: %d/%d --> ������: %d/%d", frem->pos.row, frem->pos.column, frem->dest.row, frem->dest.column);
		p_system_message(mes);
		if (frem->pos.row == frem->dest.row && frem->pos.column == frem->dest.column) {
			p_system_message("������ ����");
			if (fighter->hp <= 0) {
				p_system_message("���� ���簡 �����࿡ ���� ����Ͽ����ϴ�.");
				fighter->repr = ' ';
				map[1][fighter->pos.row][fighter->pos.column] = fighter->repr;
				frem->enemy_found = false;
				frem_fighter_find = false;
				frem_find_enemy = false;
			}
			else {
				fighter->hp -= 1;
				char mes[100];
				sprintf_s(mes, sizeof(mes), "���� ���簡 �����࿡�� ���ݹ޾� hp�� %d�� �����߽��ϴ�.", fighter->hp);
				p_system_message(mes);
			}
		}

		frem->attack_period = sys_clock + frem->next_attack_time;
}

// 3 - [ �� ������ ó�� �Լ� ]
void frem_kill_enemy_tank(OBJECT_SAMPLE* frem, OBJECT_SAMPLE* tank) {
		// [ ���� �ֱ� ]
		if (sys_clock <= frem->attack_period) {
			return;
		}

		if (frem->pos.row == frem->dest.row && frem->pos.column == frem->dest.column) {
			if (tank->hp <= 0) {
				p_system_message("���� �������� �����࿡ ���� �ı��Ǿ����ϴ�.");
				tank->repr = ' ';
				map[1][tank->pos.row][tank->pos.column] = tank->repr;
				frem->enemy_found = false;
				frem_tank_find = false;
				frem_find_enemy = false;
			}
			else {
				tank->hp -= frem->dps;
				char mes[100];
				sprintf_s(mes, sizeof(mes), "���� �������� �����࿡�� ���ݹ޾� hp�� %d�� �����߽��ϴ�.", tank->hp);
				p_system_message(mes);
			}
		}

		frem->attack_period = sys_clock + frem->next_attack_time;
}

// [ ������ �̵� �Լ� ]
POSITION frem_move(OBJECT_SAMPLE* frem, POSITION target_pos) {
	POSITION diff = psub(target_pos, frem->pos);
	DIRECTION dir;

	// ������� ������ �� �� �� ������ �̵� ���� ����
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// �⺻ �̵� ��ġ ���
	POSITION next_pos = pmove(frem->pos, dir);

	// �� �� ���� Ȯ��
	if (next_pos.row < 1 || next_pos.row >= MAP_HEIGHT - 1 ||
		next_pos.column < 1 || next_pos.column >= MAP_WIDTH - 1) {
		return frem->pos; // �̵��� �� ������ ���� ��ġ ����
	}

	// ��ֹ� Ȯ�� �� ��ü ��� ���
	char target_tile = map[0][next_pos.row][next_pos.column]; // �ǹ�
	char target_tile2 = map[1][next_pos.row][next_pos.column]; // �����
	if (target_tile != ' ' || target_tile2 == 'W') {
		// ��ֹ��� ȸ���ϱ� ���� ��ü ��� ���
		double best_distance = DBL_MAX;
		POSITION best_pos = frem->pos;

		// �����¿�� ������ ��ü ��� Ž��
		DIRECTION alternate_dirs[] = { d_up, d_down, d_left, d_right };
		for (int j = 0; j < 4; j++) {
			POSITION alt_pos = pmove(frem->pos, alternate_dirs[j]);

			// ��ü ��ΰ� �� ���� �ִ��� Ȯ��
			if (alt_pos.row < 1 || alt_pos.row >= MAP_HEIGHT - 1 ||
				alt_pos.column < 1 || alt_pos.column >= MAP_WIDTH - 1) {
				continue; // �� ���̸� ��ŵ
			}

			// ��ü ��ΰ� ��ֹ��� �ƴ��� Ȯ��
			char alt_tile = map[0][alt_pos.row][alt_pos.column];
			if (alt_tile == ' ' && target_tile2 != 'W') {
				double distance = sqrt(pow(target_pos.row - alt_pos.row, 2) +
					pow(target_pos.column - alt_pos.column, 2));
				if (distance < best_distance) {
					best_distance = distance;
					best_pos = alt_pos;
				}
			}
		}
		return best_pos; // ���� ����� ��ü ��� ��ȯ
	}

	return next_pos; // �⺻ �̵� �����ϸ� ��ȯ
}

// [ ������ ���� �Լ� ]
void frem_main_func(void) {
	// [ ������ ���� ]
	select_frem_act();

	// [ �̵� �ֱ� ]
	for (int i = 0; i < frem_count; i++) {
		if (sys_clock <= frem[i]->move_period) {
			return;
		}
	}
	// [ ������ �̵� �� ���� ���� ]
	for (int i = 0; i < frem_count; i++) {
		// ��� �˻� �߰�
		if (i < 0 || i >= MAX_FREM) {
			continue;  // ��ȿ���� ���� �ε��� �ǳʶٱ�
		}

		POSITION target_pos = { -1, -1 };

		// [ �̵� ]
		if ((frem[i]->dest.row != -1 && frem[i]->dest.column != -1) &&
			(frem[i]->pos.row != frem[i]->dest.row || frem[i]->pos.column != frem[i]->dest.column)) {
			target_pos = frem[i]->dest;
		}

		// [ ���� ]
		else if (frem[i]->is_patrolling) {
			// [ ���� ��ġ���� patrol_point�� �����ߴٸ� ]
			if (frem[i]->pos.row == frem[i]->patrol_point.row &&
				frem[i]->pos.column == frem[i]->patrol_point.column) {
				// [ ���� �������� ���ư��� ]
				frem[i]->dest = frem[i]->original_dest;
				target_pos = frem[i]->dest;
			}
			// [ dest ��ġ�� �����ߴٸ� ]
			else if (frem[i]->pos.row == frem[i]->dest.row &&
				frem[i]->pos.column == frem[i]->dest.column) {
				// [ patrol_point�� �̵� ]
				frem[i]->original_dest = frem[i]->dest;  // ���� ������ ����
				frem[i]->dest = frem[i]->patrol_point;
				target_pos = frem[i]->dest;
			}
			// [ ���� �������� �������� �ʾҴٸ� ���� dest�� �̵� ]
			else {
				target_pos = frem[i]->dest;
			}
		}

		// [ ������ �̵� ]
		if (target_pos.row != -1 && target_pos.column != -1) {
			// [ ���� ��ġ ����� ]
			if (frem[i]->pos.row >= 0 && frem[i]->pos.row < MAP_HEIGHT &&
				frem[i]->pos.column >= 0 && frem[i]->pos.column < MAP_WIDTH &&
				map[1][frem[i]->pos.row][frem[i]->pos.column] == frem[i]->repr) {
				map[1][frem[i]->pos.row][frem[i]->pos.column] = ' ';
				frem_fight_fact_check[frem[i]->pos.row][frem[i]->pos.column] = 0;
			}

			// [ ������ �̵� ]
			POSITION next_pos = frem_move(frem[i], target_pos);
			frem[i]->pos = next_pos;

			// [ �� ��ġ�� ���� ǥ�� ]
			if (frem[i]->pos.row >= 0 && frem[i]->pos.row < MAP_HEIGHT &&
				frem[i]->pos.column >= 0 && frem[i]->pos.column < MAP_WIDTH) {
				map[1][frem[i]->pos.row][frem[i]->pos.column] = frem[i]->repr;
				frem_fight_fact_check[frem[i]->pos.row][frem[i]->pos.column] = 2;
			}
		}
	}

	// [ ������ �ǹ� ���� ]
	frem_attack_building();

	// [ ������ ���� ���� ]
	frem_attack_unit();



	// [ �̵� �ֱ� ]
	for (int i = 0; i < frem_count; i++) {
		frem[i]->move_period = sys_clock + frem[i]->next_move_time;
	}
}







/* -=-=-=-=-=-=-=-=[ ���ڳ� ���� AI ]-=-=-=-=-=-=-=-= -*/


// [ ���ڳ� �ʱ� ���� �ʱ�ȭ ]
void hako_unit_reset(void) {
	fighter[0] = &fighter1;
	tank[0] = &tank1;
}


// [ ���ڳ� �ʱ� ���� ��ġ ]
void hako_unit(void) {
	// [ ���� �ʱ�ȭ ]
	hako_unit_reset();

	// [ ���� ��� ] 
	// [ ���� ]
	map[1][fighter[0]->pos.row][fighter[0]->pos.column] = fighter[0]->repr;
	check_friend[fighter[0]->pos.row][fighter[0]->pos.column] = 2;
	frem_fight_fact_check[fighter[0]->pos.row][fighter[0]->pos.column] = 3;
	// [ ������ ]
	map[1][tank[0]->pos.row][tank[0]->pos.column] = tank[0]->repr;
	check_friend[tank[0]->pos.row][tank[0]->pos.column] = 2;

}

