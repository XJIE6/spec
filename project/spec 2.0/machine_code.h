#pragma once

typedef struct _code {
    char prefix;
    char number; 
    param p1;
    param p2;
    long long address;
    struct _code* next;
} code;

typedef struct _code_block {
	long long hash;
	code* instructions;
	struct _code_block* next;
} code_block;

code* read_instruction(state* state);
void write_instruction(code instruction, char* place);
void* write_blocks(code_block* blocks);
int size(code* instructions);

