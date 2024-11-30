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

// [ 하베스터 & 스파이스 관련 ]
OBJECT_SAMPLE* havs[MAX_HAV]; 
int hav_count = 1; 
int selected_harvester = -1; // 선택된 하베스터 인덱스 (-1: 선택 안됨)

void hav_reset(void);
void harvest_move_all(OBJECT_SAMPLE havs[MAX_HAV], int hav_count, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], OBJECT_BUILDING* SPICE);
int harvest_gather_spice(OBJECT_BUILDING* spice, OBJECT_SAMPLE* hav, char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
POSITION harvest_next_move(OBJECT_SAMPLE* hav, POSITION new_dest);
POSITION harvest_set_dest(POSITION curr, OBJECT_SAMPLE* hav, OBJECT_BUILDING* SPICE);
void handle_harvester_input(CURSOR cursor, OBJECT_SAMPLE havs[MAX_HAV], int hav_count, OBJECT_BUILDING* SPICE);

int spice_in_progress[MAX_SPICE] = { -1 }; // 각 스파이스 매장지에 대한 작업 상태 (-1: 작업 없음)


// [ 보병 ]
OBJECT_SAMPLE* sold[MAX_SOLD];
int sold_count = 0;
int selected_sold = -1;

extern int base_barr_check[MAP_HEIGHT][MAP_WIDTH];

// [ 프레멘 ]
OBJECT_SAMPLE* frem[MAX_FREM];
int frem_count = 0;
int selected_frem = -1;
extern int frem_fight_fact_check[MAP_HEIGHT][MAP_WIDTH];

// [ 건물 ]
extern OBJECT_BUILDING e_base;

extern OBJECT_BUILDING* SPICE;
extern int spi_count;

extern OBJECT_BUILDING* DOR[MAX_DOR];
extern int dor_count;

extern OBJECT_BUILDING* GAR[MAX_GAR];
extern int gar_count;

extern OBJECT_BUILDING* BAR[MAX_BAR];
extern int bar_count;

extern OBJECT_BUILDING* SHE[MAX_SHE];
extern int she_count;

extern OBJECT_BUILDING* ARE[MAX_ARE];
extern int are_count;

extern OBJECT_BUILDING* FAC[MAX_FAC];
extern int fac_count;


/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
CURSOR cursor = { { 1, 1 }, {1, 1} };

int move_count = 0;
int move_check[2] = { 0 };
char spice_number[9] = { '1', '2', '3', '4', '5', '6', '7', '8', '9' };
int spice_plus_value = 0;
int* spice_plus = &spice_plus_value;

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
bool space_key_press = false; // 스페이스바가 눌렸는지 확인 / false = 비활성화, true = 활성하ㅗ
bool hav_move = false;
bool h_key_press = false;
bool space_and_h_key = false;
bool space_and_m_key = false;


bool m_key_press = false; // 이동 확인
bool p_p_key_press = false; // 순찰 확인


// [ 자원 ]
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


/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL));

	hav_reset();
	init();
	intro();
	display(resource, map, cursor, state_map, sysmes_map, order_map, check_friend, big_cursor);
	p_building(map, check_friend);
	
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
			case k_p: press_p();
				// [ 시스템 메시지 추가 ]
				p_system_message("P키를 눌렀습니다.");

				if (map[0][cursor.current.row][cursor.current.column] == ' ') {
					if (b_key_press) {
						big_cursor = true;
						p_key_press = true;
					}
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

			case k_m:
				p_system_message("M키를 눌렀습니다.");

			// [ F ]
			case k_f:
				p_system_message("F키를 눌렀습니다.");
				press_f(cursor, &resource, map, &space_key_press);

			case k_none:
			case k_undef:
			default: break;
			}
		}
	
		// [ 오브젝트 동작 ]
		d_eagle_move();
		sw1_move();
		sw2_move(); 

		// 하베스터 움직이기
		harvest_move_all(*havs, hav_count, map, SPICE);

		

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
	map[2][d_eagle.pos.row][d_eagle.pos.column] = d_eagle.repr;

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


/* ================= [유닛 움직임 함수 ] =================== */

// [ 사막 독수리 ]
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

