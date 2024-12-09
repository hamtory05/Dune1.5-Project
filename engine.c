// 현재 이 코드는 6번 거의 다 한 상태입니다. 각 건물에 관한 상태창과 명령어만 넣어주면 됩니다. 
// 5번까지는 완성했습니다.


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

// [ 샌드웜 ]
POSITION sw1_next_pos(void);
void sw1_move(void);

POSITION sw2_next_pos(void);
void sw2_move(void);

// [ 사막 독수리 ]
POSITION d_eagle_next_pos(void);
void d_eagle_move(void);

// [ 모래 폭풍 (X) ]
POSITION sand_wind_next_pos(void);
void sand_wind_move(void);

// [ 시스템 메시지 ]
char* send_system_message[1];

// [ KEY RESET ]
void reset_key_false();

// ==== [ 하베스터 & 스파이스 관련 ] ====
// [ 아군 ]
OBJECT_SAMPLE* havs[MAX_HAV]; // 하베스터 배열
int hav_count = 1; // 하베스터 갯수
int selected_harvester = -1; // 선택된 하베스터 인덱스 (-1: 선택 안됨)

// [ 적군 ]
OBJECT_SAMPLE* e_havs[MAX_HAV]; // 하베스터 배열
int e_hav_count = 1; // 하베스터 갯수


void hav_reset(void); // 하베스터 초기화
void harvest_move_all(OBJECT_SAMPLE havs[MAX_HAV], int hav_count, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], OBJECT_BUILDING* SPICE); // 하베스터 이동 메인
int harvest_gather_spice(OBJECT_BUILDING* spice, OBJECT_SAMPLE* hav, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]); // 하베스터 스파이스 수확
POSITION harvest_next_move(OBJECT_SAMPLE* hav, POSITION new_dest); // 하베스터 이동
POSITION harvest_set_dest(POSITION curr, OBJECT_SAMPLE* hav, OBJECT_BUILDING* SPICE); // 하베스터 목적지 정하기
void handle_harvester_input(CURSOR cursor, OBJECT_SAMPLE havs[MAX_HAV], int hav_count, OBJECT_BUILDING* SPICE); // 하베스터 선택 및 스파이스 선택

int spice_in_progress[MAX_SPICE] = { -1 }; // 각 스파이스 매장지에 대한 작업 상태 (-1: 작업 없음)


// ==== [ 보병 ] ====
OBJECT_SAMPLE* sold[MAX_SOLD]; // 보병 배열
int sold_count = 0; // 보병 갯수 
int selected_sold = -1; // 보병이 선택되었는지 확인

extern int base_barr_check[MAP_HEIGHT][MAP_WIDTH]; 

bool sold_p_key_press = false; // 보병 순찰 p키 bool
bool sold_e_hav_find = false;
bool sold_fighter_find = false;
bool sold_tank_find = false;


void select_sold_act(void); // 보병 행동 선택 
void sold_attack_building(void); // 보병 건물 공격
void sold_attack_unit(void); // 보병 유닛 공격
void process_enemy_unit(int row, int col, OBJECT_SAMPLE* soldier); // 보병 공격 처리
void handle_enemy_harvester(OBJECT_SAMPLE* soldier, OBJECT_SAMPLE* harvester); // 보병 하베스터 공격
void handle_enemy_fighter(OBJECT_SAMPLE* soldier, OBJECT_SAMPLE* fighter); // 보병 투사 공격
void handle_enemy_tank(OBJECT_SAMPLE* soldier, OBJECT_SAMPLE* tank); // 보병 중전차 공격
POSITION sold_move(OBJECT_SAMPLE* sold_unit, POSITION target_pos); // 보병 이동 
void main_sold_func(void); // 보병 메인 함수


// ==== [ 프레멘 ] ====
OBJECT_SAMPLE* frem[MAX_FREM]; // 프레멘 배열
int frem_count = 0; // 프레멘 갯수 
int selected_frem = -1; // 프레멘 선택되었는지 확인
extern int frem_fight_fact_check[MAP_HEIGHT][MAP_WIDTH];

bool frem_p_key_press = false; // 프레멘 순찰 p키 눌렀는지 확인하는 bool
bool frem_e_hav_find = false;
bool frem_fighter_find = false;
bool frem_tank_find = false;
bool frem_find_enemy = false;

void select_frem_act(void); // 프레멘 행동 선택 
void frem_attack_building(void); // 프레멘 건물 공격 
void frem_attack_unit(void); // 프레멘 유닛 공격
void frem_kill_enemy_hav(OBJECT_SAMPLE* frem, OBJECT_SAMPLE* harvester); // 프레멘 하베스터 공격
void frem_kill_enemy_fighter(OBJECT_SAMPLE* frem, OBJECT_SAMPLE* fighter); // 프레멘 투사 공격
void frem_kill_enemy_tank(OBJECT_SAMPLE* frem, OBJECT_SAMPLE* tank); // 프레멘 중전차 공격
POSITION frem_move(OBJECT_SAMPLE* frem, POSITION target_pos); // 프레멘 이동
void frem_main_func(void); // 프레멘 이동 메인 함수


// [ 투사 ]
OBJECT_SAMPLE* fighter[MAX_FIGHTER]; // 투사 배열
int fighter_count = 1; // 투사 갯수

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


// [ 중전차 ]
OBJECT_SAMPLE* tank[MAX_TANK]; // 중전차 배열
int tank_count = 1; // 중전차 갯수

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

// [ 건물 불러오기 ]
extern OBJECT_BUILDING e_base; // 적군 본진

extern OBJECT_BUILDING* SPICE; // 스파이스
extern int spi_count; // 스파이스 갯수

extern OBJECT_BUILDING* DOR[MAX_DOR]; // 숙소
extern int dor_count; // 숙소 갯수

extern OBJECT_BUILDING* GAR[MAX_GAR]; // 창고 
extern int gar_count; // 창고 갯수

extern OBJECT_BUILDING* BAR[MAX_BAR]; // 병영
extern int bar_count; // 병영 갯수

extern OBJECT_BUILDING* SHE[MAX_SHE]; // 은신처
extern int she_count; // 은신처 갯수

extern OBJECT_BUILDING* ARE[MAX_ARE]; // 아레나
extern int are_count; // 아레나 갯수

extern OBJECT_BUILDING* FAC[MAX_FAC]; // 공장
extern int fac_count; // 공장 갯수

extern OBJECT_BUILDING* E_DOR[MAX_DOR]; // 적군 숙소
extern int e_dor_count; // 적국 숙소 갯수

extern OBJECT_BUILDING* E_GAR[MAX_GAR]; // 적군 창고
extern int e_gar_count; // 적군 창고 갯수

extern shle_sold_check[MAP_HEIGHT][MAP_WIDTH]; // 은신처  - 1, 보병 - 2 구분


/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
CURSOR cursor = { { 1, 1 }, {1, 1} };

int move_count = 0;
int move_check[2] = { 0 };
char spice_number[9] = { '1', '2', '3', '4', '5', '6', '7', '8', '9' }; // 스파이스 숫자
int spice_plus_value = 0; // 더할 스파이스 갯수
int* spice_plus = &spice_plus_value; // 스파이스 더할 때 쓰는 변수

/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };  // 2, 16, 60
char state_map[STATE_HEIGHT][STATE_WIDTH] = { 0 }; // 16, 50
char sysmes_map[SYSMES_HEIGHT][SYSMES_WIDTH] = { 0 }; // 6 60
char order_map[ORDER_HEIGHT][ORDER_WIDTH] = { 0 }; // 6 60
int check_friend[MAP_HEIGHT][MAP_WIDTH] = { 0 }; // 0 --> 아무것도 아님, 1 --> 아군, 2 --> 적군
int hav_gather_spice[MAX_HAV] = { 0 }; // 0 스파이스를 캐지 않음 / 1면 스파이스를 캔 것
POSITION pre_hav_dest[MAX_HAV] = { 0 };

bool b_key_press = false; // 장판 설치 할 때 b키를 눌렀는지 확인
bool big_cursor = false; // 2X2 커서 활성화, 비활성화
bool p_key_press = false; // 장판 설치 확인
bool d_key_press = false; // 숙소 설치 확인
bool g_key_press = false; // 창고 설치 확인
bool b_b_key_press = false; // 병영 설치 확인
bool s_key_press = false;  // 은신처 설치 확인
bool space_key_press = false; // 스페이스바가 눌렸는지 확인 / false = 비활성화, true = 활성화
bool hav_move = false; // 하베스터 이동 확인
bool h_key_press = false; // h키 눌렸는지 확인
bool space_and_h_key = false; // 스페이스 누르고 h키 눌렀는지 확인
bool space_and_m_key = false; // 스페이스 누르고 m키 눌렀는지 확인
bool m_key_press = false; // 이동 확인
bool p_p_key_press = false; // 순찰 확인


// [ 자원 ]
RESOURCE resource = {
	.spice = 10,
	.spice_max = 50,
	.population = 5,
	.population_max = 30
};

// [ obj 기본 틀 ]
OBJECT_SAMPLE obj = { 
	.pos = {1, 1},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'o',
	.move_period = 300,
	.next_move_time = 300
};  

// [ 아군 하베스터 ]
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

// [ 적군 하베스터 ]
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

// [ 샌드웜 1 ]
OBJECT_SAMPLE sw1_obj = {
	.pos = {2, 4},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'W',
	.move_period = 2500,
	.next_move_time = 2500
};

// [ 샌드웜 2 ]
OBJECT_SAMPLE sw2_obj = {
	.pos = {12, 55},
	.dest = {1, 1},
	.repr = 'W',
	.move_period = 2500,
	.next_move_time = 2500
};

// [ 사막 독수리 ]
OBJECT_SAMPLE d_eagle = {
	.pos = {1, 1},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'E',
	.move_period = 500,
	.next_move_time = 500
};

// [ 모래 폭풍 ]
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

// [ 하코넨 ]
void hako_unit(void);
void hako_unit_reset(void);

/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL));

	hav_reset(); // 하베스터 초기화
	init();
	intro();
	display(resource, map, cursor, state_map, sysmes_map, order_map, check_friend, big_cursor);
	p_building(map, check_friend); // 건물 출력
	// [ 하코넨 ]
	hako_building(map, check_friend); 
	hako_unit();

	// [ 시스템 메시지 추가 ]
	p_system_message("게임이 시작되었습니다.");

	while (1) {
		// [ loop 돌 때마다(즉, TICK==10ms마다) 키 입력 확인 ]
		KEY key = get_key();

		// [ sys_clock 값에 따라 커서 여러칸 이동 변수 초기화 ]
		if (sys_clock % 110 == 0) {
			move_count = 0; // 커서가 1칸 움직이는지 3칸 움직이는지 확인
			for (int i = 0; i < 2; i++) {
				move_check[i] = 0; // 3칸 움직이는 커서일 때, 동일한 방향키를 두번 눌렀는지 확인
			}
		}

		// [ 키 입력이 있을 때, 1칸이동인지, 3칸이동인지 확인 ]
		if (is_arrow_key(key)) {
			move_count++;
			if (move_count == 1) {
				move_check[0] = key;
			}
			else if (move_count == 2) {
				move_check[1] = key;
			}
			// 3칸 움직이는 경우, 방향키가 다르다면 한칸 움직이게 작동
			if (move_count == 2 && move_check[1] != move_check[0]) {
				move_count = 1;
				for (int i = 0; i < 2; i++) {
					move_check[i] = 0; // 초기화
				}
			}
		}
		
		// [ 키 입력이 있으면 처리 ]
		if (is_arrow_key(key) && move_count == 2 && move_check[0] == move_check[1]) {
			cursor_move(ktod(key), 1);
			move_count = 0; // 3칸 이동하고 나서 변수 초기화.
		}
		else if (is_arrow_key(key) && move_count == 1) {
			cursor_move(ktod(key), 0);
		}
		else {
			// 방향키 외의 입력
			switch (key) {
			// [ Q ]
			case k_quit: outro();
			
			// [ ESC ]
			case k_esc: state_esc();
				// [ 시스템 메시지 추가 ]
				p_system_message("ESC키를 눌렀습니다.");
				b_key_press = false;
				space_key_press = false;

				reset_key_false();
				break;

			// [ SPACE ]
			case k_space: 
				space_key_press = true;
				state_spacebar(&resource, cursor, &f_hav_obj, &e_hav_obj, check_friend, p_key_press, map, d_key_press, g_key_press, 
				b_b_key_press, s_key_press, havs, &space_key_press);
				// [ 시스템 메시지 추가 ]
				p_system_message("스페이스바를 눌렀습니다.");

				// [ KEY FALSE ]
				reset_key_false();
				break;

			// [ H ]
			case k_h: press_h(&resource, map, check_friend, cursor, &f_hav_obj, hav_count, havs);
				h_key_press = true;
				// [ 시스템 메시지 추가 ]
				p_system_message("H키를 눌렀습니다.");
				break;

			// [ B ]
			case k_b: press_b(&resource, cursor, check_friend);

				// [ 병영 ]
				if (map[0][cursor.current.row][cursor.current.column] == 'P') {
					if (b_key_press) { // b키를 눌렀을 때
						press_b_b();
						b_b_key_press = true; // 병영 설치 활성화 
						b_key_press = false;  // b키 비활성화
					}
				}

				// [ 시스템 메시지 추가 ]
				p_system_message("B키를 눌렀습니다.");
				b_key_press = true; // 건물 키 입력
				break;

			// [ P ]
			case k_p: 
				// [ 시스템 메시지 추가 ]
				p_system_message("P키를 눌렀습니다.");
				
				// [ 장판 건설 ]
				if (map[0][cursor.current.row][cursor.current.column] == ' ') {
					if (b_key_press) {
						big_cursor = true;
						p_key_press = true;
						press_p();
					}
				}
				// [ 보병, 프레멘 순찰 선택 ]
				if (map[0][cursor.current.row][cursor.current.column] == ' ') {
					sold_p_key_press = true;
					frem_p_key_press = true;
				}
				
				b_key_press = false;
				break;

			// [ D ]
			case k_d: 
				// [ 시스템 메시지 추가 ]
				p_system_message("D키를 눌렀습니다.");
				
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
				// [ 시스템 메시지 추가 ]
				p_system_message("G키를 눌렀습니다.");
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
				// [ 시스템 메시지 추가 ]
				p_system_message("S키를 눌렀습니다.");

				// [ 은신처 ]
				if (map[0][cursor.current.row][cursor.current.column] == 'P') {
					if (b_key_press) {
						press_s();
						big_cursor = true;
						s_key_press = true;
					}
				}

				// [ 보병 ]
				press_s_s(cursor, &resource, map, &space_key_press);
				
				space_key_press = false;
				b_key_press = false;
				break;

			// [ M ]
			case k_m:
				m_key_press = true;
				p_system_message("M키를 눌렀습니다.");
				break;

			// [ F ]
			case k_f:
				p_system_message("F키를 눌렀습니다.");
				press_f(cursor, &resource, map, &space_key_press);
				break;

			case k_none:
			case k_undef:
			default: break;
			}
		}
	
		// [ 오브젝트 동작 ]
		//d_eagle_move();
		//sw1_move();
		//sw2_move(); 

		// [ 하베스터 움직이기 ]
		harvest_move_all(*havs, hav_count, map, SPICE);

		// [ 보병 움직이기 ]
		main_sold_func();
		
		// [ 프레멘 움직이기 ]
		frem_main_func();

		// [ 화면 출력 ]
		display(resource, map, cursor, state_map, sysmes_map, order_map, check_friend, big_cursor);
		Sleep(TICK);
		sys_clock += 10;
	}
}


