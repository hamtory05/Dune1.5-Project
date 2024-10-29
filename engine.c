#include <stdlib.h>
#include <time.h>
#include <assert.h>
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


/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };  // 2, 16, 60
char state_map[STATE_HEIGHT][STATE_WIDTH] = { 0 }; // 16, 110


RESOURCE resource = {
	.spice = 0,
	.spice_max = 0,
	.population = 0,
	.population_max = 0
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
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'W',
	.move_period = 2500,
	.next_move_time = 2500
};




/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL));

	init();
	intro();
	display(resource, map, cursor, state_map);

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
		sample_obj_move();
		sw1_move();

		// 화면 출력
		display(resource, map, cursor, state_map);
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

	// 명렁 & 상태창 기본 틀
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
	
	// 시스템 기본 틀

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

	// 목적지 도착. 지금은 단순히 원래 자리로 왕복
	if (diff.row == 0 && diff.column == 0) {
		return obj.pos;
	}

	// [ 목적지는 가장 가까운 유닛 ]
	
	// 가장 가까운 유닛을 찾아서 그 유닛의 좌표값을 dest에 지정. (같은 샌드웜끼리는 제외)
	int close_i = MAP_HEIGHT - 1, close_j = MAP_WIDTH - 1;
	for (int i = 1; i < MAP_HEIGHT - 1; i++) {
		for (int j = 1; j < MAP_WIDTH - 1; j++) {
			// 하베스터와 가장 가깝다고 가정하고, 실제로 하베스터가 가장 가까우면
			// 하베스터한테 이동, 다른 유닛이 더 가까우면 다른 유닛한테 이동
			
			// [ 샌드웜 --> 하베스터 ]
			if (map[1][i][j] == 'H') {
				if (abs(sw1_obj.pos.row - i) < close_i && abs(sw1_obj.pos.column - j) < close_j) {
					close_i = sw1_obj.pos.row - i, close_j = sw1_obj.pos.row - j;
					POSITION new_dest = { i, j };
					sw1_obj.dest = new_dest;
				}
			}
			// [ 샌드웜 --> 프레멘 ]
			else if (map[1][i][j] == 'F') {
				if (abs(sw1_obj.pos.row - i) < close_i && abs(sw1_obj.pos.column - j) < close_j) {
					close_i = sw1_obj.pos.row - i, close_j = sw1_obj.pos.row - j;
					POSITION new_dest = { i, j };
					sw1_obj.dest = new_dest;
				}
			}
			// [ 샌드웜 --> 보병 ]
			else if (map[1][i][j] == 'S') {
				if (abs(sw1_obj.pos.row - i) < close_i && abs(sw1_obj.pos.column - j) < close_j) {
					close_i = sw1_obj.pos.row - i, close_j = sw1_obj.pos.row - j;
					POSITION new_dest = { i, j };
					sw1_obj.dest = new_dest;
				}
			}
			// [ 샌드웜 --> 투사 ]
			else if (map[1][i][j] == 'F') {
				if (abs(sw1_obj.pos.row - i) < close_i && abs(sw1_obj.pos.column - j) < close_j) {
					close_i = sw1_obj.pos.row - i, close_j = sw1_obj.pos.row - j;
					POSITION new_dest = { i, j };
					sw1_obj.dest = new_dest;
				}
			}
			// [ 샌드웜 --> 중전차 ]
			else if (map[1][i][j] == 'T') {
				if (abs(sw1_obj.pos.row - i) < close_i && abs(sw1_obj.pos.column - j) < close_j) {
					close_i = sw1_obj.pos.row - i, close_j = sw1_obj.pos.row - j;
					POSITION new_dest = { i, j };
					sw1_obj.dest = new_dest;
				}
			}
			// 유닛이 없을 때는 dest값을 원래 초기 값으로 초기화
			else {
				POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
				sw1_obj.dest = new_dest;
			}
		}
	}

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
		
		return sw1_obj.pos;
	}
	// [ 샌드웜(2)와 만났을 때 피해가기 (미완성) ]
	else if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] == 'W') {

		return sw1_obj.pos;
	}
	


	// 10% 확률로 배설 --> 스파이스 매장지 생성 (매장량 1 ~ 9 랜덤)
	int r = rand() % 100 + 1;
	if (r <= 10) {
		map[1][sw1_obj.pos.row][sw1_obj.pos.column] = '5';
	}

}

void sw1_move(void) {
	// 움직이는 주기
	if (sys_clock <= sw1_obj.next_move_time) {
		return;
	}
	// 샌드웜(1) layer1 (map[1])에 저장
	map[1][sw1_obj.pos.row][sw1_obj.pos.column] = -1;
	sw1_obj.pos = sw1_next_pos();
	map[1][sw1_obj.pos.row][sw1_obj.pos.column] = sw1_obj.repr;
	
	sw1_obj.next_move_time = sys_clock + sw1_obj.move_period;
}

// [ 샌드웜 (2) ]

POSITION sw2_next_pos(void) {

	// [ 목적지는 가장 가까운 유닛 ]

		// 가장 가까운 유닛을 찾아서 그 유닛의 좌표값을 dest에 지정. (같은 샌드웜끼리는 제외)

		// 유닛이 없을 때는 dest값을 원래 초기 값으로 초기화

		// 유닛과 만났을 때 (겹쳐졌을 때) 유닛과 전투 (잡아먹기)

		// 10% 확률로 배설 --> 스파이스 매장지 생성 (매장량 1 ~ 9 랜덤)

}

void sw2_move(void) {
	// 움직이는 주기

	// 샌드웜(2) layer1 (map[1])에 저장

}







