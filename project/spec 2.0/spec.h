#include "state.h"

#pragma once

typedef struct _param {
	char reg1;
	char reg2;
	long long base;
	char scale;
} param;

typedef struct _value {
	long long base;
	char mem;
} value;

extern unsigned char is_dynamic;
extern value v;

extern char is_end;

extern state* st;

int spec(state* _st);
void assign_8(param* p);
void assign_32(param* p);
void assign_64(param* p);
void eval_8(param* p);
void eval_32(param* p);
void eval_64(param* p);
void prefix(param* p);
void print(param* p);
unsigned char REXW();
void parce_reg_mem_8();
void parce_reg_mem_32();
void parce_reg_mem_64();
extern param p1, p2;