/* ================= [ subfunctions ] =================== */

// [ 초기 하베스터 초기화 ]
void hav_reset(void) {
	for (int i = 0; i < MAX_HAV; i++) {
		havs[i] = NULL; // 포인터 초기화
	}

	for (int i = 0; i < MAX_HAV; i++) {
		e_havs[i] = NULL; // 포인터 초기화
	}

	e_havs[0] = &e_hav_obj; // 첫 번째 하베스터 초기화
	havs[0] = &f_hav_obj; // 첫 번째 하베스터 초기화
}

// [ 인트로 ]
void intro(void) {
	printf("DUNE 1.5\n");
	Sleep(2000);
	system("cls");
}

// [ 아웃트로 ]
void outro(void) {
	// 화면 전체를 싹다 빈칸으로 만든 후 게임 오버라고 출력하게 하기
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
	// [ layer 0(map[0])에 지형 생성 ]
	for (int j = 0; j < MAP_WIDTH; j++) {
		map[0][0][j] = '#';
		map[0][MAP_HEIGHT - 1][j] = '#';
	}

	// [ layer 0(map[0])에 지형 생성 ]
	for (int i = 1; i < MAP_HEIGHT - 1; i++) {
		map[0][i][0] = '#';
		map[0][i][MAP_WIDTH - 1] = '#';
		for (int j = 1; j < MAP_WIDTH - 1; j++) {
			map[0][i][j] = ' ';
		}
	}

	// [ layer 1(map[1])은 비워 두기(-1로 채움) ]
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[1][i][j] = -1;
		}
	}

	// [ layer 2(map[2])은 비워 두기(-1로 채움) ]
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[2][i][j] = -1;
		}
	}
}

void init_state(void) {
	// [ 상태창 기본 틀 ]
	for (int i = 0; i < STATE_WIDTH; i++) {
		state_map[0][i] = '#';
		state_map[STATE_HEIGHT - 1][i] = '#';
	}

	// [ 상태창 기본 틀 ]
	for (int i = 1; i < STATE_HEIGHT - 1; i++) {
		state_map[i][0] = '#';
		state_map[i][STATE_WIDTH - 1] = '#';
		for (int j = 1; j < STATE_WIDTH - 1; j++) {
			state_map[i][j] = ' ';
		}
	}
} 

void init_sysmes(void) {
	// [ 시스템 기본 틀 ]
	for (int i = 0; i < SYSMES_WIDTH; i++) {
		sysmes_map[0][i] = '#';
		sysmes_map[SYSMES_HEIGHT - 1][i] = '#';
	}

	// [ 시스템 기본 틀 ]
	for (int i = 1; i < SYSMES_HEIGHT - 1; i++) {
		sysmes_map[i][0] = '#';
		sysmes_map[i][SYSMES_WIDTH - 1] = '#';
		for (int j = 1; j < SYSMES_WIDTH - 1; j++) {
			sysmes_map[i][j] = ' ';
		}
	}
} 

void init_order(void) {
	// [ 명령창 기본 틀 ]
	for (int i = 0; i < ORDER_WIDTH; i++) {
		order_map[0][i] = '#';
		order_map[ORDER_HEIGHT - 1][i] = '#';
	}

	// [ 명령창 기본 틀 ]
	for (int i = 1; i < ORDER_HEIGHT - 1; i++) {
		order_map[i][0] = '#';
		order_map[i][ORDER_WIDTH - 1] = '#';
		for (int j = 1; j < ORDER_WIDTH - 1; j++) {
			order_map[i][j] = ' ';
		}
	}
} 

void init_unit(void) {
	// [ 사막 독수리 ]
	//map[2][d_eagle.pos.row][d_eagle.pos.column] = d_eagle.repr;

	// [ 아군 하베스터 ]
	map[1][f_hav_obj.pos.row][f_hav_obj.pos.column] = f_hav_obj.repr; 
	check_friend[f_hav_obj.pos.row][f_hav_obj.pos.column] = 1;

	// [ 적군 하베스터 ]
	map[1][e_hav_obj.pos.row][e_hav_obj.pos.column] = e_hav_obj.repr;
	check_friend[e_hav_obj.pos.row][e_hav_obj.pos.column] = 2;

	// [ 샌드웜 ]
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

// [ 커서 이동 ]
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
	else { // 커서의 위치가 범위에서 벗어나려고 하면 계속 그 위치에 위치하게 하기.
		cursor.previous = cursor.previous;
		cursor.current = cursor.current;
	}
}


/* ================= [ 유닛 샘플 함수 ] =================== */
POSITION sample_obj_next_position(void) {
	// 현재 위치와 목적지를 비교해서 이동 방향 결정	
	POSITION diff = psub(obj.dest, obj.pos);
	DIRECTION dir;

	// 목적지 도착. 지금은 단순히 원래 자리로 왕복
	if (diff.row == 0 && diff.column == 0) {
		if (obj.dest.row == 1 && obj.dest.column == 1) {
			// topleft --> bottomright로 목적지 설정
			POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
			obj.dest = new_dest;
		}
		else {
			// bottomright --> topleft로 목적지 설정
			POSITION new_dest = { 1, 1 };
			obj.dest = new_dest;
		}
		return obj.pos;
	}

	// 가로축, 세로축 거리를 비교해서 더 먼 쪽 축으로 이동
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// validation check
	// next_pos가 맵을 벗어나지 않고, (지금은 없지만)장애물에 부딪히지 않으면 다음 위치로 이동
	// 지금은 충돌 시 아무것도 안 하는데, 나중에는 장애물을 피해가거나 적과 전투를 하거나... 등등
	POSITION next_pos = pmove(obj.pos, dir);
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] < 0) {

		return next_pos;
	}
	else {
		return obj.pos;  // 제자리
	}
}

void sample_obj_move(void) {
	if (sys_clock <= obj.next_move_time) {
		// 아직 시간이 안 됐음
		return;
	}

	// 오브젝트(건물, 유닛 등)은 layer1(map[1])에 저장
	map[1][obj.pos.row][obj.pos.column] = -1;
	obj.pos = sample_obj_next_position();
	map[1][obj.pos.row][obj.pos.column] = obj.repr;

	obj.next_move_time = sys_clock + obj.move_period;
}


/* ================= [ 유닛 움직임 함수 ] =================== */

// [ 사막 독수리 (X) ]
POSITION d_eagle_next_pos(void) {
	// 현재 위치와 목적지를 비교해서 이동 방향 결정	
	POSITION diff = psub(d_eagle.dest, d_eagle.pos);
	DIRECTION dir;

	// 목적지 도착. 지금은 단순히 원래 자리로 왕복
	if (diff.row == 0 && diff.column == 0) {
		if (d_eagle.dest.row == 1 && d_eagle.dest.column == 1) {
			// topleft --> bottomright로 목적지 설정
			POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
			d_eagle.dest = new_dest;
		}
		else {
			// bottomright --> topleft로 목적지 설정
			POSITION new_dest = { 1, 1 };
			d_eagle.dest = new_dest;
		}
		return d_eagle.pos;
	}

	// 가로축, 세로축 거리를 비교해서 더 먼 쪽 축으로 이동
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
		return d_eagle.pos;  // 제자리
	}
}

// [ 사막 독수리 움직임 함수 (X ]
void d_eagle_move(void) {
	if (sys_clock <= d_eagle.next_move_time) {
		return;
	}

	map[2][d_eagle.pos.row][d_eagle.pos.column] = -1;

	d_eagle.pos = d_eagle_next_pos();
	map[2][d_eagle.pos.row][d_eagle.pos.column] = d_eagle.repr;
	d_eagle.next_move_time = sys_clock + d_eagle.move_period;
}


// [ 샌드웜 (1) ]
POSITION sw1_next_pos(void) {
	// [ 현재 위치와 목적지를 비교해서 이동 방향 결정 ]
	POSITION diff = psub(sw1_obj.dest, sw1_obj.pos);
	DIRECTION dir;

	// [ 가장 가까운 유닛을 찾아서 그 유닛의 행렬값을 new_dest에 지정 ]
	double check_close = 61.0;
	int move_i = 2, move_j = 4;
	for (int i = 1; i < MAP_HEIGHT - 1; i++) {
		for (int j = 1; j < MAP_WIDTH - 1; j++) {
			if (map[1][i][j] == 'H' || map[1][i][j] == 'S' || (map[1][i][j] == 'F' && 1 /* 1은 나중에 수정 프레멘과 투사 구분 */)) {
				if (sqrt(pow(i - sw1_obj.pos.row, 2) + pow(j - sw1_obj.pos.column, 2)) < check_close) {
					check_close = sqrt(pow(i - sw1_obj.pos.row, 2) + pow(j - sw1_obj.pos.column, 2));
					move_i = i, move_j = j;
				}
			}
		}
	}
	POSITION new_dest = { move_i, move_j };
	sw1_obj.dest = new_dest;

	// [ 가로축, 세로축 거리를 비교해서 더 먼 쪽 축으로 이동 ]
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// [ 샌드웜 이동 ]
	POSITION next_pos = pmove(sw1_obj.pos, dir);

	// [ 건물, 장애물 만났을 때 피해가기 ]
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[0][next_pos.row][next_pos.column] == 'R' || map[0][next_pos.row][next_pos.column] == '1' || map[0][next_pos.row][next_pos.column] == '2' || \
		map[0][next_pos.row][next_pos.column] == '3' || map[0][next_pos.row][next_pos.column] == '4' || map[0][next_pos.row][next_pos.column] == '5' || \
		map[0][next_pos.row][next_pos.column] == '6' || map[0][next_pos.row][next_pos.column] == '7' || map[0][next_pos.row][next_pos.column] == '8' || \
		map[0][next_pos.row][next_pos.column] == '9' || map[0][next_pos.row][next_pos.column] == 'B' || map[0][next_pos.row][next_pos.column] == 'P' || \
		map[0][next_pos.row][next_pos.column] == 'S' || map[0][next_pos.row][next_pos.column] == 'G' || map[0][next_pos.row][next_pos.column] == 'D' || \
		map[0][next_pos.row][next_pos.column] == 'A' || map[0][next_pos.row][next_pos.column] == 'S' || map[0][next_pos.row][next_pos.column] == 'F') {

		// [ 건물, 장애물 위, 아래에 있을 때 ]
		if (sw1_obj.pos.row + 1 == next_pos.row || sw1_obj.pos.row - 1 == next_pos.row) {
			// [ 오른쪽으로 갈 때가 빠른지 왼쪽으로 갈 때가 빠른지 비교 ]
			double move_left = sqrt(pow(sw1_obj.pos.row - new_dest.row, 2) + pow((sw1_obj.pos.column - 1) - new_dest.column, 2));
			double move_right = sqrt(pow(sw1_obj.pos.row - new_dest.row, 2) + pow((sw1_obj.pos.column + 1) - new_dest.column, 2));
			// [ 오른쪽으로 가기 ]
			if (move_left < move_right) {
				// < 오른쪽으로 갔을 때 맵을 벗어나게 되는지를 체크 >
				if (sw1_obj.pos.column + 1 != MAP_WIDTH - 1) { // 맵을 벗어나지 않으면 오른쪽
					next_pos.row += 1;
					next_pos.column = sw1_obj.pos.column + 1;
				}
				else { // 맵을 벗어나면 왼쪽
					next_pos.row -= 1;
					next_pos.column = sw1_obj.pos.column - 1;
				}
			}
			// [ 왼쪽으로 가기 ]
			else {
				// < 왼쪽으로 갔을 때 맵을 벗어나게 되는지를 체크 >
				if (sw1_obj.pos.column - 1 != 0) { // 맵을 벗어나지 않으면 왼쪽
					next_pos.row -= 1;
					next_pos.column = sw1_obj.pos.column - 1;
				}
				else { // 맵을 벗어나면 오른쪽
					next_pos.row += 1;
					next_pos.column = sw1_obj.pos.column + 1;
				}
			}
		}
		// [ 건물, 장애물 오른쪽, 왼쪽에 있을 때 ]
		else if (sw1_obj.pos.column + 1 == next_pos.column || sw1_obj.pos.column - 1 == next_pos.column) {
			// [ 위로 갈 때가 빠른지 아래로 갈 때가 빠른지 비교 ]
			double move_up = sqrt(pow((sw1_obj.pos.row - 1) - new_dest.row, 2) + pow(sw1_obj.pos.column - new_dest.column, 2));
			double move_down = sqrt(pow((sw1_obj.pos.row + 1) - new_dest.row, 2) + pow(sw1_obj.pos.column - new_dest.column, 2));
			// [ 위로 가기 ]
			if (move_up < move_down) {
				// < 위로 갔을 때 맵을 벗어나는지 확인 >
				if (sw1_obj.pos.row + 1 != 0) { // 맵을 벗어나지 않으면 위로
					next_pos.column += 1;
					next_pos.row = sw1_obj.pos.row + 1;
				}
				else { // 맵을 벗어나면 아래로
					next_pos.column -= 1;
					next_pos.row = sw1_obj.pos.row - 1;
				}
			}
			// [ 아래로 가기 ]
			else {
				if (sw1_obj.pos.row - 1 != MAP_HEIGHT - 1) { // 맵을 벗어나면 아래로
					next_pos.column -= 1;
					next_pos.row = sw1_obj.pos.row - 1;
				}
				else { // 맵을 벗어나면 위로
					next_pos.column += 1;
					next_pos.row = sw1_obj.pos.row + 1;
				}
			}
		}
		return next_pos;
	}
	return next_pos; 
}

