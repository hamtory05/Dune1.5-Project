/*
* raw(?) I/O
*/
#include "io.h"

// [ �ܼ� ��ġ ���� ]
void gotoxy(POSITION pos) {
	COORD coord = { pos.column, pos.row }; // ��, �� �ݴ�� ����
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// [ ���� ���� ]
void set_color(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// [ ���� ��� ]
void printc(POSITION pos, char ch, int color) {
	if (color >= 0) {
		set_color(color);
	}
	gotoxy(pos);
	printf("%c", ch);
}

// [ ���ڿ� ��� ]
void prints(POSITION pos, char str[100]) {
	set_color(15);
	gotoxy(pos); 
	printf("%s", str);
}

// [ Ű Ȯ�� �� �ൿ ]
KEY get_key(void) {
	if (!_kbhit()) {  // �Էµ� Ű�� �ִ��� Ȯ��
		return k_none;
	}
	
	int byte = _getch();    // �Էµ� Ű�� ���� �ޱ�

	switch (byte) {
	case 'q': return k_quit;  // 'q'�� ������ ����

	// [ �����̽��ٸ� ���� ���� �ൿ ]
	case 32:
		return k_space;

	// [ ESCŰ ���� ���� �ൿ ]
	case 27:
		return k_esc;

	// [ HŰ�� ���� ���� �ൿ ]
	case 72:
		return k_h;
	case 104:
		return k_h;
	
	// [ BŰ�� ���� ���� �ൿ ]
	case 66:
		return k_b;
	case 98:
		return k_b;

	// [ PŰ�� ���� ���� �ൿ ]
	case 80:
		return k_p;
	case 112:
		return k_p;

	// [ ����Ű �Էµ��� ���� �ൿ ]
	case 224:
		byte = _getch();  // MSB 224�� �Է� �Ǹ� 1����Ʈ �� ���� �ޱ�
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