// [ 사막 독수리 움직임 함수 ]
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
		spi_count++;
	}
	sw1_obj.pos = sw1_next_pos();

	// [ 샌드웜 잡아먹기 ]
	// [ 아군 하베스터와 만났을 때 (미완성) ]
	if (map[1][sw1_obj.pos.row][sw1_obj.pos.column] == 'H' && check_friend[sw1_obj.pos.row][sw1_obj.pos.column] == 1) {
		// [ 시스템 메시지 추가 ]
		send_system_message[0] = "샌드웜이 아군 하베스터를 잡아먹었습니다.";
		p_system_message(send_system_message[0]);
		
		//// 임시로 repr 변경 (원래는 샌드웜이 밟고 지나가면 사라지는데 나중에 하베스터 움직임 추가할 때 map상에서는 ' '여도 repr 상태는 H니까 하베스터가 움지깅게 되면 다시 H가 출력될것임 이건 나중에 추가 및 수정 해야할듯?)
		//if (f_hav_obj.pos.row == sw1_obj.pos.row && f_hav_obj.pos.column == sw1_obj.pos.column) {
		//	f_hav_obj.repr = ' ';
		//}
	
		// [ 다음 이동 유닛 결정 ] // 지금은 유닛이 하베스터 밖에 없기도 하고 구체적인 구조체가 짜여있지않아서 오류가 발생하므로 임시로 2,4 원래 자리로 해놓았음.
		POSITION new_dest = { 2, 4 }; 
		sw1_obj.dest = new_dest; 

	}

	// [ 적군 하베스터와 만났을 때 (미완성) ]
	else if (map[1][sw1_obj.pos.row][sw1_obj.pos.column] == 'H' && check_friend[sw1_obj.pos.row][sw1_obj.pos.column] == 2) {
		// [ 시스템 메시지 추가 ]
		send_system_message[0] = "샌드웜이 적군 하베스터를 잡아먹었습니다.";
		p_system_message(send_system_message[0]);

		//// 임시로 repr 변경
		//e_hav_obj.repr = ' ';
	}

	// [ 투사와 만났을 때 (아직 구현 X) ]
	else if (map[1][sw1_obj.pos.row][sw1_obj.pos.column] == 'F') {
		send_system_message[0] = "샌드웜이 투사를 잡아먹었습니다.";
		p_system_message(send_system_message[0]);


	}

	// [ 보병 만났을 때 (아직 구현 X) ]
	else if (map[1][sw1_obj.pos.row][sw1_obj.pos.column] == 'S') {
		send_system_message[0] = "샌드웜이 보병을 잡아먹었습니다.";
		p_system_message(send_system_message[0]);


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
			.pos1 = {sw1_obj.pos.row, sw1_obj.pos.column},
			.repr = spice_number[r],
			.layer = 0
		};

		SPICE[spi_count] = new_spice;
		spi_count++;
	}
	sw2_obj.pos = sw2_next_pos();

	// [ 샌드웜 잡아먹기 ]
	// [ 아군 하베스터와 만났을 때 (미완성) ]
	if (map[1][sw2_obj.pos.row][sw2_obj.pos.column] == 'H' && check_friend[sw2_obj.pos.row][sw2_obj.pos.column] == 1) {
		// [ 시스템 메시지 추가 ]
		send_system_message[0] = "샌드웜이 아군 하베스터를 잡아먹었습니다.";
		p_system_message(send_system_message[0]);

		// [ 다음 이동 유닛 결정 ] // 지금은 유닛이 하베스터 밖에 없기도 하고 구체적인 구조체가 짜여있지않아서 오류가 발생하므로 임시로 2,4 원래 자리로 해놓았음.
		POSITION new_dest = { 12, 55 };
		sw1_obj.dest = new_dest;

		//// 임시로 repr 변경
		//f_hav_obj.repr = ' ';
	}

	// [ 적군 하베스터와 만났을 때 (미완성) ]
	else if (map[1][sw2_obj.pos.row][sw2_obj.pos.column] == 'H' && check_friend[sw2_obj.pos.row][sw2_obj.pos.column] == 2) {
		// [ 시스템 메시지 추가 ]
		send_system_message[0] = "샌드웜이 적군 하베스터를 잡아먹었습니다.";
		p_system_message(send_system_message[0]);

		// [ 다음 이동 유닛 결정 ] // 지금은 유닛이 하베스터 밖에 없기도 하고 구체적인 구조체가 짜여있지않아서 오류가 발생하므로 임시로 2,4 원래 자리로 해놓았음.
		POSITION new_dest = { 2, 4 };
		sw1_obj.dest = new_dest;

		//// 임시로 repr 변경
		//e_hav_obj.repr = ' ';
	}

	// [ 투사와 만났을 때 (아직 구현 X) ]
	else if (map[1][sw2_obj.pos.row][sw2_obj.pos.column] == 'F') {
		send_system_message[0] = "샌드웜이 투사를 잡아먹었습니다.";
		p_system_message(send_system_message[0]);


	}

	// [ 보병 만났을 때 (아직 구현 X) ]
	else if (map[1][sw2_obj.pos.row][sw2_obj.pos.column] == 'S') {
		send_system_message[0] = "샌드웜이 보병을 잡아먹었습니다.";
		p_system_message(send_system_message[0]);


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

	if (sys_clock <= f_hav_obj.next_move_time) {
		return; // 이동 주기가 되지 않았음
	}

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

		POSITION next_pos = harvest_next_move(&havs[i], havs[i].dest);
		havs[i].pos = next_pos;
		check_friend[havs[i].pos.row][havs[i].pos.column] = 1;

		if (map[0][havs[i].pos.row][havs[i].pos.column] >= '1' && map[0][havs[i].pos.row][havs[i].pos.column] <= '9') {
			// 스파이스 위치는 그대로 유지
		}
		else {
			map[1][havs[i].pos.row][havs[i].pos.column] = havs[i].repr; // 새 위치 표시
		}
	}
	f_hav_obj.next_move_time = sys_clock + f_hav_obj.move_period; // 이동 주기 설정
}