// [샌드웜 (1) 움직임 함수 ]
void sw1_move(void) {
	// [ 움직이는 주기 ]
	if (sys_clock <= sw1_obj.next_move_time) {
		return;
	}
	// [ 샌드웜(1) layer1 (map[1])에 저장 ]
	map[1][sw1_obj.pos.row][sw1_obj.pos.column] = -1;

	// [ 10/300 확률로 배설 --> 스파이스 매장지 생성 (매장량 1 ~ 9 랜덤) ]
	int r = rand() % 299;
	if (r < 9) {
		// [ 시스템 메시지 추가 ]
		send_system_message[0] = "샌드웜이 스파이스 매장지를 생성했습니다.";
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

	// [ 샌드웜 잡아먹기 ]
	// [ 아군 하베스터와 만났을 때 ]
	for (int i = 0; i < hav_count; i++) {
		if (map[1][sw1_obj.pos.row][sw1_obj.pos.column] == 'H'  && check_friend[sw1_obj.pos.row][sw1_obj.pos.column] == 1) {
			// [ 시스템 메시지 추가 ]
			send_system_message[0] = "샌드웜이 아군 하베스터를 잡아먹었습니다.";
			p_system_message(send_system_message[0]);

			// [ 하베스터 맵에서 사라지기 ]
			havs[i]->repr = ' ';

			// [ 임시로 샌드웜 위치 바꾸기 ]
			POSITION new_dest = { 2, 4 };
			sw1_obj.dest = new_dest;

		}
	}
	
	// [ 적군 하베스터와 만났을 때 ]
	for (int i = 0; i < e_hav_count; i++) {
		if (map[1][sw1_obj.pos.row][sw1_obj.pos.column] == 'H' && check_friend[sw1_obj.pos.row][sw1_obj.pos.column] == 2) {
			// [ 시스템 메시지 추가 ]
			send_system_message[0] = "샌드웜이 적군 하베스터를 잡아먹었습니다.";
			p_system_message(send_system_message[0]);

			// [ 하베스터 맵에서 사라지기 ]
			e_havs[i]->repr = ' ';
			resource.population -= 5;
			// [ 임시로 샌드웜 위치 바꾸기 ]
			POSITION new_dest = { 2, 4 };
			sw1_obj.dest = new_dest;
		}
	}
	

	// [ 투사와 만났을 때 ]
	for (int i = 0; i < fighter_count; i++) {
		if (map[1][sw1_obj.pos.row][sw1_obj.pos.column] == 'F' && \
			frem_fight_fact_check[sw1_obj.pos.row][sw1_obj.pos.column] == 3) {
			// [ 시스템 메시지 추가 ]
			send_system_message[0] = "샌드웜이 투사를 잡아먹었습니다.";
			p_system_message(send_system_message[0]);

			// [ 투사 맵에서 사라지기 ]
			fighter[i]->repr = ' ';
			
			// [ 임시로 샌드웜 위치 바꾸기 ]
			POSITION new_dest = { 2, 4 };
			sw1_obj.dest = new_dest;
		}
	}
	

	// [ 보병 만났을 때 ]
	for (int i = 0; i < sold_count; i++) {
		if (map[1][sw1_obj.pos.row][sw1_obj.pos.column] == 'S' && \
			shle_sold_check[sw1_obj.pos.row][sw1_obj.pos.column] == 2) {
			// [ 시스템 메시지 추가 ]
			send_system_message[0] = "샌드웜이 보병을 잡아먹었습니다.";
			p_system_message(send_system_message[0]);

			// [ 보병 맵에서 사라지기 ]
			sold[i]->repr = ' ';
			resource.population -= 1;

			// [ 임시로 샌드웜 위치 바꾸기 ]
			POSITION new_dest = { 2, 4 };
			sw1_obj.dest = new_dest;
		}
	}
	

	map[1][sw1_obj.pos.row][sw1_obj.pos.column] = sw1_obj.repr;
	sw1_obj.next_move_time = sys_clock + sw1_obj.move_period;
}


// [ 샌드웜 (2) ]
POSITION sw2_next_pos(void) {
	// [ 현재 위치와 목적지를 비교해서 이동 방향 결정 ]
	POSITION diff = psub(sw2_obj.dest, sw2_obj.pos);
	DIRECTION dir;

	// [ 가장 가까운 유닛을 찾아서 그 유닛의 행렬값을 new_dest에 지정 ]
	double check_close = 61.0;
	int move_i = 12, move_j = 55;
	for (int i = 1; i < MAP_HEIGHT - 1; i++) {
		for (int j = 1; j < MAP_WIDTH - 1; j++) {
			if (map[1][i][j] == 'H'  || map[1][i][j] == 'S' || (map[1][i][j] == 'F' && 1 /* 1은 나중에 수정 프레멘 투사 비교 해야됨 */)) {
				if (sqrt(pow(i - sw2_obj.pos.row, 2) + pow(j - sw2_obj.pos.column, 2)) < check_close) {
					check_close = sqrt(pow(i - sw2_obj.pos.row, 2) + pow(j - sw2_obj.pos.column, 2));
					move_i = i, move_j = j;
				}
			}
		}
	}
	POSITION new_dest = { move_i, move_j };
	sw2_obj.dest = new_dest;

	// [ 가로축, 세로축 거리를 비교해서 더 먼 쪽 축으로 이동 ]
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// [ 샌드웜 이동 ]
	POSITION next_pos = pmove(sw2_obj.pos, dir);

	// [ 건물, 장애물, 유닛 만났을 때 피해가기 ]
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[0][next_pos.row][next_pos.column] == 'R' || map[0][next_pos.row][next_pos.column] == '1' || map[0][next_pos.row][next_pos.column] == '2' || \
		map[0][next_pos.row][next_pos.column] == '3' || map[0][next_pos.row][next_pos.column] == '4' || map[0][next_pos.row][next_pos.column] == '5' || \
		map[0][next_pos.row][next_pos.column] == '6' || map[0][next_pos.row][next_pos.column] == '7' || map[0][next_pos.row][next_pos.column] == '8' || \
		map[0][next_pos.row][next_pos.column] == '9' || map[0][next_pos.row][next_pos.column] == 'B' || map[0][next_pos.row][next_pos.column] == 'P' || \
		map[0][next_pos.row][next_pos.column] == 'S' || map[0][next_pos.row][next_pos.column] == 'G' || map[0][next_pos.row][next_pos.column] == 'D' || \
		map[0][next_pos.row][next_pos.column] == 'A' || map[0][next_pos.row][next_pos.column] == 'S' || map[0][next_pos.row][next_pos.column] == 'F') {

		// [ 건물, 장애물, 유닛이 위, 아래에 있을 때 ]
		if (sw2_obj.pos.row + 1 == next_pos.row || sw2_obj.pos.row - 1 == next_pos.row) {
			// [ 오른쪽으로 갈 때가 빠른지 왼쪽으로 갈 때가 빠른지 비교 ]
			double move_left = sqrt(pow(sw2_obj.pos.row - new_dest.row, 2) + pow((sw2_obj.pos.column - 1) - new_dest.column, 2));
			double move_right = sqrt(pow(sw2_obj.pos.row - new_dest.row, 2) + pow((sw2_obj.pos.column + 1) - new_dest.column, 2));
			// [ 오른쪽으로 가기 ]
			if (move_left < move_right) {
				// < 오른쪽으로 갔을 때 맵을 벗어나게 되는지를 체크 >
				if (sw2_obj.pos.column + 1 != MAP_WIDTH - 1) { // 맵을 벗어나지 않으면 오른쪽
					next_pos.row += 1;
					next_pos.column = sw2_obj.pos.column + 1;
				}
				else { // 맵을 벗어나면 왼쪽
					next_pos.row -= 1;
					next_pos.column = sw2_obj.pos.column - 1;
				}
			}
			// [ 왼쪽으로 가기 ]
			else {
				// < 왼쪽으로 갔을 때 맵을 벗어나게 되는지를 체크 >
				if (sw2_obj.pos.column - 1 != 0) { // 맵을 벗어나지 않으면 왼쪽
					next_pos.row -= 1;
					next_pos.column = sw2_obj.pos.column - 1;
				}
				else { // 맵을 벗어나면 오른쪽
					next_pos.row += 1;
					next_pos.column = sw2_obj.pos.column + 1;
				}
			}
		}
		// [ 건물, 장애물, 유닛이 오른쪽, 왼쪽에 있을 때 ]
		else if (sw2_obj.pos.column + 1 == next_pos.column || sw2_obj.pos.column - 1 == next_pos.column) {
			// [ 위로 갈 때가 빠른지 아래로 갈 때가 빠른지 비교 ]
			double move_up = sqrt(pow((sw2_obj.pos.row - 1) - new_dest.row, 2) + pow(sw2_obj.pos.column - new_dest.column, 2));
			double move_down = sqrt(pow((sw2_obj.pos.row + 1) - new_dest.row, 2) + pow(sw2_obj.pos.column - new_dest.column, 2));
			// [ 위로 가기 ]
			if (move_up < move_down) {
				// < 위로 갔을 때 맵을 벗어나는지 확인 >
				if (sw2_obj.pos.row + 1 != 0) { // 맵을 벗어나지 않으면 위로
					next_pos.column += 1;
					next_pos.row = sw2_obj.pos.row + 1;
				}
				else { // 맵을 벗어나면 아래로
					next_pos.column -= 1;
					next_pos.row = sw2_obj.pos.row - 1;
				}
			}
			// [ 아래로 가기 ]
			else {
				if (sw2_obj.pos.row - 1 != MAP_HEIGHT - 1) { // 맵을 벗어나면 아래로
					next_pos.column -= 1;
					next_pos.row = sw2_obj.pos.row - 1;
				}
				else { // 맵을 벗어나면 위로
					next_pos.column += 1;
					next_pos.row = sw2_obj.pos.row + 1;
				}
			}
		}
		return next_pos;
	}
	return next_pos;
}

