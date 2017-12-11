#include "spec.h"
#include <stdio.h>

#include "jmp_funcs.c"

#define BIT8
#include "add_funcs.c"
#include "cmp_funcs.c"
#include "imul_funcs.c"
#include "mov_funcs.c"
#include "pop_funcs.c"
#include "push_funcs.c"
#include "ret_funcs.c"
#include "sub_funcs.c"
#undef BIT8
#define BIT32
#include "add_funcs.c"
#include "cmp_funcs.c"
#include "imul_funcs.c"
#include "mov_funcs.c"
#include "pop_funcs.c"
#include "push_funcs.c"
#include "ret_funcs.c"
#include "sub_funcs.c"
#undef BIT32
#define BIT64
#include "add_funcs.c"
#include "cmp_funcs.c"
#include "imul_funcs.c"
#include "mov_funcs.c"
#include "pop_funcs.c"
#include "push_funcs.c"
#include "ret_funcs.c"
#include "sub_funcs.c"
#undef BIT64

void eval_01(char cmd) {
	if (REXW()) {
		add_01_64(cmd);
	}
	else {
		add_01_32(cmd);
	}
}

void eval_03(char cmd) {
	if (REXW()) {
		add_03_64(cmd);
	}
	else {
		add_03_32(cmd);
	}
}

void eval_05(char cmd) {
	if (REXW()) {
		add_05_64(cmd);
	}
	else {
		add_05_32(cmd);
	}
}

void eval_8b(char cmd);
void eval_0f(char cmd) {
	int cur = get_char();
	fprintf(stderr, "real %#04x\n", cur);
	if (cur >= 0x80 && cur <= 0x8f) {
		jc(cur, 0);
	}
	else if(cur == 0xaf) {
		imul_af(cur);
	}
	else if(cur == 0xbe) {
		eval_be(cur);
	}	
	else if(cur == 0xb6) {
		eval_b6(cur);
	}
	else { 
		fprintf(stderr, "%#04x\n", cur);
		is_end = 1;
	}
}

void eval_29(char cmd) {
	if (REXW()) {
		sub_29_64(cmd);
	}
	else {
		sub_29_32(cmd);
	}
}

void eval_2b(char cmd) {
	if (REXW()) {
		sub_2b_64(cmd);
	}
	else {
		sub_2b_32(cmd);
	}
}

void eval_2d(char cmd) {
	if (REXW()) {
		sub_2d_64(cmd);
	}
	else {
		sub_2d_32(cmd);
	}
}

void eval_39(char cmd) {
	if (REXW()) {
		cmp_39_64(cmd);
	}
	else {
		cmp_39_32(cmd);
	}
}

void eval_3b(char cmd) {
	if (REXW()) {
		cmp_3b_64(cmd);
	}
	else {
		cmp_3b_32(cmd);
	}
}

void eval_3d(char cmd) {
	if (REXW()) {
		cmp_3d_64(cmd);
	}
	else {
		cmp_3d_32(cmd);
	}
}


void eval_50(char cmd) {
	if (REXW()) {
		push_50_64(cmd);
	}
	else {
		push_50_32(cmd);
	}
}

void eval_58(char cmd) {
	if (REXW()) {
		pop_58_64(cmd);
	}
	else {
		pop_58_32(cmd);
	}
}

void eval_68(char cmd) {
	if (REXW()) {
		push_68_64(cmd);
	}
	else {
		push_68_32(cmd);
	}
}

void eval_80(char cmd) {
	char f = read_reg();
	switch(f) {
		case 0:
			add_81_8(cmd);
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
			cmp_81_8(cmd);
			break;
	}
}

void eval_81(char cmd) {
	char f = read_reg();
	switch(f) {
		case 0:
			if (REXW()) {
				add_81_64(cmd);
			}
			else {
				add_81_32(cmd);
			}
			break;
		//case 1:
		//case 2:
		//case 3:
		//case 4:
		case 5:
			if (REXW()) {
				sub_81_64(cmd);
			}
			else {
				sub_81_32(cmd);
			}
			break;
		//case 6:
		case 7:
			if (REXW()) {
				cmp_81_64(cmd);
			}
			else {
				cmp_81_32(cmd);
			}
		break;
	}
}

void eval_83(char cmd) {
	char f = read_reg();
	switch(f) {
		case 0:
			if (REXW()) {
				add_83_64(cmd);
			}
			else {
				add_83_32(cmd);
			}
			break;
		//case 1:
		//case 2:
		//case 3:
		//case 4:
		case 5:
			if (REXW()) {
				sub_83_64(cmd);
			}
			else {
				sub_83_32(cmd);
			}
			break;
		//case 6:
		case 7:
			if (REXW()) {
				cmp_83_64(cmd);
			}
			else {
				cmp_83_32(cmd);
			}
		break;
	}
}

void eval_85(char cmd) {
	if (REXW()) {
		test_85_64(cmd);
	}
	else {
		test_85_32(cmd);
	}
}

void eval_89(char cmd) {
	if (REXW()) {
		mov_89_64(cmd);
	}
	else {
		mov_89_32(cmd);
	}
}

