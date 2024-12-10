<h1 align="center"> Dune 1.5 </h1>

* 고급프로그래밍 실습 과제

<br>

## 게임 화면
* 아군(아트레이디스)진영은 아래에 위치
* 적군(하코넨)진영은 위에 위치

<img width="806" alt="스크린샷 2024-12-10 200240" src="https://github.com/user-attachments/assets/58d30fb0-9644-4def-a9bb-24db7206ec05">

<br><br>

## 건물 설명
### 아군(아트레이디스) 건물
* 본진(B) : 메인 건물, 하베스터 생산 (파란색)
* 장판(P) : 숙소, 창고, 병영, 은신처 건설 가능
* 숙소(D) : 설치시 인구 최대치 10 증가
* 창고(G) : 설치시 스파이스 보관 최대치 10 증가
* 병영(B) : 보병 생산
* 은신처(S) : 특수유닛 프레멘 생산
### 적군(하코넨) 건물
* 본진(B) : 메인 건물, 하베스터 생산 (빨간색)
* 장판(P) : 숙소, 창고, 병영, 은신처 건설 가능
* 숙소(D) : 설치시 인구 최대치 10 증가
* 창고(G) : 설치시 스파이스 보관 최대치 10 증가
* 투기장(A) : 투사 생산
* 공장(F) : 특수유닛 중전차 생산
### 장애물
* 바위(R) : 아무 효과 없음


## 유닛 설명
* 하베스터(H) : 스파이스 자원 수확
* 프레멘(F) : 아군 특수유닛 
* 보병(S) : 아군 일반유닛
* 투사(F) : 적군 일반 유닛
* 중전차(T) : 적군 특수유닛
* 샌드웜(W) : 중립유닛

<br>

## 샌드웜 가까운 유닛으로 이동

``` C
// ====================================[ MAIN CODE ]==================================
double check_close = 61.0;
int move_i = 2, move_j = 4;
for (int i = 1; i < MAP_HEIGHT - 1; i++) {
	for (int j = 1; j < MAP_WIDTH - 1; j++) {
		if (map[1][i][j] == 'H' || map[1][i][j] == 'S' || (map[1][i][j] == 'F')) {
			if (sqrt(pow(i - sw1_obj.pos.row, 2) + pow(j - sw1_obj.pos.column, 2)) < check_close) {
				check_close = sqrt(pow(i - sw1_obj.pos.row, 2) + pow(j - sw1_obj.pos.column, 2));
				move_i = i, move_j = j;
			}
		}
	}
}
POSITION new_dest = { move_i, move_j };
sw1_obj.dest = new_dest;

```

``` C
// ====================================[ PARTITION CODE ]==================================

// (1, 1)와 (MAP_HEIGHT - 1, MAP_WIDTH - 1) | 두점 사이의 최대 길이
// 두 점 사이의 최대 길이를 check_close에 저장
double check_close = 61.0;

// 샌드웜(1)의 초기 좌표값을 두 변수에 저장
int move_i = 2, move_j = 4;
```

``` C
// 현재 샌드웜의 좌표와 Layer1에 존재하는 유닛들의 좌표 | 두 좌표 사이의 거리를 구한다.
// 두 좌표 사이의 거리와 check_close와 비교하여 가장 가까운 유닛의 행렬을 move_i, move_j에 저장
for (int i = 1; i < MAP_HEIGHT - 1; i++) {
  for (int j = 1; j < MAP_WIDTH - 1; j++) {
    if (map[1][i][j] == 'H' || map[1][i][j] == 'S' || map[1][i][j] == 'F') {
      if (sqrt(pow(i - sw1_obj.pos.row, 2) + pow(j - sw1_obj.pos.column, 2)) < check_close) {
        check_close = sqrt(pow(i - sw1_obj.pos.row, 2) + pow(j - sw1_obj.pos.column, 2));
        move_i = i, move_j = j;
      }
    }
  }
}

```

``` C
// new_dest에 move_i, move_j를 저장하고 샌드웜(1)의 목적지는 new_dest로 변경
POSITION new_dest = { move_i, move_j };
sw1_obj.dest = new_dest;
```