// [ 샌드웜 (2) 움직임 함수 ]
void sw2_move(void) {
	// [ 움직이는 주기 ]
	if (sys_clock <= sw2_obj.next_move_time) {
		return;
	}

	// [ 샌드웜(2) layer1 (map[1])에 저장 ]
	map[1][sw2_obj.pos.row][sw2_obj.pos.column] = -1;

	// [ 10/300 확률로 배설 --> 스파이스 매장지 생성 (매장량 1 ~ 9 랜덤) ]
	int r = rand() % 299;
	if (r < 9) {
		// [ 시스템 메시지 추가 ]
		send_system_message[0] = "샌드웜이 스파이스 매장지를 생성했습니다.";
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

	// [ 샌드웜 잡아먹기 ]
	// [ 아군 하베스터와 만났을 때 ]
	for (int i = 0; i < hav_count; i++) {
		if (map[1][sw2_obj.pos.row][sw2_obj.pos.column] == 'H' && check_friend[sw2_obj.pos.row][sw2_obj.pos.column] == 1) {
			// [ 시스템 메시지 추가 ]
			send_system_message[0] = "샌드웜이 아군 하베스터를 잡아먹었습니다.";
			p_system_message(send_system_message[0]);

			// [ 하베스터 맵에서 사라지기 ]
			havs[i]->repr = ' ';
			map[1][sw2_obj.pos.row][sw2_obj.pos.column] = ' ';
			resource.population -= 5;

			// [ 임시로 샌드웜 위치 바꾸기 ]
			POSITION new_dest = { 12, 55 };
			sw2_obj.dest = new_dest;

		}
	}

	// [ 적군 하베스터와 만났을 때 ]
	for (int i = 0; i < e_hav_count; i++) {
		if (map[1][sw2_obj.pos.row][sw2_obj.pos.column] == 'H' && check_friend[sw2_obj.pos.row][sw2_obj.pos.column] == 2) {
			// [ 시스템 메시지 추가 ]
			send_system_message[0] = "샌드웜이 적군 하베스터를 잡아먹었습니다.";
			p_system_message(send_system_message[0]);

			// [ 하베스터 맵에서 사라지기 ]
			e_havs[i]->repr = ' ';
			map[1][sw2_obj.pos.row][sw2_obj.pos.column] = ' ';

			// [ 임시로 샌드웜 위치 바꾸기 ]
			POSITION new_dest = { 12, 55 };
			sw2_obj.dest = new_dest;
		}
	}


	// [ 투사와 만났을 때 ]
	for (int i = 0; i < fighter_count; i++) {
		if (map[1][sw2_obj.pos.row][sw2_obj.pos.column] == 'F' && \
			frem_fight_fact_check[sw2_obj.pos.row][sw2_obj.pos.column] == 3) {
			// [ 시스템 메시지 추가 ]
			send_system_message[0] = "샌드웜이 투사를 잡아먹었습니다.";
			p_system_message(send_system_message[0]);

			// [ 투사 맵에서 사라지기 ]
			fighter[i]->repr = ' ';
			map[1][sw2_obj.pos.row][sw2_obj.pos.column] = ' ';

			// [ 임시로 샌드웜 위치 바꾸기 ]
			POSITION new_dest = { 12, 55 };
			sw2_obj.dest = new_dest;
		}
	}


	// [ 보병 만났을 때 ]
	for (int i = 0; i < sold_count; i++) {
		if (map[1][sw2_obj.pos.row][sw2_obj.pos.column] == 'S' && \
			shle_sold_check[sw2_obj.pos.row][sw2_obj.pos.column] == 2) {
			// [ 시스템 메시지 추가 ]
			send_system_message[0] = "샌드웜이 보병을 잡아먹었습니다.";
			p_system_message(send_system_message[0]);

			// [ 보병 맵에서 사라지기 ]
			sold[i]->repr = ' ';
			map[1][sw2_obj.pos.row][sw2_obj.pos.column] = ' ';
			resource.population -= 1;

			// [ 임시로 샌드웜 위치 바꾸기 ]
			POSITION new_dest = { 12, 55 };
			sw2_obj.dest = new_dest;
		}
	}

	map[1][sw2_obj.pos.row][sw2_obj.pos.column] = sw2_obj.repr;
	sw2_obj.next_move_time = sys_clock + sw2_obj.move_period;
}


// [ 모래 폭풍 (X) ]
POSITION sand_wind_next_pos(void) {
	// 모래 폭풍 생성 위치
	int np_c = rand() % 14 + 3; // 높이 3 ~ 14 사이의 무작위 수
	int np_r = rand() % 55 + 3; // 너비 3 ~ 55 사이의 무작위 수

	// 모래 폭풍 목적지 위치
	int nd_c = rand() % 14 + 3; // 높이 3 ~ 14 사이의 무작위 수
	int nd_r = rand() % 53 + 3; // 너비 3 ~ 55 사이의 무작위 수

	// 모래 폭풍 생성 위치 지정
	POSITION random_pos1 = { np_c - 1, np_r - 1 };
	POSITION random_pos2 = { np_c, np_r - 1 };
	POSITION random_pos3 = { np_c - 1, np_r };
	POSITION random_pos4 = { np_c, np_r };
	sand_wind.pos1 = random_pos1;
	sand_wind.pos2 = random_pos2;
	sand_wind.pos3 = random_pos3;
	sand_wind.pos4 = random_pos4;
	
	// 모래 폭풍 목적지 위치 지정
	POSITION random_dest1 = { nd_c - 1, nd_r - 1 };
	POSITION random_dest2 = { nd_c, nd_r - 1 };
	POSITION random_dest3 = { nd_c - 1, nd_r };
	POSITION random_dest4 = { nd_c, nd_r };
	sand_wind.dest1 = random_dest1;
	sand_wind.dest2 = random_dest2;
	sand_wind.dest3 = random_dest3;
	sand_wind.dest4 = random_dest4;

	// 현재 위치와 목적지를 비교해서 이동 방향 결정	
	POSITION diff = psub(sand_wind.dest4, sand_wind.pos4);
	DIRECTION dir;

	// 가로축, 세로축 거리를 비교해서 더 먼 쪽 축으로 이동
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// 모래 폭풍 이동
	POSITION next_pos1 = pmove(sand_wind.pos1, dir);
	POSITION next_pos2 = pmove(sand_wind.pos2, dir);
	POSITION next_pos3 = pmove(sand_wind.pos3, dir);
	POSITION next_pos4 = pmove(sand_wind.pos4, dir);

	// 모래 폭풍이 'T' 일때만 이동 (목적지에 도착하면 ' '으로 목적지에 계속 존재할거기 때문)
	if (1 <= next_pos4.row && next_pos4.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos4.column && next_pos4.column <= MAP_WIDTH - 2 && sand_wind.repr == 'Z') { // pos4에 따라 움직이기 때문에 pos4만 검사.
		
		return next_pos1, next_pos2, next_pos3, next_pos4;
	}
	

	return next_pos1, next_pos2, next_pos3, next_pos4;

}
void sand_wind_move(void) {
	// 움직이는 주기
	if (sys_clock <= sand_wind.next_move_time) {
		return;
	}
	// [ 모래 폭풍 layer2 (map[2])에 저장 ]
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

	// [ 모래 폭풍에 닿은 건물, 유닛 행동 ]
	// < 아군 하베스터 > (미완성)
	if ((map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'H' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'H' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'H' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'H') && \
		check_friend[sand_wind.pos4.row][sand_wind.pos4.column] == 1) {
		
		map[1][sand_wind.pos1.row][sand_wind.pos1.column] = ' ';
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] = ' ';
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] = ' ';
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] = ' ';
		
		// 시스템 메시지
		send_system_message[0] = "모래 폭풍에 의해 아군 하베스터가 사망했습니다.";
		p_system_message(send_system_message[0]);

	}
	// < 적군 하베스터 > (미완성)
	if ((map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'H' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'H' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'H' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'H') && \
		check_friend[sand_wind.pos4.row][sand_wind.pos4.column] == 2) {

		map[1][sand_wind.pos1.row][sand_wind.pos1.column] = ' ';
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] = ' ';
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] = ' ';
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] = ' ';
		
		// 시스템 메시지
		send_system_message[0] = "모래 폭풍에 의해 적군 하베스터가 사망했습니다.";
		p_system_message(send_system_message[0]);

	}
	// < 프레멘 > (아직 구현 X)
	else if (map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'F' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'F' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'F' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'F') {



		// 시스템 메시지
		send_system_message[0] = "모래 폭풍에 의해 프레멘이 사망했습니다.";
		p_system_message(send_system_message[0]);

	}
	// < 보병 > (아직 구현 X)
	else if (map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'S' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'S' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'S' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'S') {



		// 시스템 메시지
		send_system_message[0] = "모래 폭풍에 의해 보병이 사망했습니다.";
		p_system_message(send_system_message[0]);

	}
	// < 투사 > (아직 구현 X)
	else if (map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'F' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'F' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'F' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'F') {


		// 시스템 메시지
		send_system_message[0] = "모래 폭풍에 의해 투사가 사망했습니다.";
		p_system_message(send_system_message[0]);

	}
	// < 중전차 > (아직 구현 X)
	else if (map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'T' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'T' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'T' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'T') {

		// 시스템 메시지
		send_system_message[0] = "모래 폭풍에 의해 중전차가 부서졌습니다.";
		p_system_message(send_system_message[0]);

	}
	// < 숙소 > (아직 구현 X)
	else if (map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'D' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'D' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'D' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'D') {


		// 시스템 메시지
		send_system_message[0] = "모래 폭풍에 의해 숙소가 박살났습니다.";
		p_system_message(send_system_message[0]);

	}
	// < 창고 > (아직 구현 X)
	else if (map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'G' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'G' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'G' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'G') {


		// 시스템 메시지
		send_system_message[0] = "모래 폭풍에 의해 창고가 박살났습니다.";
		p_system_message(send_system_message[0]);

	}
	// < 병영 > (아직 구현 X)
	else if (map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'B' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'B' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'B' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'B') {


		// 시스템 메시지
		send_system_message[0] = "모래 폭풍에 의해 병영이 박살났습니다.";
		p_system_message(send_system_message[0]);

	}
	// < 은신처 > (아직 구현 X)
	else if (map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'S' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'S' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'S' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'S') {


		// 시스템 메시지
		send_system_message[0] = "모래 폭풍에 의해 은신처가 박살났습니다.";
		p_system_message(send_system_message[0]);

	}
	// < 투기장 > (아직 구현 X)
	else if (map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'A' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'A' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'A' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'A') {


		// 시스템 메시지
		send_system_message[0] = "모래 폭풍에 의해 투기장이 박살났습니다.";
		p_system_message(send_system_message[0]);

	}
	// < 공장 > (아직 구현 X)
	else if (map[1][sand_wind.pos1.row][sand_wind.pos1.column] == 'F' || \
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] == 'F' || \
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] == 'F' || \
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] == 'F') {
		


		// 시스템 메시지
		send_system_message[0] = "모래 폭풍에 의해 공장이 박살났습니다.";
		p_system_message(send_system_message[0]);

	}


	// [ 모래 폭풍이 목적지에 도착했을 때 (모래폭풍을 화면에서 없애야함) ]
	if (sand_wind.pos4.row == sand_wind.dest4.row && sand_wind.pos4.column == sand_wind.dest4.column) {
		sand_wind.repr = ' ';
		map[1][sand_wind.pos1.row][sand_wind.pos1.column] = sand_wind.repr;
		map[1][sand_wind.pos2.row][sand_wind.pos2.column] = sand_wind.repr;
		map[1][sand_wind.pos3.row][sand_wind.pos3.column] = sand_wind.repr;
		map[1][sand_wind.pos4.row][sand_wind.pos4.column] = sand_wind.repr;
		
		// 시스템 메시지
		send_system_message[0] = "모래 폭풍이 사라졌습니다..";
		p_system_message(send_system_message[0]);
	}

	sand_wind.next_move_time = sys_clock + sand_wind.move_period;
}


// [ 하베스터 선택 및 스파이스 선택 ]
void handle_harvester_input(CURSOR cursor, OBJECT_SAMPLE havs[MAX_HAV], int hav_count, OBJECT_BUILDING* SPICE) {
	POSITION curr = cursor.current;

	// [ 스페이스바로 하베스터 선택 ]
	if (space_key_press && map[1][curr.row][curr.column] == 'H') {
		for (int i = 0; i < hav_count; i++) {
			if (havs[i].pos.row == curr.row && havs[i].pos.column == curr.column) {
				selected_harvester = i; // 하베스터 선택
				p_system_message("하베스터 선택 완료.");
				space_key_press = false;
				space_and_h_key = true;
				space_and_m_key = true;
				break;
			}
		}
	}

	// [ H키로 스파이스 선택 및 목적지 설정 ]
	if (space_and_h_key && h_key_press && selected_harvester != -1) {
		if (map[0][curr.row][curr.column] >= '1' && map[0][curr.row][curr.column] <= '9') {
			havs[selected_harvester].dest = harvest_set_dest(curr, &havs[selected_harvester], SPICE);
			p_system_message("스파이스 위치를 목적지로 설정.");
			selected_harvester = -1;
			h_key_press = false; // 처리 완료
			space_and_h_key = false;
			space_and_m_key = false;
		}
		else {
			h_key_press = false;
			p_system_message("스파이스가 선택되지 않았습니다. 다시 h키를 눌러주세요");
		}
	}

	// [ M키로 이동 목적지 설정 ]
	if (space_and_m_key && m_key_press && selected_harvester != -1) {
		// 빈 공간(사막)으로만 이동 가능
		if (map[0][curr.row][curr.column] == ' ') {
			havs[selected_harvester].dest.row = curr.row;
			havs[selected_harvester].dest.column = curr.column;
			hav_gather_spice[selected_harvester] = -1; // 이동 상태 표시
			p_system_message("커서 위치를 목적지로 설정.");
			selected_harvester = -1;
			m_key_press = false;
			space_and_h_key = false;
			space_and_m_key = false;
		}
		else {
			m_key_press = false;
			p_system_message("하베스터는 사막지형에서만을 목적지로 설정할 수 있습니다.");
		}
	}
}

