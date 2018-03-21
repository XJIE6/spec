#include <stdio.h>
#include <limits.h>
#include <sys/mman.h>
#include <errno.h>
//#include <curses.h>
#include "spec.h"
#include "state.h"

unsigned char REX;
state* st;
char* res;
char* last; 

const int MAX_MEM_SIZE = 10;
const int MAX_STACK_SIZE = 1000000;

#define RIP st->regs[16]

// #define EAX egs[0]
// #define ECX egs[2]
// #define EDX egs[4]
// #define EBX egs[6]
// #define ESP egs[8]
// #define EBP egs[10]
// #define ESI egs[12]
// #define EDI egs[14]

// #define EIP egs[32]

// #define RAX regs[0]
// #define RCX regs[1]
// #define RDX regs[2]
// #define RBX regs[3]
#define RSP st->regs[4]
#define RBP st->regs[5]
// #define RSI regs[6]
// #define RDI regs[7]

// #define R8  regs[8]
// #define R9  regs[9]
// #define R10 regs[10]
// #define R11 regs[11]
// #define R12 regs[12]
// #define R13 regs[13]
// #define R14 regs[14]
// #define R15 regs[15]

// #define RIP regs[16]
// #define FLAGS regs[17]

#define ZF (1 << 6)
#define SF (1 << 7)
#define OF (1 << 11)

void my_clf() {
	st->flags[0] = 0;
	st->flags[6] = 0;
	st->flags[7] = 0;
	st->flags[11] = 0;
}

char is_end;

void calc_hash(state* cur) {
	int mod = 1000003;
	int mul = 1009;
	int res = 0;

	for (int i = 0; i < 16; ++i) {
		if (!cur->info_regs[i].is_dynamic) {
			res *= mul;
			res %= mod;
			res += cur->regs[i] % mod;
			res %= mod;

			res *= mul;
			res %= mod;
			res += cur->info_regs[i].mem % mod;
			res %= mod;
		}
	}
	if (!cur->info_flags.is_dynamic) {
		for (int i = 0; i < 64; ++i) {
			res *= mul;
			res %= mod;
			res += cur->flags[i] % mod;
			res %= mod;
		}
	}

	for (int j = -cur->mem_mem_len[0]; j < 0; ++j) {
		if (!cur->info_mem[0][j].is_dynamic) {
			res *= mul;
			res %= mod;
			res += cur->mem[0][j] % mod;
			res %= mod;

			res *= mul;
			res %= mod;
			res += cur->info_mem[0][j].mem % mod;
			res %= mod;
		}
	}

	for (int i = 1; i < cur->mem_len; ++i) {
		for (int j = 0; j < cur->mem_mem_len[i]; ++j) {
			if (!cur->info_mem[i][j].is_dynamic) {
				res *= mul;
				res %= mod;
				res += cur->mem[i][j] % mod;
				res %= mod;

				res *= mul;
				res %= mod;
				res += cur->info_mem[i][j].mem % mod;
				res %= mod;
			}
		}
	}
	cur->hash = res;
} 

state* new_state() {
	state* cur = malloc(sizeof(state));
	for (int i = 0; i < 17; ++i) {
		cur->regs[i] = 0;
		cur->info_regs[i].is_dynamic = 0;
		cur->info_regs[i].mem = -1;
	}
	for (int i = 0; i < 64; ++i) {
		cur->flags[i] = 0;
	}
	cur->info_flags.is_dynamic = 0;
	cur->info_flags.mem = -1;

	cur->mem_len = 1;
	cur->mem_mem_len[0] = MAX_STACK_SIZE;

	cur->mem = malloc(sizeof(char*) * MAX_MEM_SIZE);
	cur->info_mem = malloc(sizeof(info*) * MAX_MEM_SIZE);
	cur->mem[0] = malloc(sizeof(char) * MAX_STACK_SIZE) + MAX_STACK_SIZE;
	cur->info_mem[0] = malloc(sizeof(info) * MAX_STACK_SIZE) + MAX_STACK_SIZE * sizeof(info);
	for (int i = -MAX_STACK_SIZE; i < 0; ++i) {
		cur->mem[0][i] = 0;
		cur->info_mem[0][i].mem = -1;
		cur->info_mem[0][i].is_dynamic = 0;
	}
	cur->info_regs[4].mem = 0;
	cur->info_regs[5].mem = 0;
	cur->next = NULL;
	cur->hash = 0;
	return cur;
}

