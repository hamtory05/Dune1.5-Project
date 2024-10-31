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

POSITION sw1_next_pos(void);
void sw1_move(void);
POSITION sw2_next_pos(void);
void sw2_move(void);

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

RESOURCE resource = {
	.spice = 0,
	.spice_max = 5,
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
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'H',
	.move_period = 2000,
	.next_move_time = 2000
};

// [ 적군 하베스터 ]
OBJECT_SAMPLE e_hav_obj = {
	.pos = {3, 58},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'H',
	.move_period = 2000,
	.next_move_time = 2000
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




/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL));

	init();
	intro();
	display(resource, map, cursor, state_map, sysmes_map, order_map);

	while (1) {
		// loop 돌 때마다(즉, TICK==10ms마다) 키 입력 확인
		KEY key = get_key();

		// sys_clock값에 따라 커서 여러칸 이동 변수 초기화
		if (sys_clock % 90 == 0) {
			move_count = 0; // 커서가 1칸 움직이는지 3칸 움직이는지 확인
			for (int i = 0; i < 2; i++) {
				move_check[i] = 0; // 3칸 움직이는 커서일 때, 동일한 방향키를 두번 눌렀는지 확인
			}
		}

		// 키 입력이 있을 때, 1칸이동인지, 3칸이동인지 확인
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
		
		// 키 입력이 있으면 처리
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
			case k_quit: outro();
			
			case k_esc: state_esc(state_map);
				break;
			
			case k_space: state_spacebar(map, state_map, cursor);
				break;
			
			case k_none:
			case k_undef:
			default: break;
			}
		}
	
		// 샘플 오브젝트 동작
		sw1_move();
		sw2_move();

		// 화면 출력
		display(resource, map, cursor, state_map, sysmes_map, order_map);
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
	// layer 0(map[0])에 지형 생성
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

	// layer 1(map[1])은 비워 두기(-1로 채움)
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[1][i][j] = -1;
		}
	}

	// [ 상태창 기본 틀 ]
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
	
	// [ 시스템 기본 틀 ]
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

	// [ 명령창 기본 틀 ]
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



	// object sample
	map[1][obj.pos.row][obj.pos.column] = 'o';

	// [ 유닛 오브젝트 ]
	map[1][f_hav_obj.pos.row][f_hav_obj.pos.column] = f_hav_obj.repr; // 아군 하베스터
	map[1][e_hav_obj.pos.row][e_hav_obj.pos.column] = e_hav_obj.repr; // 적군 하베스터 
	map[1][sw1_obj.pos.row][sw1_obj.pos.column] = sw1_obj.repr; // 샌드웜 1
	map[1][sw2_obj.pos.row][sw2_obj.pos.column] = sw2_obj.repr; // 샌드웜 2
}



// (가능하다면) 지정한 방향으로 커서 이동
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


/* ================= sample object movement =================== */
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



// [ 샌드웜 (1) ]