// [ 하베스터 목적지 정하기 ]
POSITION harvest_set_dest(POSITION curr, OBJECT_SAMPLE* hav, OBJECT_BUILDING* SPICE) {
	POSITION new_dest = hav->dest;

	// [ 커서가 스파이스 위에 있을 때 ]
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

// [ 하베스터 이동 ]
POSITION harvest_next_move(OBJECT_SAMPLE* hav, POSITION new_dest) {
	POSITION diff = psub(new_dest, hav->pos);
	DIRECTION dir;

	// 가로축과 세로축 중 더 먼 쪽으로 이동 방향 결정
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// 기본 이동
	POSITION next_pos = pmove(hav->pos, dir);

	// 맵 내 범위 확인
	if (next_pos.row < 1 || next_pos.row >= MAP_HEIGHT - 1 ||
		next_pos.column < 1 || next_pos.column >= MAP_WIDTH - 1) {
		return hav->pos; // 이동할 수 없으면 현재 위치 유지
	}

	// 이동하려는 위치가 장애물인지 확인
	char target_tile = map[0][next_pos.row][next_pos.column];
	if (target_tile != ' ' && (target_tile < '1' || target_tile > '9')) {
		// 장애물을 회피하기 위해 대체 경로 계산
		double best_distance = DBL_MAX;
		POSITION best_pos = hav->pos;

		// 상하좌우로 가능한 대체 경로 탐색
		DIRECTION alternate_dirs[] = { d_up, d_down, d_left, d_right };
		for (int i = 0; i < 4; i++) {
			POSITION alt_pos = pmove(hav->pos, alternate_dirs[i]);

			// 대체 경로가 맵 내에 있는지 확인
			if (alt_pos.row < 1 || alt_pos.row >= MAP_HEIGHT - 1 ||
				alt_pos.column < 1 || alt_pos.column >= MAP_WIDTH - 1) {
				continue; // 맵 밖이면 스킵
			}

			// 대체 경로가 장애물이 아닌지 확인
			char alt_tile = map[0][alt_pos.row][alt_pos.column];
			if (alt_tile == ' ' || (alt_tile >= '1' && alt_tile <= '9')) {
				double distance = sqrt(pow(new_dest.row - alt_pos.row, 2) + pow(new_dest.column - alt_pos.column, 2));
				if (distance < best_distance) {
					best_distance = distance;
					best_pos = alt_pos;
				}
			}
		}

		return best_pos; // 가장 가까운 대체 경로 반환
	}

	return next_pos; // 기본 이동 가능하면 반환
}

// [ 스파이스 수확 처리 함수 ]
int harvest_gather_spice(OBJECT_BUILDING* spice, OBJECT_SAMPLE* hav, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	if (sys_clock <= hav->gather_time) {
		return 0; // 아직 수확 시간이 되지 않았음
	}

	int spice_int = spice->repr - '0'; // 스파이스 값을 정수로 변환
	int harvest_amount = 3; // 하베스터가 한 번에 수확하는 양

	// 수확 가능한 양만큼 감소
	int collected = (spice_int >= harvest_amount) ? harvest_amount : spice_int;
	spice_int -= collected; // 스파이스 값 감소
	char mes[100] = { 0 };
	sprintf_s(mes, sizeof(mes), "스파이스 매장지에서 자원을 %d개 캤습니다.", collected);
	p_system_message(mes);

	// 스파이스 값이 0이면 제거, 아니면 갱신
	if (spice_int <= 0) {
		spice->repr = ' '; // 스파이스 제거
		map[0][spice->pos1.row][spice->pos1.column] = ' '; // 맵에서 제거
		map[1][hav->pos.row][hav->pos.column] = hav->repr; // 새 위치 표시
		char* mes = "스파이스 매장지가 사라집니다.";
		p_system_message(mes);
	}
	else {
		spice->repr = spice_int + '0'; // 남은 스파이스 출력
		map[0][spice->pos1.row][spice->pos1.column] = spice->repr;
		char mes_buf[100] = { 0 };
		sprintf_s(mes_buf, sizeof(mes_buf), "스파이스 매장지에 스파이스가 %c개 남았습니다.", spice->repr);
		p_system_message(mes_buf);
	}

	hav->gather_time = sys_clock + hav->gather_next_time; // 다음 수확 시간 설정
	return collected; // 수확한 자원 반환
}

// [ 하베스터 움직임 메인 함수 ]
void harvest_move_all(OBJECT_SAMPLE havs[MAX_HAV], int hav_count, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], OBJECT_BUILDING* SPICE) {
	// [ 모든 하베스터의 다음 위치 계산 ]
	handle_harvester_input(cursor, havs, hav_count, SPICE);

	// [ 하베스터 이동 주기 ]
	for (int i = 0; i < hav_count; i++) {
		if (sys_clock <= havs[i].move_period) {
				return; 
		}
	}
	
	// [ 하베스터 이동 ]
	for (int i = 0; i < hav_count; i++) {
		// [ 하베스터 목적지에 도달한 경우 ]
		if (havs[i].pos.row == havs[i].dest.row && havs[i].pos.column == havs[i].dest.column) {
			if (hav_gather_spice[i] == 0) { // 스파이스 수확
				for (int j = 0; j < spi_count; j++) {
					if (SPICE[j].pos1.row == havs[i].pos.row && SPICE[j].pos1.column == havs[i].pos.column) {
						// 다른 하베스터가 작업 중인지 확인
						if (spice_in_progress[j] == -1) {
							spice_in_progress[j] = i; // 현재 하베스터 작업 시작
							int spice = harvest_gather_spice(&SPICE[j], &havs[i], map);
							*spice_plus = spice;
							hav_gather_spice[i] = 1; // 상태 변경
							havs[i].dest = (POSITION){ 14, 1 }; // 본진으로 이동
						}
						else {
							// 작업 중인 하베스터가 있으면 대기 메시지 출력
							char mes[100] = { 0 };
							sprintf_s(mes, sizeof(mes), "하베스터 %d가 작업 중입니다. 대기 중...", spice_in_progress[j]);
							p_system_message(mes);
						}
						break;
					}
				}
			}
			else if (hav_gather_spice[i] == 1) { // 본진으로 자원 전달
				resource.spice += *spice_plus;
				char mes[100] = { 0 };
				sprintf_s(mes, sizeof(mes), "스파이스 자원이 %d로 증가하였습니다.", resource.spice);
				p_system_message(mes);

				// 작업 상태 초기화
				for (int j = 0; j < spi_count; j++) {
					if (SPICE[j].pos1.row == pre_hav_dest[i].row && SPICE[j].pos1.column == pre_hav_dest[i].column) {
						spice_in_progress[j] = -1; // 작업 해제
						break;
					}
				}

				// 원래 스파이스 위치로 복귀
				if (map[0][pre_hav_dest[i].row][pre_hav_dest[i].column] != ' ') {
					havs[i].dest = pre_hav_dest[i];
				}

				hav_gather_spice[i] = 0; // 상태 초기화
			}
			else if (hav_gather_spice[i] == -1) { // 단순 이동 완료
				hav_gather_spice[i] = 0; // 상태 초기화
				p_system_message("하베스터 이동 완료.");
			}
			continue;
		}

		// [ 하베스터 이동 ]
		if (map[0][havs[i].pos.row][havs[i].pos.column] >= '1' && map[0][havs[i].pos.row][havs[i].pos.column] <= '9') {
			// 스파이스 위치는 그대로 유지
		}
		else {
			map[1][havs[i].pos.row][havs[i].pos.column] = ' '; // 이전 위치 제거
		}

		check_friend[havs[i].pos.row][havs[i].pos.column] = 0;

		// [ 장애물 피하기 로직 추가 ]
		POSITION next_pos = harvest_next_move(&havs[i], havs[i].dest);

		// 장애물이 있는 경우 피하기
		if (map[0][next_pos.row][next_pos.column] != ' ' &&
			(map[0][next_pos.row][next_pos.column] < '1' || map[0][next_pos.row][next_pos.column] > '9')) {

			// 위/아래로 우회할지 좌/우로 우회할지 계산
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

		// 최종 이동 처리
		havs[i].pos = next_pos;
		check_friend[havs[i].pos.row][havs[i].pos.column] = 1;

		if (map[0][havs[i].pos.row][havs[i].pos.column] >= '1' && map[0][havs[i].pos.row][havs[i].pos.column] <= '9') {
			// 스파이스 위치는 그대로 유지
		}
		else {
			map[1][havs[i].pos.row][havs[i].pos.column] = havs[i].repr; // 새 위치 표시
		}
	}

	// [ 이동 주기 설정 ]
	for (int i = 0; i < hav_count; i++) {
		havs[i].move_period = sys_clock + havs[i].next_move_time;
	} 
}



// [ 보병 선택 --> 이동, 순찰, 건물 선택 ]
void select_sold_act(void) {
	POSITION curr = cursor.current;

	// [ 보병 선택 ]
	if (space_key_press && map[1][curr.row][curr.column] == 'S') {
		for (int i = 0; i < sold_count; i++) {
			if (sold[i]->pos.row == curr.row && sold[i]->pos.column == curr.column) {
				selected_sold = i;
				p_system_message("보병 선택 완료");
				space_key_press = false;
				space_and_m_key = true; // 이동
				p_p_key_press = true; // 순찰
				break;
			}
		}
	}

	// [ 보병 이동(M) 목적지 정하기 ]
	if (space_and_m_key && m_key_press && selected_sold != -1) {

		// [ 순찰 비활성화 ]
		sold[selected_sold]->is_patrolling = false;

		// [ 사막 지형을 이동 목적지로 정하기 ]
		if (map[0][curr.row][curr.column] == ' ') {
			sold[selected_sold]->dest.row = curr.row;
			sold[selected_sold]->dest.column = curr.column;
			p_system_message("커서 위치(사막지형)를 목적지로 설정");
			selected_sold = -1;
			m_key_press = false;
			p_p_key_press = false;
			space_and_m_key = false;
		}
		// [ 적군 본진 이동 목적지로 정하기 ]
		else if (map[0][curr.row][curr.column] == 'B' && base_barr_check[curr.row][curr.column] == 1 && check_friend[curr.row][curr.column] == 2) {
			sold[selected_sold]->dest.row = e_base.pos3.row + 1;
			sold[selected_sold]->dest.column = e_base.pos3.column;
			sold[selected_sold]->e_base_attack = true;
			p_system_message("적군 본진 위치를 목적지로 설정.");
			selected_sold = -1;
			m_key_press = false;
			p_p_key_press = false;
			space_and_m_key = false;
		}
		// [ 적군 숙소 이동 목적지로 정하기 ]
		else if (map[0][curr.row][curr.column] == 'D' && check_friend[curr.row][curr.column] == 2) {
			for (int i = 0; i < e_dor_count; i++) {
				if ((E_DOR[i]->pos1.row == curr.row && E_DOR[i]->pos1.column == curr.column) || \
					(E_DOR[i]->pos2.row == curr.row && E_DOR[i]->pos2.column == curr.column) || \
					(E_DOR[i]->pos3.row == curr.row && E_DOR[i]->pos3.column == curr.column) || \
					(E_DOR[i]->pos4.row == curr.row && E_DOR[i]->pos4.column == curr.column)) {
					sold[selected_sold]->dest.row = E_DOR[i]->pos3.row + 1;
					sold[selected_sold]->dest.column = E_DOR[i]->pos3.column;
					sold[selected_sold]->e_dor_attack = true;
					p_system_message("적군 숙소 위치를 목적지로 설정.");
					selected_sold = -1;
					m_key_press = false;
					p_p_key_press = false;
					space_and_m_key = false;
					break;
				}
			}
		}
		// [ 적군 창고 이동 목적지로 정하기 ]
		else if (map[0][curr.row][curr.column] == 'G' && check_friend[curr.row][curr.column] == 2) {
			for (int i = 0; i < e_gar_count; i++) {
				if ((E_GAR[i]->pos1.row == curr.row && E_GAR[i]->pos1.column == curr.column) || \
					(E_GAR[i]->pos2.row == curr.row && E_GAR[i]->pos2.column == curr.column) || \
					(E_GAR[i]->pos3.row == curr.row && E_GAR[i]->pos3.column == curr.column) || \
					(E_GAR[i]->pos4.row == curr.row && E_GAR[i]->pos4.column == curr.column)) {
					sold[selected_sold]->dest.row = E_GAR[i]->pos3.row + 1;
					sold[selected_sold]->dest.column = E_GAR[i]->pos3.column;
					sold[selected_sold]->e_gar_attack = true;
					p_system_message("적군 창고 위치를 목적지로 설정.");
					selected_sold = -1;
					m_key_press = false;
					p_p_key_press = false;
					space_and_m_key = false;
					break;
				}
			}
		}
		// [ 적군 투기장 이동 목적지로 정하기 ]
		else if (map[0][curr.row][curr.column] == 'A' && check_friend[curr.row][curr.column] == 2) {
			for (int i = 0; i < are_count; i++) {
				if ((ARE[i]->pos1.row == curr.row && ARE[i]->pos1.column == curr.column) || \
					(ARE[i]->pos2.row == curr.row && ARE[i]->pos2.column == curr.column) || \
					(ARE[i]->pos3.row == curr.row && ARE[i]->pos3.column == curr.column) || \
					(ARE[i]->pos4.row == curr.row && ARE[i]->pos4.column == curr.column)) {
					sold[selected_sold]->dest.row = ARE[i]->pos3.row + 1;
					sold[selected_sold]->dest.column = ARE[i]->pos3.column;
					sold[selected_sold]->e_arena_attack = true;
					p_system_message("적군 투기장 위치를 목적지로 설정.");
					selected_sold = -1;
					m_key_press = false;
					p_p_key_press = false;
					space_and_m_key = false;
					break;
				}
			}
		}

		// [ 적군 공장 이동 목적지로 정하기 ]
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
					p_system_message("적군 공장 위치를 목적지로 설정.");
					selected_sold = -1;
					m_key_press = false;
					p_p_key_press = false;
					space_and_m_key = false;
					break;
				}
			}
		}
	}

	// [ 보병 순찰(P) 목적지 정하기 ]
	if (p_p_key_press && sold_p_key_press && selected_sold != -1) {
		if (map[0][curr.row][curr.column] == ' ') {
			sold[selected_sold]->patrol_point.row = sold[selected_sold]->pos.row;
			sold[selected_sold]->patrol_point.column = sold[selected_sold]->pos.column;
			sold[selected_sold]->dest.row = curr.row;
			sold[selected_sold]->dest.column = curr.column;
			sold[selected_sold]->is_patrolling = true; // 순찰 모드 활성화
			p_system_message("순찰 위치 설정 완료. 순찰 시작.");
			selected_sold = -1;
			p_key_press = false;
			p_p_key_press = false;
			sold_p_key_press = false;
		}
		else {
			p_system_message("순찰 위치를 설정할 수 없는 지형입니다.");
			p_p_key_press = false;
		}
	}
}

