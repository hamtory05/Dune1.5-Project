#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>
#include <conio.h>
#include <assert.h>

/* ================= system parameters =================== */
#define TICK 10		// time unit(ms)

#define N_LAYER 3
#define MAP_WIDTH	60
#define MAP_HEIGHT	18

#define STATE_WIDTH 50
#define STATE_HEIGHT 18

#define SYSMES_WIDTH 60
#define SYSMES_HEIGHT 9

#define ORDER_WIDTH 50
#define ORDER_HEIGHT 9

#define MAX_HAV  50   // 하베스터 최대 50마리 까지 보관 가능
#define MAX_SOLD 50   // 보병 최대 50마리 까지 보관 가능
#define MAX_FREM 50   // 프레멘 최대 50마리 까지 보관 가능
#define MAX_FIGHTER 50
#define MAX_TANK 50

#define MAX_SPICE 100
#define MAX_DOR  50
#define MAX_GAR  50
#define MAX_BAR  50
#define MAX_SHE  50
#define MAX_ARE  50
#define MAX_FAC  50


/* ================= 위치와 방향 =================== */
// 맵에서 위치를 나타내는 구조체
typedef struct {
	int row, column;
} POSITION;

// 커서 위치
typedef struct {
	POSITION previous;  // 직전 위치
	POSITION current;   // 현재 위치
} CURSOR;

// 입력 가능한 키 종류.
// 수업에서 enum은 생략했는데, 크게 어렵지 않으니 예제 검색
typedef enum {
	// k_none: 입력된 키가 없음. d_stay(안 움직이는 경우)에 대응
	k_none = 0, k_up, k_right, k_left, k_down,
	k_quit,
	k_space,
	k_esc,
	k_h, k_b, k_p, k_d, k_g, k_s, k_m, k_f,
	k_undef, // 정의되지 않은 키 입력	
} KEY;


// DIRECTION은 KEY의 부분집합이지만, 의미를 명확하게 하기 위해서 다른 타입으로 정의
typedef enum {
	d_stay = 0, d_up, d_right, d_left, d_down
} DIRECTION;


/* ================= 위치와 방향(2) =================== */
// 편의성을 위한 함수들. KEY, POSITION, DIRECTION 구조체들을 유기적으로 변환

// 편의성 함수
inline POSITION padd(POSITION p1, POSITION p2) {
	POSITION p = { p1.row + p2.row, p1.column + p2.column };
	return p;
}

// p1 - p2
inline POSITION psub(POSITION p1, POSITION p2) {
	POSITION p = { p1.row - p2.row, p1.column - p2.column };
	return p;
}

// 방향키인지 확인하는 함수
#define is_arrow_key(k)		(k_up <= (k) && (k) <= k_down)

// 화살표 '키'(KEY)를 '방향'(DIRECTION)으로 변환. 정수 값은 똑같으니 타입만 바꿔주면 됨
#define ktod(k)		(DIRECTION)(k)

// DIRECTION을 POSITION 벡터로 변환하는 함수
inline POSITION dtop(DIRECTION d) {
	static POSITION direction_vector[] = { {0, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 0} };
	return direction_vector[d];
}

inline POSITION dtop2(DIRECTION d) {
	static POSITION direction_vector[] = { {0, 0}, {-3, 0}, {0, 3}, {0, -3}, {3, 0} };
	return direction_vector[d];
}

// p를 d 방향으로 이동시킨 POSITION
#define pmove(p, d)		(padd((p), dtop(d)))

#define pmove2(p, d)	(padd((p), dtop2(d)))

/* ================= game data =================== */

// [ 스파이스, 인구 ]
typedef struct {
	int spice;		// 현재 보유한 스파이스
	int spice_max;  // 스파이스 최대 저장량
	int population; // 현재 인구 수
	int population_max;  // 수용 가능한 인구 수
} RESOURCE;


// [ OBJ ]
typedef struct {
	POSITION pos;		// 현재 위치(position)
	POSITION dest;		// 목적지(destination)
	char repr;			// 화면에 표시할 문자(representation)	
	int move_period;	// '몇 ms마다 한 칸 움직이는지'를 뜻함
	int next_move_time;	// 다음에 움직일 시간
	int color;			// 색깔
	int hp;				// 체력
	int dps;			// 공격력
	int attack_period;	// 공격 주기
	int next_attack_time; // 다음 공격할 시간
	int eyes;			// 시야
	int gather_time;    // 하베스터 수확 시간
	int gather_next_time; // 하베스터 다음 수확 시간 추가
	POSITION patrol_point; // 순찰 목적지
	bool is_patrolling; // 순찰 활성화 여부
	POSITION original_dest;  // 원래 목적지를 저장
	bool e_base_attack; // 적군 본진 공격
	bool e_dor_attack; // 적군 숙소 공격
	bool e_gar_attack; // 적군 창고 공격
	bool e_arena_attack; // 적군 투기장 공격
	bool e_factory_attack; // 적군 공장 공격
	bool enemy_found; // 적군 찾기
} OBJECT_SAMPLE;
 

// [ 건물 배치 ]
typedef struct {
	POSITION pos1; // 건물 크기 1
	POSITION pos2; // 건물 크기 2
	POSITION pos3; // 건물 크기 3
	POSITION pos4; // 건물 크기 4
	char repr;     // 건물 이름
	int layer;     // 몇번 째 레이어에 들어가는건지 구분 <-- 필요하면 사용
	int hp;        // 건물 내구도
} OBJECT_BUILDING;

// [ 모래 폭풍 ]
typedef struct {
	POSITION pos1;
	POSITION pos2;
	POSITION pos3;
	POSITION pos4;
	POSITION dest1;
	POSITION dest2;
	POSITION dest3; 
	POSITION dest4; 
	char repr;
	int move_period;
	int next_move_time;
	int color;
} SAND_WIND;





#endif
