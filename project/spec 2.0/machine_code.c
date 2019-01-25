#include "machine_code.h"

code* read_instruction(state* state) {
	fprintf(stderr, "NOT IMPLEMENTED\n");
}
void write_instruction(code instruction, char* place) {
	fprintf(stderr, "NOT IMPLEMENTED\n");
}
void* write_blocks(code_block* blocks) {
	fprintf(stderr, "NOT IMPLEMENTED\n");
}
int size(code* instructions) {
	fprintf(stderr, "NOT IMPLEMENTED\n");



void sib(state* st, code* instruction, char mod) {
    unsigned char cur = get_char(st);
    unsigned char scale = bit1_2(cur);
    unsigned char index = bit3_5(cur) + ((REXX(instruction->REX) << 3));
    long long base = bit6_8(cur);
    if (base == 5) {
        base += REXB(instruction->REX) << 3;
        switch (mod) {
            case 0:
                instruction->p2 = {.reg1 = index, .reg2 = -1, .scale = scale, .base = int_32S(st)};
            break;

            case 1:
                instruction->p2 = {.reg1 = base, .reg2 = index, .scale = scale, .base = int_8S(st)};
            break;

            case 2:
               instruction->p2 = {.reg1 = base, .reg2 = index, .scale = scale, .base = int_32S(st)};
            break;

            default:
                fprintf(stderr, "ERROR mod 2\n");
        }
    }
    else if (index == 4) {
        base += REXB(instruction->rex) << 3;
        instruction->p2 = {.reg1 = base, .reg2 = -1, .scale = scale, .base = 0};
    }
    else {
        base += REXB(instruction->rex) << 3;
        instruction->p2 = {.reg1 = base, .reg2 = index, .scale = scale, .base = 0};
    }
}

void parce_reg_mem(state* st, code* instruction) {
    unsigned char cur = get_char(st);
    unsigned char mod = bit1_2(cur);
    unsigned char reg = bit3_5(cur) + (REXR(instruction->REX) << 3);
    unsigned char r_m = bit6_8(cur);
    long long base;
    
    instruction->p1 = {.reg1 = reg, .reg2 = -1, .scale = -1, .base = 0};

    switch(mod) {
        case 0:
            if (r_m == 4) {
                sib(st, instruction, mod);
                return;
            }
            if (r_m == 5) {
                base = int_32S(st);
                instruction->p2 = {.reg1 = 16, .reg2 = 0, .scale = 0, .base = base};
                return;
            }
            r_m += REXB(instruction->REX) << 3;
            instruction->p2 = {.reg1 = r_m, .reg2 = -1, .scale = 0, .base = 0};
        break;

        case 1:
            if (r_m == 4) {
                sib(st, instruction, mod);
                base = int_8S(st);
                instruction->p2.base = base;
                return;
            }
            r_m += REXB(instruction->REX) << 3;
            base = int_8S(st);
            instruction->p2 = {.reg1 = r_m, .reg2 = -1, .scale = 0, .base = base};
        break;

        case 2:
            if (r_m == 4) {
                sib(st, instruction, mod);
                instruction->p2.base = int_32S();
                return;
            }
            r_m += REXB(instruction->REX) << 3;
            base = int_32S();
            instruction->p2 = {.reg1 = r_m, .reg2 = -1, .scale = 0, .base = base};
        break;

        case 3:
            r_m += REXB(instruction->REX) << 3;
            instruction->p2 = {.reg1 = r_m, .reg2 = -1, .scale = -1, .base = base};
        break;

        default:
            fprintf(stderr, "ERROR mod 12\n");
    }
}