// [ 보병 건물 공격 ]
void sold_attack_building(void) {
	for (int i = 0; i < sold_count; i++) {
		// [ 목적지에 도착했을 때 ]
		if (sold[i]->pos.row == sold[i]->dest.row && sold[i]->pos.column == sold[i]->dest.column) {
			// [ 적군 본진을 공격하라고 명령을 받았을 때 ]
			if (sold[i]->e_base_attack) {
				if (sys_clock <= sold[i]->attack_period) {
					return;
				}

				if (e_base.hp <= 0) {
					p_system_message("적군 본진이 보병에 의해 파괴되었습니다.");
					e_base.hp = 0;
					e_base.repr = ' ';
					map[0][e_base.pos1.row][e_base.pos1.column] = ' ';
					map[0][e_base.pos2.row][e_base.pos2.column] = ' ';
					map[0][e_base.pos3.row][e_base.pos3.column] = ' ';
					map[0][e_base.pos4.row][e_base.pos4.column] = ' ';
					sold[i]->e_base_attack = false;

					// [ 게임 종료 ]
					outro();
				}
				else {
					e_base.hp -= sold[i]->dps;
					p_system_message("보병이 본진을 공격했습니다.");
					char mes[100];
					sprintf_s(mes, sizeof(mes), "적군 본진의 내구도가 %d로 감소했습니다.", e_base.hp);
					p_system_message(mes);
				}
				sold[i]->attack_period = sys_clock + sold[i]->next_attack_time;
			}
			// [ 적군 숙소를 공격하라고 명령을 받았을 때 ]
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
							p_system_message("적군 숙소가 보병에 의해 파괴되었습니다.");
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
							p_system_message("보병이 숙소를 공격했습니다.");
							char mes[100];
							sprintf_s(mes, sizeof(mes), "적군 숙소의 내구도가 %d로 감소했습니다.", E_DOR[j]->hp);
							p_system_message(mes);
						}
					}
				}
				sold[i]->attack_period = sys_clock + sold[i]->next_attack_time;
			}
			// [ 적군 창고를 공격하라고 명령을 받았을 때 ]
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
							p_system_message("적군 창고가 보병에 의해 파괴되었습니다.");
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
							p_system_message("보병이 창고를 공격했습니다.");
							char mes[100];
							sprintf_s(mes, sizeof(mes), "적군 창고의 내구도가 %d로 감소했습니다.", E_GAR[j]->hp);
							p_system_message(mes);
						}
					}
				}
				sold[i]->attack_period = sys_clock + sold[i]->next_attack_time;
			}
			// [ 적군 투기장을 공격하라고 명령을 받았을 때 ]
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
							p_system_message("적군 은신처가 보병에 의해 파괴되었습니다.");
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
							p_system_message("보병이 은신처를 공격했습니다.");
							char mes[100];
							sprintf_s(mes, sizeof(mes), "적군 은신처의 내구도가 %d로 감소했습니다.", ARE[j]->hp);
							p_system_message(mes);
						}
					}
				}
				sold[i]->attack_period = sys_clock + sold[i]->next_attack_time;
			}
			// [ 적군 공장을 공격하라고 명령을 받았을 때 ]
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
							p_system_message("적군 공장이 보병에 의해 파괴되었습니다.");
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
							p_system_message("보병이 공장을 공격했습니다.");
							char mes[100];
							sprintf_s(mes, sizeof(mes), "적군 공장의 내구도가 %d로 감소했습니다.", FAC[j]->hp);
							p_system_message(mes);
						}
					}
				}
				sold[i]->attack_period = sys_clock + sold[i]->next_attack_time;
			}
		}
	}
}

// [ 보병 유닛 공격 ]
void sold_attack_unit(void) {
	for (int i = 0; i < sold_count; i++) {
		// [ 보병 시야 선언 ]
		int sight = sold[i]->eyes;

		// [ 보병 현재 위치 ]
		int current_row = sold[i]->pos.row;
		int current_col = sold[i]->pos.column;

		// [ 보병 주변에 유닛 있는지 확인 ]
		for (int row = current_row - sight; row <= current_row + sight; row++) {
			for (int col = current_col - sight; col <= current_col + sight; col++) {
				// [ 맵 경계 확인 ]
				if (row < 0 || row >= MAP_HEIGHT || col < 0 || col >= MAP_WIDTH) continue;

				// [ 보병 자신 제외 ]
				if (row == current_row && col == current_col) continue;

				// [ 유닛 처리 함수 호출 ]
				process_enemy_unit(row, col, sold[i]);
			}
		}
	}
}

// Main - [ 적 유닛 처리 함수 ]
void process_enemy_unit(int row, int col, OBJECT_SAMPLE* soldier) {
	// [ 적군 하베스터 확인 ]
	for (int j = 0; j < e_hav_count; j++) {
		if (row == e_havs[j]->pos.row && col == e_havs[j]->pos.column && check_friend[row][col] == 2 && \
			e_havs[j]->repr == 'H') {
			p_system_message("보병이 적군 하베스터를 발견했습니다.");
			p_system_message("보병이 적군 하베스터를 목표로 삼았습니다.");
			handle_enemy_harvester(soldier, e_havs[j]);
			return; // 하베스터 처리 후 종료
		}
	}

	// [ 적군 투사 확인 ]
	for (int j = 0; j < fighter_count; j++) {
		if (row == fighter[j]->pos.row && col == fighter[j]->pos.column && frem_fight_fact_check[row][col] == 3 && \
			fighter[j]->repr == 'F') {
			p_system_message("보병이 적군 투사를 발견했습니다.");
			p_system_message("보병이 적군 투사를 목표로 삼았습니다.");
			handle_enemy_fighter(soldier, fighter[j]);
			return; // 투사 처리 후 종료
		}
	}

	// [ 적군 중전차 확인 ]
	for (int j = 0; j < tank_count; j++) {
		if (row == tank[j]->pos.row && col == tank[j]->pos.column && tank[j]->repr == 'T') {
			p_system_message("보병이 적군 중전차 발견했습니다.");
			p_system_message("보병이 적군 투사를 목표로 삼았습니다.");
			handle_enemy_tank(soldier, tank[j]);
			return; // 중전차 처리 후 종료
		}
	}
}

// 1 - [ 적 하베스터 처리 함수 ]
void handle_enemy_harvester(OBJECT_SAMPLE* soldier, OBJECT_SAMPLE* harvester) {

	// [ 적 주변의 빈 공간 확인 및 이동 ]
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

			// 이동 방향 결정
			if (next_pos.row < soldier->dest.row) next_pos.row++;
			else if (next_pos.row > soldier->dest.row) next_pos.row--;

			if (next_pos.column < soldier->dest.column) next_pos.column++;
			else if (next_pos.column > soldier->dest.column) next_pos.column--;

			// 장애물 처리
			next_pos = sold_move(soldier, next_pos);

			// 이동 적용
			soldier->pos = next_pos;

			// 목적지 도달 시 공격
			
			// [ 공격 주기 ]
			if (sys_clock <= soldier->attack_period) {
				return;
			}

			if (soldier->pos.row == soldier->dest.row && soldier->pos.column == soldier->dest.column) {
				if (harvester->hp <= 0) {
					p_system_message("적군 하베스터이 보병에 의해 사망하였습니다.");
					harvester->repr = ' ';
					map[1][harvester->pos.row][harvester->pos.column] = harvester->repr;
				}
				else {
					harvester->hp -= soldier->dps;
					char mes[100];
					sprintf_s(mes, sizeof(mes), "적군 하베스터가 보병에게 공격받아 hp가 %d로 감소했습니다.", harvester->hp);
					p_system_message(mes);
				}
			}

			soldier->attack_period = sys_clock + soldier->next_attack_time;
			break;
		}
	}
}

// 2 - [ 적 투사 처리 함수 ]
void handle_enemy_fighter(OBJECT_SAMPLE* soldier, OBJECT_SAMPLE* fighter) {

	// [ 적 주변의 빈 공간 확인 및 이동 ]
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

			// 이동 방향 결정
			if (next_pos.row < soldier->dest.row) next_pos.row++;
			else if (next_pos.row > soldier->dest.row) next_pos.row--;

			if (next_pos.column < soldier->dest.column) next_pos.column++;
			else if (next_pos.column > soldier->dest.column) next_pos.column--;

			// 장애물 처리
			next_pos = sold_move(soldier, next_pos);

			// 이동 적용
			soldier->pos = next_pos;

			// [ 공격 주기 ]
			if (sys_clock <= soldier->attack_period) {
				return;
			}

			// 목적지 도달 시 공격
			if (soldier->pos.row == soldier->dest.row && soldier->pos.column == soldier->dest.column) {
				if (fighter->hp <= 0) {
					p_system_message("적군 투사가 보병에 의해 사망하였습니다.");
					fighter->repr = ' ';
					map[1][fighter->pos.row][fighter->pos.column] = fighter->repr;
				}
				else {
					fighter->hp -= soldier->dps;
					char mes[100];
					sprintf_s(mes, sizeof(mes), "적군 투사가 보병에게 공격받아 hp가 %d로 감소했습니다.", fighter->hp);
					p_system_message(mes);
				}
			}
			soldier->attack_period = sys_clock + soldier->next_attack_time;
			break;
		}
	}
}

// 3 - [ 적 중전차 처리 함수 ]
void handle_enemy_tank(OBJECT_SAMPLE* soldier, OBJECT_SAMPLE* tank) {

	// [ 적 주변의 빈 공간 확인 및 이동 ]
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

			// 이동 방향 결정
			if (next_pos.row < soldier->dest.row) next_pos.row++;
			else if (next_pos.row > soldier->dest.row) next_pos.row--;

			if (next_pos.column < soldier->dest.column) next_pos.column++;
			else if (next_pos.column > soldier->dest.column) next_pos.column--;

			// 장애물 처리
			next_pos = sold_move(soldier, next_pos);

			// 이동 적용
			soldier->pos = next_pos;

			// 목적지 도달 시 공격

			// [ 공격 주기 ]
			if (sys_clock <= soldier->attack_period) {
				return;
			}

			if (soldier->pos.row == soldier->dest.row && soldier->pos.column == soldier->dest.column) {
				if (tank->hp <= 0) {
					p_system_message("적군 중전차가 보병에 의해 파괴되었습니다.");
					tank->repr = ' ';
					map[1][tank->pos.row][tank->pos.column] = tank->repr;
				}
				else {
					tank->hp -= soldier->dps;
					char mes[100];
					sprintf_s(mes, sizeof(mes), "적군 중전차가 보병에게 공격받아 hp가 %d로 감소했습니다.", tank->hp);
					p_system_message(mes);
				}
			}
			soldier->attack_period = sys_clock + soldier->next_attack_time;
			break;
		}
	}
}