POSITION sw1_next_pos(void) {
	// 현재 위치와 목적지를 비교해서 이동 방향 결정	
	POSITION diff = psub(sw1_obj.dest, sw1_obj.pos);
	DIRECTION dir;

	

	// [ 목적지는 가장 가까운 유닛 ]
	
	// 가장 가까운 유닛을 찾아서 그 유닛의 행렬값을 new_dest에 지정
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


	// 가로축, 세로축 거리를 비교해서 더 먼 쪽 축으로 이동
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}


	// 유닛과 만났을 때 (겹쳐졌을 때) 유닛과 전투 (잡아먹기)
	POSITION next_pos = pmove(sw1_obj.pos, dir);
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] < 0) {

		return next_pos;
	}

	// [ 아군 하베스터와 만났을 때 (미완성) ]
	else if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] == 'H') {
		
		f_hav_obj.repr = ' ';
		return next_pos;
	} 

	// [ 적군 하베스터와 만났을 때 (미완성) ]
	else if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] == 'H' ) {

		e_hav_obj.repr = ' ';
		return next_pos;
	}

	// [ 프레멘과 만났을 때 (아직 구현 X) ]
	else if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] == 'F') {

		return next_pos;
	}

	// [ 투사와 만났을 때 (아직 구현 X) ]
	else if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] == 'F') {

		return next_pos;
	}

	// [ 보병 만났을 때 (아직 구현 X) ]
	else if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] == 'S') {

		return next_pos;
	}

	// [ 중전차 만났을 때 (아직 구현 X) ]
	else if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] == 'T') {

		return next_pos;
	}

	// [ 바위와 만났을 때 피해가기 (미완성) ]
	else if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[0][next_pos.row][next_pos.column] == 'R') {
		// [ 바위가 위, 아래에 있을 때 ]
		if (sw1_obj.pos.column + 1 == next_pos.column || sw1_obj.pos.column - 1 == next_pos.column) {
			// [ 오른쪽으로 갈 때가 빠른지 왼쪽으로 갈 때가 빠른지 비교 ]
			double move_left = sqrt(pow((sw1_obj.pos.row - 1) - new_dest.row, 2) + pow(sw1_obj.pos.column - new_dest.column, 2));
			double move_right = sqrt(pow((sw1_obj.pos.row + 1) - new_dest.row, 2) + pow(sw1_obj.pos.column - new_dest.column, 2));;
			// [ 오른쪽으로 가기 ]
			if (move_left < move_right) {
				next_pos.row = next_pos.row + 1;
			}
			// [ 왼쪽으로 가기 ]
			else {
				next_pos.row = next_pos.row - 1;
			}
		}
		// 바위가 오른쪽, 왼쪽에 있을 때
		else {
			// [ 위로 갈 때가 빠른지 아래로 갈 때가 빠른지 비교 ]
			double move_up = sqrt(pow(sw1_obj.pos.row - new_dest.row, 2) + pow(sw1_obj.pos.column - 1 - new_dest.column, 2));
			double move_down = sqrt(pow(sw1_obj.pos.row - new_dest.row, 2) + pow((sw1_obj.pos.column + 1) - new_dest.column, 2));
			// [ 위로 가기 ]
			if (move_up < move_down) {
				next_pos.column = next_pos.column + 1;
			}
			// [ 아래로 가기 ]
			else {
				next_pos.column = next_pos.column - 1;
			}
		}
		return next_pos;
	}

	// [ 샌드웜(2)와 만났을 때 피해가기 (미완성) ]
	else if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] == 'W') {

		// 샌드웜(2)가 위, 아래에 있을 때
		if (sw1_obj.pos.column + 1 == next_pos.column || sw1_obj.pos.column - 1 == next_pos.column) {
			// [ 오른쪽으로 갈 때가 빠른지 왼쪽으로 갈 때가 빠른지 비교 ]
			double move_left = sqrt(pow((sw1_obj.pos.row - 1) - new_dest.row, 2) + pow(sw1_obj.pos.column - new_dest.column, 2));
			double move_right = sqrt(pow((sw1_obj.pos.row + 1) - new_dest.row, 2) + pow(sw1_obj.pos.column - new_dest.column, 2));;
			// [ 오른쪽으로 가기 ]
			if (move_left < move_right) {
				next_pos.row = next_pos.row + 1;
			}
			// [ 왼쪽으로 가기 ]
			else {
				next_pos.row = next_pos.row - 1;
			}
		}
		// 샌드웜(2) 오른쪽, 왼쪽에 있을 때
		else {
			// [ 위로 갈 때가 빠른지 아래로 갈 때가 빠른지 비교 ]
			double move_up = sqrt(pow(sw1_obj.pos.row - new_dest.row, 2) + pow(sw1_obj.pos.column - 1 - new_dest.column, 2));
			double move_down = sqrt(pow(sw1_obj.pos.row - new_dest.row, 2) + pow((sw1_obj.pos.column + 1) - new_dest.column, 2));
			// [ 위로 가기 ]
			if (move_up < move_down) {
				next_pos.column = next_pos.column + 1;
			}
			// [ 아래로 가기 ]
			else {
				next_pos.column = next_pos.column - 1;
			}
		}
		return sw1_obj.pos;
	}
}

void sw1_move(void) {
	// 움직이는 주기
	if (sys_clock <= sw1_obj.next_move_time) {
		return;
	}
	// 샌드웜(1) layer1 (map[1])에 저장
	map[1][sw1_obj.pos.row][sw1_obj.pos.column] = -1;

	// 10/300 확률로 배설 --> 스파이스 매장지 생성 (매장량 1 ~ 9 랜덤)
	int r = rand() % 299;
	if (r < 9) {
		map[0][sw1_obj.pos.row][sw1_obj.pos.column] = spice_number[r];
	}

	sw1_obj.pos = sw1_next_pos();
	map[1][sw1_obj.pos.row][sw1_obj.pos.column] = sw1_obj.repr;

	sw1_obj.next_move_time = sys_clock + sw1_obj.move_period;
}

