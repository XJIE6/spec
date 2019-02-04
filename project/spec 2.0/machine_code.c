#include "machine_code.h"
#include "stdio.h"

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

void print_param(param p) {
    if (p.scale == -1) {
        fprintf(stderr, "%%");
        print_reg(p.reg1);
        return;
    }
    if (p.base != 0) {
        fprintf(stderr, "%d", p.base);
    }
    fprintf(stderr, "(");
    if (p.reg2 == -1) {
        print_reg(p.reg1);
    }
    else {
        print_reg(p.reg1);
        fprintf(stderr, ", ");
        print_reg(p.reg2);
        fprintf(stderr, ", %d", p.scale);
    }
    fprintf(stderr, ")");
}

void print(code* instr) {
    if (instr->next != NULL) {
        print(instr->next);
    }
    fprintf(stderr, "%#04x ", instr->number);
    switch(instr->type) {
        case empty:
        break;

        case bit_8:
            fprintf(stderr, "%d", instr->base);
        break;

        case bit_8S:
            fprintf(stderr, "%d", instr->base);
        break;

        case bit_32:
            fprintf(stderr, "%d", instr->base);
        break;

        case bit_32S:
            fprintf(stderr, "%d", instr->base);
        break;

        case bit_64:
            fprintf(stderr, "%lld", instr->base);
        break;

        case reg_mem:
            print_param(instr->p1);
            fprintf(stderr, " ");
            print_param(instr->p2);
        break;

        case reg_mem_bit_8:
            print_param(instr->p2);
            fprintf(stderr, " %d", instr->base);
        break;

        case reg_mem_bit_8S:
            print_param(instr->p2);
            fprintf(stderr, " %d", instr->base);
        break;

        case reg_mem_bit_32:
            print_param(instr->p2);
            fprintf(stderr, " %d", instr->base);
        break;

        case reg_mem_bit_32S:
            print_param(instr->p2);
            fprintf(stderr, " %d", instr->base);
        break;
    }
    fprintf(stderr, "\n");
    return;
}