// [ 보병 이동 함수 ]
POSITION sold_move(OBJECT_SAMPLE* sold_unit, POSITION target_pos) {
	POSITION diff = psub(target_pos, sold_unit->pos);
	DIRECTION dir;

	// 가로축과 세로축 중 더 먼 쪽으로 이동 방향 결정
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// 기본 이동 위치 계산
	POSITION next_pos = pmove(sold_unit->pos, dir);

	// 맵 내 범위 확인
	if (next_pos.row < 1 || next_pos.row >= MAP_HEIGHT - 1 ||
		next_pos.column < 1 || next_pos.column >= MAP_WIDTH - 1) {
		return sold_unit->pos; // 이동할 수 없으면 현재 위치 유지
	}

	// 장애물 확인 및 대체 경로 계산
	char target_tile = map[0][next_pos.row][next_pos.column]; // 건물
	char target_tile2 = map[1][next_pos.row][next_pos.column]; // 샌드웜

	if (target_tile != ' ' || target_tile2 == 'W') {
		// 장애물을 회피하기 위해 대체 경로 계산
		double best_distance = DBL_MAX;
		POSITION best_pos = sold_unit->pos;

		// 상하좌우로 가능한 대체 경로 탐색
		DIRECTION alternate_dirs[] = { d_up, d_down, d_left, d_right };
		for (int j = 0; j < 4; j++) {
			POSITION alt_pos = pmove(sold_unit->pos, alternate_dirs[j]);

			// 대체 경로가 맵 내에 있는지 확인
			if (alt_pos.row < 1 || alt_pos.row >= MAP_HEIGHT - 1 ||
				alt_pos.column < 1 || alt_pos.column >= MAP_WIDTH - 1) {
				continue; // 맵 밖이면 스킵
			}

			// 대체 경로가 장애물이 아닌지 확인
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
		return best_pos; // 가장 가까운 대체 경로 반환
	}

	return next_pos; // 기본 이동 가능하면 반환
}

// [ 보병 메인 함수 ]
void main_sold_func(void) {
	// [ 보병 선택 ]
	select_sold_act();

	// [ 이동 주기 ]
	for (int i = 0; i < sold_count; i++) {
		if (sys_clock <= sold[i]->move_period) {
			return;
		}
	}

	// [ 보병 이동 및 순찰 로직 ]
	for (int i = 0; i < sold_count; i++) {
		// 경계 검사 추가
		if (i < 0 || i >= MAX_SOLD) {
			continue;  // 유효하지 않은 인덱스 건너뛰기
		}

		POSITION target_pos = { -1, -1 };

		// [ 이동 ]
		if ((sold[i]->dest.row != -1 && sold[i]->dest.column != -1) &&
			(sold[i]->pos.row != sold[i]->dest.row || sold[i]->pos.column != sold[i]->dest.column)) {
			target_pos = sold[i]->dest;
		}

		// [ 순찰 ]
		else if (sold[i]->is_patrolling) {
			// [ 현재 위치에서 patrol_point에 도착했다면 ]
			if (sold[i]->pos.row == sold[i]->patrol_point.row &&
				sold[i]->pos.column == sold[i]->patrol_point.column) {
				// [ 원래 목적지로 돌아가기 ]
				sold[i]->dest = sold[i]->original_dest;
				target_pos = sold[i]->dest;
			}
			// [ dest 위치에 도착했다면 ]
			else if (sold[i]->pos.row == sold[i]->dest.row &&
				sold[i]->pos.column == sold[i]->dest.column) {
				// [ patrol_point로 이동 ]
				sold[i]->original_dest = sold[i]->dest;  // 현재 목적지 저장
				sold[i]->dest = sold[i]->patrol_point;
				target_pos = sold[i]->dest;
			}
			// [ 아직 목적지에 도착하지 않았다면 현재 dest로 이동 ]
			else {
				target_pos = sold[i]->dest;
			}
		}

		// [ 목적지 이동  ]
		if (target_pos.row != -1 && target_pos.column != -1) {
			// [ 이전 위치 지우기 ]
			if (sold[i]->pos.row >= 0 && sold[i]->pos.row < MAP_HEIGHT &&
				sold[i]->pos.column >= 0 && sold[i]->pos.column < MAP_WIDTH &&
				map[1][sold[i]->pos.row][sold[i]->pos.column] == sold[i]->repr) {
				map[1][sold[i]->pos.row][sold[i]->pos.column] = ' ';
				shle_sold_check[sold[i]->pos.row][sold[i]->pos.column] = 0;
			}

			// [ 보병 이동 ]
			POSITION next_pos = sold_move(sold[i], target_pos);
			sold[i]->pos = next_pos;

			// [ 새 위치에 유닛 표시 ]
			if (sold[i]->pos.row >= 0 && sold[i]->pos.row < MAP_HEIGHT &&
				sold[i]->pos.column >= 0 && sold[i]->pos.column < MAP_WIDTH) {
				map[1][sold[i]->pos.row][sold[i]->pos.column] = sold[i]->repr;
				shle_sold_check[sold[i]->pos.row][sold[i]->pos.column] = 2;
			}
		}
	}

	// [ 보병 건물 공격 ]
	sold_attack_building();

	// [ 보병 유닛 공격 ]
	for (int i = 0; i < sold_count; i++) {
		if (sold[i]->e_arena_attack == false && sold[i]->e_dor_attack == false && sold[i]->e_gar_attack == false &&\
			sold[i]->e_base_attack == false && sold[i]->e_factory_attack == false) {
			sold_attack_unit();
		}
	}
	
	

	// [ 이동 주기 ]
	for (int i = 0; i < sold_count; i++) {
		sold[i]->move_period = sys_clock + sold[i]->next_move_time;
	}
}




// [ 프레멘 선택 --> 이동, 순찰, 건물선택 ]
void select_frem_act(void) {
	POSITION curr = cursor.current;

	// [ 프레멘 선택 ]
	if (space_key_press && map[1][curr.row][curr.column] == 'F' && \
		frem_fight_fact_check[curr.row][curr.column] == 2) {
		for (int i = 0; i < frem_count; i++) {
			if (frem[i]->pos.row == curr.row && frem[i]->pos.column == curr.column) {
				selected_frem = i;
				p_system_message("프레멘 선택 완료");
				space_key_press = false;
				space_and_m_key = true; // 이동
				p_p_key_press = true; // 순찰
				break;
			}
		}
	}

	// [ 프레멘 이동 (M) 목적지 정하기 ]
	if (space_and_m_key && m_key_press && selected_frem != -1) {
		// [ 순찰 비활성화 ]
		frem[selected_frem]->is_patrolling = false;

		// [ 사막 지형을 이동 목적지로 정하기 ]
		if (map[0][curr.row][curr.column] == ' ') {
			frem[selected_frem]->dest.row = curr.row;
			frem[selected_frem]->dest.column = curr.column;
			p_system_message("커서 위치(사막지형)를 목적지로 설정");
			selected_frem = -1;
			m_key_press = false;
			p_p_key_press = false;
			space_and_m_key = false;
		}
		// [ 적군 본진 이동 목적지로 정하기 ]
		else if (map[0][curr.row][curr.column] == 'B' && base_barr_check[curr.row][curr.column] == 1 && check_friend[curr.row][curr.column] == 2) {
			frem[selected_frem]->dest.row = e_base.pos3.row + 1;
			frem[selected_frem]->dest.column = e_base.pos3.column;
			frem[selected_frem]->e_base_attack = true;
			p_system_message("적군 본진 위치를 목적지로 설정.");
			selected_frem = -1;
			m_key_press = false;
			p_p_key_press = false;
			space_and_m_key = false;
		}
		// [ 적군 숙소 이동 목적지로 정하기 ]
		else if (map[0][curr.row][curr.column] == 'D' && check_friend[curr.row][curr.column] == 2) {
			for (int i = 0; i < e_dor_count; i++) {
				if ((E_DOR[i]->pos1.row == curr.row && E_DOR[i]->pos1.column == curr.column) || \
					(E_DOR[i]->pos2.row == curr.row && E_DOR[i]->pos2.column == curr.column) || \
					(E_DOR[i]->pos3.row == curr.row && E_DOR[i]->pos3.column == curr.column) || \
					(E_DOR[i]->pos4.row == curr.row && E_DOR[i]->pos4.column == curr.column)) {
					frem[selected_frem]->dest.row = E_DOR[i]->pos3.row + 1;
					frem[selected_frem]->dest.column = E_DOR[i]->pos3.column;
					frem[selected_frem]->e_dor_attack = true;
					p_system_message("적군 숙소 위치를 목적지로 설정.");
					selected_frem = -1;
					m_key_press = false;
					p_p_key_press = false;
					space_and_m_key = false;
					break;
				}
			}
		}
		// [ 적군 창고 이동 목적지로 정하기 ]
		else if (map[0][curr.row][curr.column] == 'G' && check_friend[curr.row][curr.column] == 2) {
			for (int i = 0; i < e_gar_count; i++) {
				if ((E_GAR[i]->pos1.row == curr.row && E_GAR[i]->pos1.column == curr.column) || \
					(E_GAR[i]->pos2.row == curr.row && E_GAR[i]->pos2.column == curr.column) || \
					(E_GAR[i]->pos3.row == curr.row && E_GAR[i]->pos3.column == curr.column) || \
					(E_GAR[i]->pos4.row == curr.row && E_GAR[i]->pos4.column == curr.column)) {
					frem[selected_frem]->dest.row = E_GAR[i]->pos3.row + 1;
					frem[selected_frem]->dest.column = E_GAR[i]->pos3.column;
					frem[selected_frem]->e_gar_attack = true;
					p_system_message("적군 창고 위치를 목적지로 설정.");
					selected_frem = -1;
					m_key_press = false;
					p_p_key_press = false;
					space_and_m_key = false;
					break;
				}
			}
		}
		// [ 적군 투기장 이동 목적지로 정하기 ]
		else if (map[0][curr.row][curr.column] == 'A' && check_friend[curr.row][curr.column] == 2) {
			for (int i = 0; i < are_count; i++) {
				if ((ARE[i]->pos1.row == curr.row && ARE[i]->pos1.column == curr.column) || \
					(ARE[i]->pos2.row == curr.row && ARE[i]->pos2.column == curr.column) || \
					(ARE[i]->pos3.row == curr.row && ARE[i]->pos3.column == curr.column) || \
					(ARE[i]->pos4.row == curr.row && ARE[i]->pos4.column == curr.column)) {
					frem[selected_frem]->dest.row = ARE[i]->pos3.row + 1;
					frem[selected_frem]->dest.column = ARE[i]->pos3.column;
					frem[selected_frem]->e_arena_attack = true;
					p_system_message("적군 투기장 위치를 목적지로 설정.");
					selected_frem = -1;
					m_key_press = false;
					p_p_key_press = false;
					space_and_m_key = false;
					break;
				}
			}
		}

		// [ 적군 공장 이동 목적지로 정하기 ]
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
					p_system_message("적군 공장 위치를 목적지로 설정.");
					selected_frem = -1;
					m_key_press = false;
					p_p_key_press = false;
					space_and_m_key = false;
					break;
				}
			}
		}

		// [ 프레멘 순찰(P) 목적지 정하기 ]
		if (p_p_key_press && frem_p_key_press && selected_frem != -1) {
			if (map[0][curr.row][curr.column] == ' ') {
				frem[selected_frem]->patrol_point.row = frem[selected_frem]->pos.row;
				frem[selected_frem]->patrol_point.column = frem[selected_frem]->pos.column;
				frem[selected_frem]->dest.row = curr.row;
				frem[selected_frem]->dest.column = curr.column;
				frem[selected_frem]->is_patrolling = true; // 순찰 모드 활성화
				p_system_message("순찰 위치 설정 완료. 순찰 시작.");
				selected_frem = -1;
				p_key_press = false;
				p_p_key_press = false;
				frem_p_key_press = false;
			}
			else {
				p_system_message("순찰 위치를 설정할 수 없는 지형입니다.");
				p_p_key_press = false;
			}
		}
	}
}

// [ 프레멘 건물 공격 ]
void frem_attack_building(void) {
	for (int i = 0; i < frem_count; i++) {
		// [ 목적지에 도착했을 때 ]
		if (frem[i]->pos.row == frem[i]->dest.row && frem[i]->pos.column == frem[i]->dest.column) {
			// [ 적군 본진을 공격하라고 명령을 받았을 때 ]
			if (frem[i]->e_base_attack) {
				if (sys_clock <= frem[i]->attack_period) {
					return;
				}

				if (e_base.hp <= 0) {
					p_system_message("적군 본진이 프레멘에 의해 파괴되었습니다.");
					e_base.hp = 0;
					e_base.repr = ' ';
					map[0][e_base.pos1.row][e_base.pos1.column] = ' ';
					map[0][e_base.pos2.row][e_base.pos2.column] = ' ';
					map[0][e_base.pos3.row][e_base.pos3.column] = ' ';
					map[0][e_base.pos4.row][e_base.pos4.column] = ' ';
					frem[i]->e_base_attack = false;

					// [ 게임 종료 ]
					outro();
				}
				else {
					e_base.hp -= frem[i]->dps;
					p_system_message("프레멘이 본진을 공격했습니다.");
					char mes[100];
					sprintf_s(mes, sizeof(mes), "적군 본진의 내구도가 %d로 감소했습니다.", e_base.hp);
					p_system_message(mes);
				}
				frem[i]->attack_period = sys_clock + frem[i]->next_attack_time;
			}
			// [ 적군 숙소를 공격하라고 명령을 받았을 때 ]
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
							p_system_message("적군 숙소가 프레멘에 의해 파괴되었습니다.");
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
							p_system_message("프레멘이 숙소를 공격했습니다.");
							char mes[100];
							sprintf_s(mes, sizeof(mes), "적군 숙소의 내구도가 %d로 감소했습니다.", E_DOR[j]->hp);
							p_system_message(mes);
						}
					}
				}
				frem[i]->attack_period = sys_clock + frem[i]->next_attack_time;
			}
			// [ 적군 창고를 공격하라고 명령을 받았을 때 ]
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
							p_system_message("적군 창고가 프레멘에 의해 파괴되었습니다.");
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
							p_system_message("프레멘이 창고를 공격했습니다.");
							char mes[100];
							sprintf_s(mes, sizeof(mes), "적군 창고의 내구도가 %d로 감소했습니다.", E_GAR[j]->hp);
							p_system_message(mes);
						}
					}
				}
				frem[i]->attack_period = sys_clock + frem[i]->next_attack_time;
			}
			// [ 적군 투기장을 공격하라고 명령을 받았을 때 ]
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
							p_system_message("적군 은신처가 프레멘에 의해 파괴되었습니다.");
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
							p_system_message("프레멘이 은신처를 공격했습니다.");
							char mes[100];
							sprintf_s(mes, sizeof(mes), "적군 은신처의 내구도가 %d로 감소했습니다.", ARE[j]->hp);
							p_system_message(mes);
						}
					}
				}
				frem[i]->attack_period = sys_clock + frem[i]->next_attack_time;
			}
			// [ 적군 공장을 공격하라고 명령을 받았을 때 ]
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
							p_system_message("적군 공장이 프레멘에 의해 파괴되었습니다.");
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
							p_system_message("프레멘이 공장을 공격했습니다.");
							char mes[100];
							sprintf_s(mes, sizeof(mes), "적군 공장의 내구도가 %d로 감소했습니다.", FAC[j]->hp);
							p_system_message(mes);
						}
					}
				}
				frem[i]->attack_period = sys_clock + frem[i]->next_attack_time;
			}
		}
	}
}