// [ 샌드웜 (2) ]

POSITION sw2_next_pos(void) {
	// 현재 위치와 목적지를 비교해서 이동 방향 결정	
	POSITION diff = psub(sw2_obj.dest, sw2_obj.pos);
	DIRECTION dir;

	// [ 목적지는 가장 가까운 유닛 ]

	// 가장 가까운 유닛을 찾아서 그 유닛의 행렬값을 new_dest에 지정
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


	// 가로축, 세로축 거리를 비교해서 더 먼 쪽 축으로 이동
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// 유닛과 만났을 때 (겹쳐졌을 때) 유닛과 전투 (잡아먹기)
	POSITION next_pos = pmove(sw2_obj.pos, dir);
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] < 0) {

		return next_pos;
	}
	// [ 아군 하베스터와 만났을 때 (미완성) ]
	else if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] == 'H') {

		f_hav_obj.repr = ' ';
		return next_pos;
	}
	// [ 적군 하베스터와 만났을 때 (미완성) ]
	else if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] == 'H') {

		e_hav_obj.repr = ' ';
		return next_pos;
	}
	// [ 프레멘과 만났을 때 (아직 구현 X) ]
	else if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] == 'F') {

		return next_pos;
	}
	// [ 투사와 만났을 때 (아직 구현 X) ]
	else if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] == 'F') {

		return next_pos;
	}
	// [ 보병 만났을 때 (아직 구현 X) ]
	else if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] == 'S') {

		return next_pos;
	}
	// [ 중전차 만났을 때 (아직 구현 X) ]
	else if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] == 'T') {

		return next_pos;
	}
	// [ 바위와 만났을 때 피해가기 (미완성) ]
	else if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[0][next_pos.row][next_pos.column] == 'R') {
		// [ 바위가 위, 아래에 있을 때 ]
		if (sw2_obj.pos.column + 1 == next_pos.column || sw2_obj.pos.column - 1 == next_pos.column) {
			// [ 오른쪽으로 갈 때가 빠른지 왼쪽으로 갈 때가 빠른지 비교 ]
			double move_left = sqrt(pow((sw2_obj.pos.row - 1) - new_dest.row, 2) + pow(sw2_obj.pos.column - new_dest.column, 2));
			double move_right = sqrt(pow((sw2_obj.pos.row + 1) - new_dest.row, 2) + pow(sw2_obj.pos.column - new_dest.column, 2));;
			// [ 오른쪽으로 가기 ]
			if (move_left < move_right) {
				next_pos.row = next_pos.row + 1;
			}
			// [ 왼쪽으로 가기 ]
			else {
				next_pos.row = next_pos.row - 1;
			}
		}
		// 바위가 오른쪽, 왼쪽에 있을 때
		else {
			// [ 위로 갈 때가 빠른지 아래로 갈 때가 빠른지 비교 ]
			double move_up = sqrt(pow(sw2_obj.pos.row - new_dest.row, 2) + pow(sw2_obj.pos.column - 1 - new_dest.column, 2));
			double move_down = sqrt(pow(sw2_obj.pos.row - new_dest.row, 2) + pow((sw2_obj.pos.column + 1) - new_dest.column, 2));
			// [ 위로 가기 ]
			if (move_up < move_down) {
				next_pos.column = next_pos.column + 1;
			}
			// [ 아래로 가기 ]
			else {
				next_pos.column = next_pos.column - 1;
			}
		}
		return next_pos;
	}
	// [ 샌드웜(1)과 만났을 때 피해가기 (미완성) ]
	else if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] == 'W') {
		// 샌드웜(1)이 피해갈거라 샌드웜(2)는 갈 길 가면 됨.
		return next_pos;
	}
}

void sw2_move(void) {
	// 움직이는 주기
	if (sys_clock <= sw2_obj.next_move_time) {
		return;
	}
	// 샌드웜(2) layer1 (map[1])에 저장
	map[1][sw2_obj.pos.row][sw2_obj.pos.column] = -1;

	// 10/300 확률로 배설 --> 스파이스 매장지 생성 (매장량 1 ~ 9 랜덤)
	int r = rand() % 299;
	if (r < 9) {
		map[0][sw2_obj.pos.row][sw2_obj.pos.column] = spice_number[r];
	}

	sw2_obj.pos = sw2_next_pos();
	map[1][sw2_obj.pos.row][sw2_obj.pos.column] = sw2_obj.repr;

	sw2_obj.next_move_time = sys_clock + sw2_obj.move_period;
}