code* read_instruction(state* st) {
    code* instruction = malloc(sizeof(code));
    instruction->REX = 0;
    instruction->pre = 0;
    instruction->next = NULL;
    instruction->number = get_char(st);
    instruction->base = 0;
    if (instruction->number >= 0x40 && instruction->number <= 0x4f) {
        instruction->REX = instruction->number;
        instruction->number = get_char(st);
    }
    if (instruction->number == 0x0f) {
        instruction->pre = instruction->number;
        instruction->number = get_char(st);
    }

    if (instruction->pre == 0x0f) {
        switch (instruction->number) {
            case 0x80:
            case 0x81:
            case 0x82:
            case 0x83:
            case 0x84:
            case 0x85:
            case 0x86:
            case 0x87:
            case 0x88:
            case 0x89:
            case 0x8a:
            case 0x8b:
            case 0x8c:
            case 0x8d:
            case 0x8e:
            case 0x8f:
                instruction->type = bit_32S;
            break;

            case 0xaf:
                instruction->type = reg_mem;
            break;

            case 0xb6:
                instruction->type = reg_mem;
            break;

            case 0xbe:
                instruction->type = reg_mem;
            break;

            case 0x1f:
                instruction->type = empty;
            break;

            default:
                fprintf(stderr, "UNKNOWN 0x0f %#04x\n", instruction->number);
                st->regs[16] = 0;
                return NULL;
            break;
        }
    }
    else {
        switch(instruction->number) {
            case 0x00:
            case 0x01:
            case 0x02:
            case 0x03:
                instruction->type = reg_mem;
            break;

            case 0x04:
                instruction->type = bit_8;
            break;

            case 0x05:
                if (REXW(instruction->REX)) {
                    instruction->type = bit_32S;
                }
                else {
                    instruction->type = bit_32;
                }
            break;

            case 0x28:
            case 0x29:
            case 0x2a:
            case 0x2b:
                instruction->type = reg_mem;
            break;

            case 0x2c:
                instruction->type = bit_8;
            break;

            case 0x2d:
                if (REXW(instruction->REX)) {
                    instruction->type = bit_32S;
                }
                else {
                    instruction->type = bit_32;
                }
            break;

            case 0x38:
            case 0x39:
            case 0x3a:
            case 0x3b:
                instruction->type = reg_mem;
            break;

            case 0x3c:
                instruction->type = bit_8;
            break;

            case 0x3d:
                if (REXW(instruction->REX)) {
                    instruction->type = bit_32S;
                }
                else {
                    instruction->type = bit_32;
                }
            break;

            case 0x50:
            case 0x51:
            case 0x52:
            case 0x53:
            case 0x54:
            case 0x55:
            case 0x56:
            case 0x57:
                instruction->type = empty;
            break;

            case 0x58:
            case 0x59:
            case 0x5a:
            case 0x5b:
            case 0x5c:
            case 0x5d:
            case 0x5e:
            case 0x5f:
                instruction->type = empty;
            break;

            case 0x63:
                instruction->type = reg_mem;
            break;

            case 0x68:
                if (REXW(instruction->REX)) {
                    instruction->type = bit_32S;
                }
                else {
                    instruction->type = bit_32;
                }
            break;

            case 0x6a:
                instruction->type = bit_8;
            break;

            case 0x70:
            case 0x71:
            case 0x72:
            case 0x73:
            case 0x74:
            case 0x75:
            case 0x76:
            case 0x77:
            case 0x78:
            case 0x79:
            case 0x7a:
            case 0x7b:
            case 0x7c:
            case 0x7d:
            case 0x7e:
            case 0x7f:
                instruction->type = bit_8S;
            break;
            

            case 0x80:
                instruction->type = reg_mem_bit_8;
            break;

            case 0x81:
                if (REXW(instruction->REX)) {
                    instruction->type = reg_mem_bit_32S;
                }
                else {
                    instruction->type = reg_mem_bit_32;
                }
            break;

            case 0x83:
                instruction->type = reg_mem_bit_8S;
            break;

            case 0x84:
            case 0x85:
                instruction->type = reg_mem;
            break;

            case 0x88:
            case 0x89:
            case 0x8a:
            case 0x8b:
                instruction->type = reg_mem;
            break;

            case 0x8d:
                instruction->type = reg_mem;
            break;

            case 0x8f:
                instruction->type = reg_mem;
            break;

            case 0x90:
                instruction->type = empty;
            break;

            case 0x98:
                instruction->type = empty;
            break;

            case 0x99:
                instruction->type = empty;
            break;

            case 0xb0:
            case 0xb1:
            case 0xb2:
            case 0xb3:
            case 0xb4:
            case 0xb5:
            case 0xb6:
            case 0xb7:
                instruction->type = bit_8;
            break;

            case 0xb8:
            case 0xb9:
            case 0xba:
            case 0xbb:
            case 0xbc:
            case 0xbd:
            case 0xbe:
            case 0xbf:
                if (REXW(instruction->REX)) {
                    instruction->type = bit_64;
                }
                else {
                    instruction->type = bit_32;
                }
            break;

            case 0xc3:
                instruction->type = empty;
            break;

            case 0xc6:
                instruction->type = reg_mem_bit_8S;
            break;

            case 0xc7:
                if (REXW(instruction->REX)) {
                    instruction->type = reg_mem_bit_32S;
                }
                else {
                    instruction->type = reg_mem_bit_32;
                }
            break;

            case 0xc9:
                instruction->type = empty;
            break;

            case 0xe8:
                instruction->type = bit_32S;
            break;

            case 0xe9:
                instruction->type = bit_32S;
            break;

            case 0xeb:
                instruction->type = bit_8S;
            break;

            case 0xf7:
                instruction->type = reg_mem;
            break;

            case 0xff:
                instruction->type = reg_mem;
            break;

            default:
                fprintf(stderr, "UNKNOWN %#04x\n", instruction->number);
                st->regs[16] = 0;
                return NULL;
        }
    }

    switch(instruction->type) {
        case empty:
        break;

        case bit_8:
            instruction->base = int_8(st);
        break;

        case bit_8S:
            instruction->base = int_8S(st);
        break;

        case bit_32:
            instruction->base = int_32(st);
        break;

        case bit_32S:
            instruction->base = int_32S(st);
        break;

        case bit_64:
            instruction->base = int_64(st);
        break;

        case reg_mem:
            parce_reg_mem(st, instruction);
        break;

        case reg_mem_bit_8:
            parce_reg_mem(st, instruction);
            instruction->base = int_8(st);
        break;

        case reg_mem_bit_8S:
            parce_reg_mem(st, instruction);
            instruction->base = int_8S(st);
        break;

        case reg_mem_bit_32:
            parce_reg_mem(st, instruction);
            instruction->base = int_32(st);
        break;

        case reg_mem_bit_32S:
            parce_reg_mem(st, instruction);
            instruction->base = int_32S(st);
        break;
    }

    return instruction;
}

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