// [ 프레멘 유닛 공격 및 적 탐지/처리 ]
void frem_attack_unit(void) {
	for (int i = 0; i < frem_count; i++) {
		OBJECT_SAMPLE* current_frem = frem[i];
		if (!current_frem) continue; // 안전성 검사

		// [ 프레멘 시야 선언 ]
		int sight = current_frem->eyes;

		// [ 프레멘 현재 위치 ]
		int current_row = current_frem->pos.row;
		int current_col = current_frem->pos.column;

		// [ 탐지 플래그 초기화 ]
		bool target_found = false;

		// [ 프레멘 주변에 유닛 있는지 확인 ]
		for (int row = current_row - sight; row <= current_row + sight && !target_found; row++) {
			for (int col = current_col - sight; col <= current_col + sight && !target_found; col++) {
				// [ 맵 경계 확인 ]
				if (row < 0 || row >= MAP_HEIGHT || col < 0 || col >= MAP_WIDTH) continue;

				// [ 프레멘 자신 제외 ]
				if (row == current_row && col == current_col) continue;

				// [ 적 유닛 탐지 및 처리 ]
				for (int j = 0; j < e_hav_count; j++) {
					if (row == e_havs[j]->pos.row && col == e_havs[j]->pos.column && check_friend[row][col] == 2 && e_havs[j]->repr == 'H') {
						if (!current_frem->enemy_found) { // 적을 처음 발견했을 때만 출력
							p_system_message("프레멘이 적군 하베스터를 발견했습니다.");
							p_system_message("프레멘이 적군 하베스터를 목표로 삼았습니다.");
							current_frem->enemy_found = true; // 발견 상태 기록
						}

						// [ 목적지 설정 - 왼쪽, 아래, 위, 오른쪽 순으로 체크 ]
						POSITION potential_destinations[] = {
							{e_havs[j]->pos.row, e_havs[j]->pos.column - 1}, // 왼쪽
							{e_havs[j]->pos.row + 1, e_havs[j]->pos.column}, // 아래
							{e_havs[j]->pos.row - 1, e_havs[j]->pos.column}, // 위
							{e_havs[j]->pos.row, e_havs[j]->pos.column + 1}  // 오른쪽
						};

						for (int k = 0; k < 4; k++) {
							int dest_row = potential_destinations[k].row;
							int dest_col = potential_destinations[k].column;

							// [ 목적지가 맵 경계 안에 있고, 빈 공간인지 확인 ]
							if (dest_row >= 0 && dest_row < MAP_HEIGHT && dest_col >= 0 && dest_col < MAP_WIDTH &&
								map[0][dest_row][dest_col] == ' ') {
								current_frem->dest.row = dest_row;
								current_frem->dest.column = dest_col;
								break;
							}
						}

						// [ 적 유닛 공격 처리 ]
						frem_kill_enemy_hav(current_frem, e_havs[j]);
						target_found = true;
						break;
					}
				}


				for (int j = 0; j < fighter_count && !target_found; j++) {
					if (row == fighter[j]->pos.row && col == fighter[j]->pos.column && frem_fight_fact_check[row][col] == 3 && fighter[j]->repr == 'F') {
						if (!current_frem->enemy_found) { // 적을 처음 발견했을 때만 출력
							p_system_message("프레멘이 적군 투사를 발견했습니다.");
							p_system_message("프레멘이 적군 투사를 목표로 삼았습니다.");
							current_frem->enemy_found = true; // 발견 상태 기록
						}

						// [ 목적지 설정 - 왼쪽, 아래, 위, 오른쪽 순으로 체크 ]
						POSITION potential_destinations[] = {
							{fighter[j]->pos.row, fighter[j]->pos.column - 1}, // 왼쪽
							{fighter[j]->pos.row + 1, fighter[j]->pos.column}, // 아래
							{fighter[j]->pos.row - 1, fighter[j]->pos.column}, // 위
							{fighter[j]->pos.row, fighter[j]->pos.column + 1}  // 오른쪽
						};

						for (int k = 0; k < 4; k++) {
							int dest_row = potential_destinations[k].row;
							int dest_col = potential_destinations[k].column;

							// [ 목적지가 맵 경계 안에 있고, 빈 공간인지 확인 ]
							if (dest_row >= 0 && dest_row < MAP_HEIGHT && dest_col >= 0 && dest_col < MAP_WIDTH &&
								map[0][dest_row][dest_col] == ' ') {
								current_frem->dest.row = dest_row;
								current_frem->dest.column = dest_col;

								char mes[100];
								sprintf_s(mes, 100, "투사 위치:%d/%d, 지금:%d/%d", fighter[j]->pos.row, fighter[j]->pos.column, dest_row, dest_col);
								p_system_message(mes);



								break;
							}
						}

						// [ 적 유닛 공격 처리 ]
						frem_kill_enemy_fighter(current_frem, fighter[j]);
						target_found = true;
						break;
					}
				}

				for (int j = 0; j < tank_count && !target_found; j++) {
					if (row == tank[j]->pos.row && col == tank[j]->pos.column && tank[j]->repr == 'T') {
						if (!current_frem->enemy_found) { // 적을 처음 발견했을 때만 출력
							p_system_message("프레멘이 적군 중전차를 발견했습니다.");
							p_system_message("프레멘이 적군 중전차를 목표로 삼았습니다.");
							current_frem->enemy_found = true; // 발견 상태 기록
						}

						// [ 목적지 설정 - 왼쪽, 아래, 위, 오른쪽 순으로 체크 ]
						POSITION potential_destinations[] = {
							{tank[j]->pos.row, tank[j]->pos.column - 1}, // 왼쪽
							{tank[j]->pos.row + 1, tank[j]->pos.column}, // 아래
							{tank[j]->pos.row - 1, tank[j]->pos.column}, // 위
							{tank[j]->pos.row, tank[j]->pos.column + 1}  // 오른쪽
						};

						for (int k = 0; k < 4; k++) {
							int dest_row = potential_destinations[k].row;
							int dest_col = potential_destinations[k].column;

							// [ 빈 공간인지 확인 ]
							if (map[0][dest_row][dest_col] == ' ') {
								current_frem->dest.row = dest_row;
								current_frem->dest.column = dest_col;
								break;
							}
						}

						// [ 적 유닛 공격 처리 ]
						frem_kill_enemy_tank(current_frem, tank[j]);
						target_found = true;
						break;
					}
				}
			}
		}

		// [ 적을 발견하지 못했을 때 발견 플래그 초기화 ]
		if (!target_found) {
			current_frem->enemy_found = false;
		}
	}
}


// 1 - [ 적 하베스터 처리 함수 ]
void frem_kill_enemy_hav(OBJECT_SAMPLE* frem, OBJECT_SAMPLE* harvester) {
	// [ 공격 주기 ]
		if (sys_clock <= frem->attack_period) {
			return;
		}

		if (frem->pos.row == frem->dest.row && frem->pos.column == frem->dest.column) {
			if (harvester->hp <= 0) {
				p_system_message("적군 하베스터이 프레멘에 의해 사망하였습니다");
				harvester->repr = ' ';
				map[1][harvester->pos.row][harvester->pos.column] = harvester->repr;
				frem->enemy_found = false;
				frem_e_hav_find = false;
				frem_find_enemy = false;
			}
			else {
				harvester->hp -= frem->dps;
				char mes[100];
				sprintf_s(mes, sizeof(mes), "적군 하베스터가 프레멘에게 공격받아 hp가 %d로 감소했습니다", harvester->hp);
				p_system_message(mes);
			}
		}

		frem->attack_period = sys_clock + frem->next_attack_time;
}

// 2 - [ 적 투사 처리 함수 ]
void frem_kill_enemy_fighter(OBJECT_SAMPLE* frem, OBJECT_SAMPLE* fighter) {
		// [ 공격 주기 ]
		if (sys_clock <= frem->attack_period) {
			return;
		}
		p_system_message("1");
		// 목적지 도달 시 공격
		char mes[100];
		sprintf_s(mes, 100, "프레멘: %d/%d --> 목적지: %d/%d", frem->pos.row, frem->pos.column, frem->dest.row, frem->dest.column);
		p_system_message(mes);
		if (frem->pos.row == frem->dest.row && frem->pos.column == frem->dest.column) {
			p_system_message("목적지 도착");
			if (fighter->hp <= 0) {
				p_system_message("적군 투사가 프레멘에 의해 사망하였습니다.");
				fighter->repr = ' ';
				map[1][fighter->pos.row][fighter->pos.column] = fighter->repr;
				frem->enemy_found = false;
				frem_fighter_find = false;
				frem_find_enemy = false;
			}
			else {
				fighter->hp -= 1;
				char mes[100];
				sprintf_s(mes, sizeof(mes), "적군 투사가 프레멘에게 공격받아 hp가 %d로 감소했습니다.", fighter->hp);
				p_system_message(mes);
			}
		}

		frem->attack_period = sys_clock + frem->next_attack_time;
}

// 3 - [ 적 중전차 처리 함수 ]
void frem_kill_enemy_tank(OBJECT_SAMPLE* frem, OBJECT_SAMPLE* tank) {
		// [ 공격 주기 ]
		if (sys_clock <= frem->attack_period) {
			return;
		}

		if (frem->pos.row == frem->dest.row && frem->pos.column == frem->dest.column) {
			if (tank->hp <= 0) {
				p_system_message("적군 중전차가 프레멘에 의해 파괴되었습니다.");
				tank->repr = ' ';
				map[1][tank->pos.row][tank->pos.column] = tank->repr;
				frem->enemy_found = false;
				frem_tank_find = false;
				frem_find_enemy = false;
			}
			else {
				tank->hp -= frem->dps;
				char mes[100];
				sprintf_s(mes, sizeof(mes), "적군 중전차가 프레멘에게 공격받아 hp가 %d로 감소했습니다.", tank->hp);
				p_system_message(mes);
			}
		}

		frem->attack_period = sys_clock + frem->next_attack_time;
}

// [ 프레멘 이동 함수 ]
POSITION frem_move(OBJECT_SAMPLE* frem, POSITION target_pos) {
	POSITION diff = psub(target_pos, frem->pos);
	DIRECTION dir;

	// 가로축과 세로축 중 더 먼 쪽으로 이동 방향 결정
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// 기본 이동 위치 계산
	POSITION next_pos = pmove(frem->pos, dir);

	// 맵 내 범위 확인
	if (next_pos.row < 1 || next_pos.row >= MAP_HEIGHT - 1 ||
		next_pos.column < 1 || next_pos.column >= MAP_WIDTH - 1) {
		return frem->pos; // 이동할 수 없으면 현재 위치 유지
	}

	// 장애물 확인 및 대체 경로 계산
	char target_tile = map[0][next_pos.row][next_pos.column]; // 건물
	char target_tile2 = map[1][next_pos.row][next_pos.column]; // 샌드웜
	if (target_tile != ' ' || target_tile2 == 'W') {
		// 장애물을 회피하기 위해 대체 경로 계산
		double best_distance = DBL_MAX;
		POSITION best_pos = frem->pos;

		// 상하좌우로 가능한 대체 경로 탐색
		DIRECTION alternate_dirs[] = { d_up, d_down, d_left, d_right };
		for (int j = 0; j < 4; j++) {
			POSITION alt_pos = pmove(frem->pos, alternate_dirs[j]);

			// 대체 경로가 맵 내에 있는지 확인
			if (alt_pos.row < 1 || alt_pos.row >= MAP_HEIGHT - 1 ||
				alt_pos.column < 1 || alt_pos.column >= MAP_WIDTH - 1) {
				continue; // 맵 밖이면 스킵
			}

			// 대체 경로가 장애물이 아닌지 확인
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
		return best_pos; // 가장 가까운 대체 경로 반환
	}

	return next_pos; // 기본 이동 가능하면 반환
}

// [ 프레멘 메인 함수 ]
void frem_main_func(void) {
	// [ 프레멘 선택 ]
	select_frem_act();

	// [ 이동 주기 ]
	for (int i = 0; i < frem_count; i++) {
		if (sys_clock <= frem[i]->move_period) {
			return;
		}
	}
	// [ 프레멘 이동 및 순찰 로직 ]
	for (int i = 0; i < frem_count; i++) {
		// 경계 검사 추가
		if (i < 0 || i >= MAX_FREM) {
			continue;  // 유효하지 않은 인덱스 건너뛰기
		}

		POSITION target_pos = { -1, -1 };

		// [ 이동 ]
		if ((frem[i]->dest.row != -1 && frem[i]->dest.column != -1) &&
			(frem[i]->pos.row != frem[i]->dest.row || frem[i]->pos.column != frem[i]->dest.column)) {
			target_pos = frem[i]->dest;
		}

		// [ 순찰 ]
		else if (frem[i]->is_patrolling) {
			// [ 현재 위치에서 patrol_point에 도착했다면 ]
			if (frem[i]->pos.row == frem[i]->patrol_point.row &&
				frem[i]->pos.column == frem[i]->patrol_point.column) {
				// [ 원래 목적지로 돌아가기 ]
				frem[i]->dest = frem[i]->original_dest;
				target_pos = frem[i]->dest;
			}
			// [ dest 위치에 도착했다면 ]
			else if (frem[i]->pos.row == frem[i]->dest.row &&
				frem[i]->pos.column == frem[i]->dest.column) {
				// [ patrol_point로 이동 ]
				frem[i]->original_dest = frem[i]->dest;  // 현재 목적지 저장
				frem[i]->dest = frem[i]->patrol_point;
				target_pos = frem[i]->dest;
			}
			// [ 아직 목적지에 도착하지 않았다면 현재 dest로 이동 ]
			else {
				target_pos = frem[i]->dest;
			}
		}

		// [ 목적지 이동 ]
		if (target_pos.row != -1 && target_pos.column != -1) {
			// [ 이전 위치 지우기 ]
			if (frem[i]->pos.row >= 0 && frem[i]->pos.row < MAP_HEIGHT &&
				frem[i]->pos.column >= 0 && frem[i]->pos.column < MAP_WIDTH &&
				map[1][frem[i]->pos.row][frem[i]->pos.column] == frem[i]->repr) {
				map[1][frem[i]->pos.row][frem[i]->pos.column] = ' ';
				frem_fight_fact_check[frem[i]->pos.row][frem[i]->pos.column] = 0;
			}

			// [ 프레멘 이동 ]
			POSITION next_pos = frem_move(frem[i], target_pos);
			frem[i]->pos = next_pos;

			// [ 새 위치에 유닛 표시 ]
			if (frem[i]->pos.row >= 0 && frem[i]->pos.row < MAP_HEIGHT &&
				frem[i]->pos.column >= 0 && frem[i]->pos.column < MAP_WIDTH) {
				map[1][frem[i]->pos.row][frem[i]->pos.column] = frem[i]->repr;
				frem_fight_fact_check[frem[i]->pos.row][frem[i]->pos.column] = 2;
			}
		}
	}

	// [ 프레멘 건물 공격 ]
	frem_attack_building();

	// [ 프레멘 유닛 공격 ]
	frem_attack_unit();



	// [ 이동 주기 ]
	for (int i = 0; i < frem_count; i++) {
		frem[i]->move_period = sys_clock + frem[i]->next_move_time;
	}
}







/* -=-=-=-=-=-=-=-=[ 하코넨 진영 AI ]-=-=-=-=-=-=-=-= -*/


// [ 하코넨 초기 유닛 초기화 ]
void hako_unit_reset(void) {
	fighter[0] = &fighter1;
	tank[0] = &tank1;
}


// [ 하코넨 초기 유닛 배치 ]
void hako_unit(void) {
	// [ 유닛 초기화 ]
	hako_unit_reset();

	// [ 유닛 출력 ] 
	// [ 투사 ]
	map[1][fighter[0]->pos.row][fighter[0]->pos.column] = fighter[0]->repr;
	check_friend[fighter[0]->pos.row][fighter[0]->pos.column] = 2;
	frem_fight_fact_check[fighter[0]->pos.row][fighter[0]->pos.column] = 3;
	// [ 중전차 ]
	map[1][tank[0]->pos.row][tank[0]->pos.column] = tank[0]->repr;
	check_friend[tank[0]->pos.row][tank[0]->pos.column] = 2;

}

