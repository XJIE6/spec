#include "machine_code.h"
#include "stdio.h"


unsigned char get_char(state* st) {
    unsigned char res = *((unsigned char*)st->regs[16]);
    st->regs[16] += sizeof(unsigned char);
    return res;
}

unsigned char try_get_char(state* st) {
    unsigned char res = *((unsigned char*)st->regs[16]);
    return res;
}

char read_reg(state* st) {
    unsigned char res = *((unsigned char*)st->regs[16]);
    return bit3_5(res);
}

char get_schar(state* st) {
    char res = *((char*)st->regs[16]);
    st->regs[16] += sizeof(char);
    return res;
}

int int_8S(state* st) {
    return get_schar(st);
}

int int_8(state* st) {
    return get_char(st);
}

int int_16S(state* st) {
    int res = get_char(st);
    res += get_schar(st) << 8;
    return res;
}

int int_16(state* st) {
    int res = get_char(st);
    res += get_char(st) << 8;
    return res;
}

int int_32S(state* st) {
    int res = get_char(st);
    res += get_char(st) << 8;
    res += get_char(st) << 16;
    res += get_schar(st) << 24;
    return res;
}

unsigned int int_32(state* st) {
    int res = get_char(st);
    res += get_char(st) << 8;
    res += get_char(st) << 16;
    res += get_char(st) << 24;
    return res;
}

long long int_64S(state* st) {
    long res = get_char(st);
    res += get_char(st) << 8;
    res += get_char(st) << 16;
    res += get_char(st) << 24;
    res += (long long) get_char(st) << 32;
    res += (long long) get_char(st) << 40;
    res += (long long) get_char(st) << 48;
    res += (long long) get_schar(st) << 56;
    return res;
}

unsigned long long int_64(state* st) {
    long res = get_char(st);
    res += get_char(st) << 8;
    res += get_char(st) << 16;
    res += get_char(st) << 24;
    res += (long long) get_char(st) << 32;
    res += (long long) get_char(st) << 40;
    res += (long long) get_char(st) << 48;
    res += (long long) get_char(st) << 56;
    return res;
}

void sib(state* st, code* instruction, char mod) {
    unsigned char cur = get_char(st);
    char scale = bit1_2(cur);
    unsigned char index = bit3_5(cur) + ((REXX(instruction->REX) << 3));
    long long base = bit6_8(cur);
    if (base == 5) {
        base += REXB(instruction->REX) << 3;
        switch (mod) {
            case 0:
                instruction->p2 = (param) {index, -1, scale, int_32S(st)};
            break;

            case 1:
                instruction->p2 = (param) {base, index, scale, int_8S(st)};
            break;

            case 2:
               instruction->p2 = (param) {base, index, scale, int_32S(st)};
            break;

            default:
                fprintf(stderr, "ERROR mod 2\n");
        }
    }
    else if (index == 4) {
        base += REXB(instruction->REX) << 3;
        instruction->p2 = (param) {base, -1, scale, 0};
    }
    else {
        base += REXB(instruction->REX) << 3;
        instruction->p2 = (param) {base, index, scale, 0};
    }
}

void parce_reg_mem(state* st, code* instruction) {
    unsigned char cur = get_char(st);
    unsigned char mod = bit1_2(cur);
    unsigned char reg = bit3_5(cur) + (REXR(instruction->REX) << 3);
    unsigned char r_m = bit6_8(cur);
    long long base;
    
    instruction->p1 = (param) {reg, -1, -1, 0};

    switch(mod) {
        case 0:
            if (r_m == 4) {
                sib(st, instruction, mod);
                return;
            }
            if (r_m == 5) {
                base = int_32S(st);
                instruction->p2 = (param) {16, 0, 0, base};
                return;
            }
            r_m += REXB(instruction->REX) << 3;
            instruction->p2 = (param) {r_m, -1, 0, 0};
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
            instruction->p2 = (param) {r_m, -1, 0, base};
        break;

        case 2:
            if (r_m == 4) {
                sib(st, instruction, mod);
                instruction->p2.base = int_32S(st);
                return;
            }
            r_m += REXB(instruction->REX) << 3;
            base = int_32S(st);
            instruction->p2 = (param) {r_m, -1, 0, base};
        break;

        case 3:
            r_m += REXB(instruction->REX) << 3;
            instruction->p2 = (param) {r_m, -1, -1, 0};
        break;

        default:
            fprintf(stderr, "ERROR mod 12\n");
    }
}

void my_clf(state* st) {
    st->flags[0] = 0;
    st->flags[6] = 0;
    st->flags[7] = 0;
    st->flags[11] = 0;
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

unsigned char REXW(char REX) {
    return (REX & 8) >> 3;
}

unsigned char REXR(char REX) {
    return (REX & 4) >> 2;
}

unsigned char REXX(char REX) {
    return (REX & 2) >> 1;
}

unsigned char REXB(char REX) {
    return REX & 1;
}