/*
* raw(?) I/O
*/
#include "io.h"

// [ 콘솔 위치 설정 ]
void gotoxy(POSITION pos) {
	COORD coord = { pos.column, pos.row }; // 행, 열 반대로 전달
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// [ 색상 설정 ]
void set_color(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// [ 문자 출력 ]
void printc(POSITION pos, char ch, int color) {
	if (color >= 0) {
		set_color(color);
	}
	gotoxy(pos);
	printf("%c", ch);
}

// [ 문자열 출력 ]
void prints(POSITION pos, char str[100]) {
	set_color(15);
	gotoxy(pos); 
	printf("%s", str);
}

// [ 키 확인 및 행동 ]
KEY get_key(void) {
	if (!_kbhit()) {  // 입력된 키가 있는지 확인
		return k_none;
	}
	
	int byte = _getch();    // 입력된 키를 전달 받기

	switch (byte) {
	case 'q': return k_quit;  // 'q'를 누르면 종료

	// [ 스페이스바를 누를 때의 행동 ]
	case 32:
		return k_space;

	// [ ESC키 누를 때의 행동 ]
	case 27:
		return k_esc;

	// [ H키를 누를 때의 행동 ]
	case 72:
		return k_h;
	case 104:
		return k_h;
	
	// [ B키를 누를 때의 행동 ]
	case 66:
		return k_b;
	case 98:
		return k_b;

	// [ P키를 누를 때의 행동 ]
	case 80:
		return k_p;
	case 112:
		return k_p;

	// [ 방향키 입력됐을 때의 행동 ]
	case 224:
		byte = _getch();  // MSB 224가 입력 되면 1바이트 더 전달 받기
		switch (byte) {
		case 72: return k_up;
		case 75: return k_left;
		case 77: return k_right;
		case 80: return k_down;
		default: return k_undef;
		}
	default: return k_undef;
	}
}