// [ 보병 선택 --> 이동, 순찰, 건물 선택 ]
void select_sold_act(void) {
	POSITION curr = cursor.current;

	// [ 보병 선택 ]
	if (space_key_press && map[0][curr.row][curr.column] == 'S') {
		for (int i = 0; i < sold_count; i++) {
			if (sold[i]->pos.row == curr.row && sold[i]->pos.column == curr.column) {
				selected_sold = i;
				p_system_message("보병 선택 완료");
				space_key_press = false;
				space_and_m_key = true; // 이동
				p_p_key_press = true; // 순찰
			}
		}
	}

	// [ 보병 이동(M) 목적지 정하기 ]
	if (space_and_m_key && m_key_press && selected_sold != -1) {
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
			p_system_message("적군 본진 위치를 목적지로 설정.");
			selected_sold = -1;
			m_key_press = false;
			p_p_key_press = false;
			space_and_m_key = false;
		}
		// [ 적군 숙소 이동 목적지로 정하기 ]
		else if (map[0][curr.row][curr.column] == 'D' && check_friend[curr.row][curr.column] == 2) {
			for (int i = 0; i < dor_count; i++) {
				if ((DOR[i]->pos1.row == curr.row && DOR[i]->pos1.column == curr.column) || \
					(DOR[i]->pos2.row == curr.row && DOR[i]->pos2.column == curr.column) || \
					(DOR[i]->pos3.row == curr.row && DOR[i]->pos3.column == curr.column) || \
					(DOR[i]->pos4.row == curr.row && DOR[i]->pos4.column == curr.column)) {
					sold[selected_sold]->dest.row = DOR[i]->pos3.row + 1;
					sold[selected_sold]->dest.column = DOR[i]->pos3.column;
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
			for (int i = 0; i < gar_count; i++) {
				if ((GAR[i]->pos1.row == curr.row && GAR[i]->pos1.column == curr.column) || \
					(GAR[i]->pos2.row == curr.row && GAR[i]->pos2.column == curr.column) || \
					(GAR[i]->pos3.row == curr.row && GAR[i]->pos3.column == curr.column) || \
					(GAR[i]->pos4.row == curr.row && GAR[i]->pos4.column == curr.column)) {
					sold[selected_sold]->dest.row = GAR[i]->pos3.row + 1;
					sold[selected_sold]->dest.column = GAR[i]->pos3.column;
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
		else if (map[0][curr.row][curr.column] == 'A' && check_friend[curr.row][curr.column] == 2 && \
			frem_fight_fact_check[curr.row][curr.column] == 1) {
			for (int i = 0; i < fac_count; i++) {
				if ((FAC[i]->pos1.row == curr.row && FAC[i]->pos1.column == curr.column) || \
					(FAC[i]->pos2.row == curr.row && FAC[i]->pos2.column == curr.column) || \
					(FAC[i]->pos3.row == curr.row && FAC[i]->pos3.column == curr.column) || \
					(FAC[i]->pos4.row == curr.row && FAC[i]->pos4.column == curr.column)) {
					sold[selected_sold]->dest.row = FAC[i]->pos3.row + 1;
					sold[selected_sold]->dest.column = FAC[i]->pos3.column;
					p_system_message("적군 공장 위치를 목적지로 설정.");
					selected_sold = -1;
					m_key_press = false;
					p_p_key_press = false;
					space_and_m_key = false;
					break;
				}
			}
		}

	// [ 보병 순찰(P) 목적지 정하기 ]












	}










}
// [ 보병 건물 목적지 정하기 ]

// [ 보병 이동 ]

// [ 보병 유닛 전투 ]

// [ 보병 건물 공격 ]

// [ 보병 움직임 메인 함수 ]