state* copy(state* cur) {
	state* new = new_state();
	for (int i = 0; i < 17; ++i) {
		new->regs[i] = cur->regs[i];
		new->info_regs[i].mem = cur->info_regs[i].mem;
		new->info_regs[i].is_dynamic = cur->info_regs[i].is_dynamic;
	}

	for (int i = 0; i < 64; ++i) {
		new->flags[i] = cur->flags[i];
	}

	new->info_flags.is_dynamic = cur->info_flags.is_dynamic;

	new->mem_len = cur->mem_len;
	for (int i = 0; i < cur->mem_len; ++i) {
		new->mem_mem_len[i] = cur->mem_mem_len[i];
	}

	int k = 1; 
	while (cur->mem_mem_len[k]) {
		new->mem[k] = malloc(cur->mem_mem_len[k]);
		new->info_mem[k] = malloc(sizeof(info) * cur->mem_mem_len[k]);
		k++;
	}
	for (int j = -cur->mem_mem_len[0]; j < 0; ++j) {
		new->mem[0][j] = cur->mem[0][j];
		new->info_mem[0][j].mem = cur->info_mem[0][j].mem;
		new->info_mem[0][j].is_dynamic = cur->info_mem[0][j].is_dynamic;
	}
	for (int i = 1; i < cur->mem_len; ++i) {
		for (int j = 0; j < cur->mem_mem_len[i]; ++j) {
			new->mem[i][j] = cur->mem[i][j];
			new->info_mem[i][j].mem = cur->info_mem[i][j].mem;
			new->info_mem[i][j].is_dynamic = cur->info_mem[i][j].is_dynamic;
		}
	}
	cur->next = new;
}

void print_reg(int i) {
	switch(i) {
		case(0):
			fprintf(stderr, "rax");
			break;
		case(1):
			fprintf(stderr, "rcx");
			break;
		case(2):
			fprintf(stderr, "rdx");
			break;
		case(3):
			fprintf(stderr, "rbx");
			break;
		case(4):
			fprintf(stderr, "rsp");
			break;
		case(5):
			fprintf(stderr, "rbp");
			break;
		case(6):
			fprintf(stderr, "rsi");
			break;
		case(7):
			fprintf(stderr, "rdi");
			break;
	}
}

unsigned char bit3_5(unsigned char b) {
	return (b & (7 << 3)) >> 3;
}

unsigned char bit1_2(unsigned char b) {
	return (b & (3 << 6)) >> 6;
}

unsigned char bit6_8(unsigned char b) {
	return b & 7;
}

unsigned char REXW() {
	return (REX & 8) >> 3;
}

unsigned char REXR() {
	return (REX & 4) >> 2;
}

unsigned char REXX() {
	return (REX & 2) >> 1;
}

unsigned char REXB() {
	return REX & 1;
}

unsigned char get_char() {
	unsigned char res = *((unsigned char*)RIP);
	RIP += sizeof(unsigned char);
	//fprintf(stderr, "read %#04x\n", res);
	return res;
}

char read_reg() {
	unsigned char res = *((unsigned char*)RIP);
	return bit3_5(res);
}

char get_schar() {
	char res = *((char*)RIP);
	RIP += sizeof(char);
	//fprintf(stderr, "read %#04x\n", res);
	return res;
}

int int_8S() {
	return get_schar();
}

int int_8() {
	return get_char();
}

int int_16S() {
	int res = get_char();
	res += get_schar() << 8;
	return res;
}

int int_16() {
	int res = get_char();
	res += get_char() << 8;
	return res;
}

int int_32S() {
	int res = get_char();
	res += get_char() << 8;
	res += get_char() << 16;
	res += get_schar() << 24;
	return res;
}

unsigned int int_32() {
	int res = get_char();
	res += get_char() << 8;
	res += get_char() << 16;
	res += get_char() << 24;
	return res;
}

long long int_64S() {
	long res = get_char();
	res += get_char() << 8;
	res += get_char() << 16;
	res += get_char() << 24;
	res += (long long) get_char() << 32;
	res += (long long) get_char() << 40;
	res += (long long) get_char() << 48;
	res += (long long) get_schar() << 56;
	return res;
}

unsigned long long int_64() {
	long res = get_char();
	res += get_char() << 8;
	res += get_char() << 16;
	res += get_char() << 24;
	res += (long long) get_char() << 32;
	res += (long long) get_char() << 40;
	res += (long long) get_char() << 48;
	res += (long long) get_char() << 56;
	return res;
}

unsigned char cur, mod, reg, r_m, scale, _index, is_dynamic;
long long base;

value v;
param p1, p2;

char f;

void set_f() {
	f = 1;
}

void print_state() {
	return;
	fprintf(stderr, "RIP = val: %lld\n", st->regs[16]);
	for (int i = 0; i < 8; ++i) {
		fprintf(stderr, "%d) val: %lld, mem: %d, dyn: %d\n", i, st->regs[i], st->info_regs[i].mem, st->info_regs[i].is_dynamic);
	}
	for (int i = 0; i < 250; i += 4) {
		fprintf(stderr, "%d) val: %lld, mem: %d, dyn: %d\n", i, (long long)st->mem[0][-i], (int)st->info_mem[0][-i].mem, (int)st->info_mem[0][-i].is_dynamic);
	}
}

