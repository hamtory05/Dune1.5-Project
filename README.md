<h1 align="center"> 👑Dune 1.5👑 </h1>
<p2> 🪛고급 프로그래밍 Dune 1.5 과제 </p2>

<br>

<h3 align="center"> ⚙️샌드웜 가까운 유닛으로 이동 </h3>

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