void eval_8b(char cmd) {
	if (REXW()) {
		mov_8b_64(cmd);
	}
	else {
		mov_8b_32(cmd);
	}
}

void eval_8d(char cmd) {
	if (REXW()) {
		lea_8d_64(cmd);
	}
	else {
		lea_8d_32(cmd);
	}
}

void eval_8f(char cmd) {
	char f = read_reg();
	switch(f) {
		case 0:
			if (REXW()) {
				pop_8f_64(cmd);
			}
			else {
				pop_8f_32(cmd);
			}
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		break;
	}
}

void eval_b8(char cmd) {
	if (REXW()) {
		mov_b8_64(cmd);
	}
	else {
		mov_b8_32(cmd);
	}
}

void eval_be(char cmd) {
	if (REXW()) {
		mov_be_64(cmd);
	}
	else {
		mov_be_32(cmd);
	}
}

void eval_b6(char cmd) {
	if (REXW()) {
		mov_b6_64(cmd);
	}
	else {
		mov_b6_32(cmd);
	}
}

void eval_c7(char cmd) {
	if (REXW()) {
		mov_c7_64(cmd);
	}
	else {
		mov_c7_32(cmd);
	}
}

void eval_ff(char cmd) {
	char f = read_reg();
	switch(f) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			if (REXW()) {
				push_ff_64(cmd);
			}
			else {
				push_ff_32(cmd);
			}
		case 7:
		break;
	}
}

void jc_sh(char cmd) {
	jc(cmd + 0x10, 1);
}

void init(void *(**a)()) {
	a[0x00] = add_01_8;
	a[0x01] = eval_01;
	a[0x02] = add_03_8;
	a[0x03] = eval_03;
	a[0x04] = add_05_8;
	a[0x05] = eval_05;

	a[0x0f] = eval_0f;

	a[0x28] = sub_29_8;
	a[0x29] = eval_29;
	a[0x2a] = sub_2b_8;
	a[0x2b] = eval_2b;
	a[0x2c] = sub_2d_8;
	a[0x2d] = eval_2d;

	a[0x38] = cmp_39_8;
	a[0x39] = eval_39;
	a[0x3a] = cmp_3b_8;
	a[0x3b] = eval_3b;
	a[0x3c] = cmp_3d_8;
	a[0x3d] = eval_3d;

	a[0x50] = eval_50;
	a[0x51] = eval_50;
	a[0x52] = eval_50;
	a[0x53] = eval_50;
	a[0x54] = eval_50;
	a[0x55] = eval_50;
	a[0x56] = eval_50;
	a[0x57] = eval_50;
	a[0x58] = eval_58;
	a[0x59] = eval_58;
	a[0x5a] = eval_58;
	a[0x5b] = eval_58;
	a[0x5c] = eval_58;
	a[0x5d] = eval_58;
	a[0x5e] = eval_58;
	a[0x5f] = eval_58;

	a[0x63] = movsxd_63;

	a[0x68] = eval_68;
	a[0x6a] = push_68_8;

	a[0x70] = jc_sh;
	a[0x71] = jc_sh;
	a[0x72] = jc_sh;
	a[0x73] = jc_sh;
	a[0x74] = jc_sh;
	a[0x75] = jc_sh;
	a[0x76] = jc_sh;
	a[0x77] = jc_sh;
	a[0x78] = jc_sh;
	a[0x79] = jc_sh;
	a[0x7a] = jc_sh;
	a[0x7b] = jc_sh;
	a[0x7c] = jc_sh;
	a[0x7d] = jc_sh;
	a[0x7e] = jc_sh;
	a[0x7f] = jc_sh;

	a[0x80] = eval_80;
	a[0x81] = eval_81;

	a[0x83] = eval_83;
	a[0x84] = test_85_8;
	a[0x85] = eval_85;

	a[0x88] = mov_89_8;
	a[0x89] = eval_89;

	a[0x8a] = mov_8b_8;
	a[0x8b] = eval_8b;

	a[0x8d] = eval_8d;

	a[0x8f] = eval_8f;

	a[0xb0] = mov_b8_8;
	a[0xb1] = mov_b8_8;
	a[0xb2] = mov_b8_8;
	a[0xb3] = mov_b8_8;
	a[0xb4] = mov_b8_8;
	a[0xb5] = mov_b8_8;
	a[0xb6] = mov_b8_8;
	a[0xb7] = mov_b8_8;

	a[0xb8] = eval_b8;
	a[0xb9] = eval_b8;
	a[0xba] = eval_b8;
	a[0xbb] = eval_b8;
	a[0xbc] = eval_b8;
	a[0xbd] = eval_b8;
	a[0xbe] = eval_b8;
	a[0xbf] = eval_b8;

	a[0xc3] = ret_c3_64;

	a[0xc6] = mov_c7_8;
	a[0xc7] = eval_c7;

	a[0xc9] = leave_c9;

	a[0xe8] = call_e8;
	a[0xe9] = jmp_e9;

	a[0xeb] = jmp_eb;


	a[0xff] = eval_ff;

}