void print_params() {
	return;
	fprintf(stderr, "p1: reg1 = %d, reg2 = %d, base = %lld, scale = %d\n", (int) p1.reg1, (int) p1.reg2, (long long) p1.base, (int) p1.scale);
	fprintf(stderr, "p2: reg1 = %d, reg2 = %d, base = %lld, scale = %d\n", (int) p2.reg1, (int) p2.reg2, (long long) p2.base, (int) p2.scale);	
}

void print_value() {
	return;
	fprintf(stderr, "v: base = %lld, mem = %d, is_dynamic = %d\n", (long long) v.base, (int) v.mem, (int) is_dynamic);		
}

void write_byte(char byte) {
	*last = byte;
	//fprintf(stderr, "write_byte %02X\n", *last);
	last++;
}

void write_int(int value) {
	*((int *)last) = value;
	//fprintf(stderr, "write_byte %02X\n", *last);
	last += 4;
}

void write_params() {
	if (p1.reg2 != -1 || p1.scale != -1 || p1.base != 0) {
		fprintf(stderr, "ERROR print_params wrong p1\n");
		is_end = 1;
		return;
	}
	if ((p2.scale != 0 && p1.scale != -1) || p2.reg2 != -1) {
		fprintf(stderr, "ERROR print_params wrong p2\n");
		is_end = 1;
		return;
	}
	//fprintf(stderr, "p1: reg1 = %d, reg2 = %d, base = %lld, scale = %d\n", (int) p1.reg1, (int) p1.reg2, (long long) p1.base, (int) p1.scale);
	//fprintf(stderr, "p2: reg1 = %d, reg2 = %d, base = %lld, scale = %d\n", (int) p2.reg1, (int) p2.reg2, (long long) p2.base, (int) p2.scale);	

	if (p2.scale == 0) {
		if (p2.base == 0) {
			*last = (p1.reg1 << 3) + p2.reg1;
			//fprintf(stderr, "param1 %02X\n", *last);
			last++;
			return;
		}
		else if (p2.base < (1 << 8) && p2.base > -(1 << 8)) {
			*last = (1 << 6) + (p1.reg1 << 3) + p2.reg1;
			//fprintf(stderr, "param1 %02X\n", *last);
			last++;
			*last = p2.base;
			//fprintf(stderr, "param2 %02X\n", *last);
			last++;
			return;
		}
		else {
			*last = (2 << 6) + (p1.reg1 << 3) + p2.reg1;
			//fprintf(stderr, "param1 %02X\n", *last);
			last++;
			*((int*)last) = p2.base;
			//fprintf(stderr, "param2 %02X\n", *last);
			last += 4;
		}
	}
	if (p2.scale == -1) {
		*last = (3 << 6) + (p1.reg1 << 3) + p2.reg1;
		//fprintf(stderr, "param1 %02X\n", *last);
		last++;
	}

}

int u_len = 1000000;
char u[1000000];

int used(long long a) {
	return u[((a % u_len) + u_len) % u_len];
}

void use(long long a) {
	u[((a % u_len) + u_len) % u_len] = 1;
}

int (*cmd[256])();
state* que;

#define BIT8
#include "spec_funcs.c"
#undef BIT8
#define BIT32
#include "spec_funcs.c"
#undef BIT32
#define BIT64
#include "spec_funcs.c"
#undef BIT64
int i = 0;

char* spec(state* _st) {
	res = mmap(NULL, 4096, PROT_EXEC | PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
	//res = malloc(1000);
	fprintf(stderr, "res = %d\n", (int) res);
	if (res == -1) {
		fprintf(stderr, "%d", errno);
		return 0;
	}
	last = res;
	int xxx = 0;
	for (int i = 0; i < u_len; ++i) {
		u[i] = 0;
	}
	init(cmd);
	que = _st;
	unsigned char cur;
	while (que != NULL) {
		st = que;
		que = que->next;
		st->next = NULL;
		if (used(st->hash)) {
			fprintf(stderr, "USED\n");
			xxx++;
			continue;
		}
		//getchar();
		use(st->hash);
		fprintf(stderr, "\n\nStart block %d\n", st->hash % u_len);
		is_end = 0;
		while(1) {
			REX = 0;
			cur = get_char();
			if (cur >= 0x40 && cur <= 0x4f) {
				REX = cur;
				cur = get_char();
			}
			// fprintf(stderr, "HERE\n");
			// for (int i = 0; i < 17; ++i) {
			// 	fprintf(stderr, "r %lld %lld\n", i, st->regs[i]);
			// }
			// for (int i = 0; i < 10; ++i) {
			// 	fprintf(stderr, "m %d %d\n", i, st->mem[0][1000 - i]);
			// }
			//fprintf(stderr, "%d cmd %#04x\n", i, cur);
			i++;
			//fprintf(stderr, "RIP = val: %lld\n", st->regs[16]);
			print_state();
			cmd[cur](cur);
			if (st->next != NULL) {
				st->next->next = que;
				que = st->next;
				st->next = NULL;
			}
			if (is_end) {
				break;
			}
		}
	}
	fprintf(stderr, "USED: %d\n", xxx);
	return res;
}