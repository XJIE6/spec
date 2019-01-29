#include "../machine_code.h"
#include "../state.h"
#include "funcs.h"
#include "../registers.h"
#include "../spec_funcs.h"
#include <stdio.h>

//#include "jmp_funcs.c"

#define BIT8
#include "add_funcs.c"
#include "cmp_funcs.c"
// #include "imul_funcs.c"
// #include "mov_funcs.c"
// #include "pop_funcs.c"
// #include "push_funcs.c"
// #include "ret_funcs.c"
#include "sub_funcs.c"
#undef BIT8
#define BIT32
#include "add_funcs.c"
#include "cmp_funcs.c"
// #include "imul_funcs.c"
// #include "mov_funcs.c"
// #include "pop_funcs.c"
// #include "push_funcs.c"
// #include "ret_funcs.c"
#include "sub_funcs.c"
#undef BIT32
#define BIT64
#include "add_funcs.c"
#include "cmp_funcs.c"
// #include "imul_funcs.c"
// #include "mov_funcs.c"
// #include "pop_funcs.c"
// #include "push_funcs.c"
// #include "ret_funcs.c"
#include "sub_funcs.c"
#undef BIT64

code* eval_instruction(state* st, code* instruction) {
    char f;
    switch(instruction->number) {
        case 0x00:
            return add_01_8(st, instruction);
        break;

        case 0x01:
            if (REXW(instruction->REX)) {
                return add_01_64(st, instruction);
            }
            else {
                return add_01_32(st, instruction);
            }
        break;

        case 0x02:
            return add_03_8(st, instruction);
        break;

        case 0x03:
            if (REXW(instruction->REX)) {
                return add_03_64(st, instruction);
            }
            else {
                return add_03_32(st, instruction);
            }
        break;

        case 0x04:
            return add_05_8(st, instruction);
        break;

        case 0x05:
            if (REXW(instruction->REX)) {
                return add_05_64(st, instruction);
            }
            else {
                return add_05_32(st, instruction);
            }
        break;

        case 0x28:
            return sub_29_8(st, instruction);
        break;

        case 0x29:
            if (REXW(instruction->REX)) {
                return sub_29_64(st, instruction);
            }
            else {
                return sub_29_32(st, instruction);
            }
        break;

        case 0x2a:
            return sub_2b_8(st, instruction);
        break;

        case 0x2b:
            if (REXW(instruction->REX)) {
                return sub_2b_64(st, instruction);
            }
            else {
                return sub_2b_32(st, instruction);
            }
        break;

        case 0x2c:
            return sub_2d_8(st, instruction);
        break;

        case 0x2d:
            if (REXW(instruction->REX)) {
                return sub_2d_64(st, instruction);
            }
            else {
                return sub_2d_32(st, instruction);
            }
        break;

        case 0x38:
            return cmp_39_8(st, instruction);
        break;

        case 0x39:
            if (REXW(instruction->REX)) {
                return cmp_39_64(st, instruction);
            }
            else {
                return cmp_39_32(st, instruction);
            }
        break;

        case 0x3a:
            return cmp_3b_8(st, instruction);
        break;

        case 0x3b:
            if (REXW(instruction->REX)) {
                return cmp_3b_64(st, instruction);
            }
            else {
                return cmp_3b_32(st, instruction);
            }
        break;

        case 0x3c:
            return cmp_3d_8(st, instruction);
        break;

        case 0x3d:
            if (REXW(instruction->REX)) {
                return cmp_3d_64(st, instruction);
            }
            else {
                return cmp_3d_32(st, instruction);
            }
        break;

        case 0x81:
        f = read_reg(st);
        switch(f) {
            case 0:
            if (REXW(instruction->REX)) {
               return add_81_64(st, instruction);
            }
            else {
               return add_81_32(st, instruction);
            }
            break;
            case 5:
                if (REXW(instruction->REX)) {
                   return sub_81_64(st, instruction);
                }
                else {
                   return sub_81_32(st, instruction);
                }
            break;
            case 7:
                if (REXW(instruction->REX)) {
                   return cmp_81_64(st, instruction);
                }
                else {
                   return cmp_81_32(st, instruction);
                }
            break;
        }   

        case 0x83:
        f = read_reg(st);
        switch(f) {
            case 0:
                if (REXW(instruction->REX)) {
                   return add_83_64(st, instruction);
                }
                else {
                   return add_83_32(st, instruction);
                }
            break;
            case 5:
                if (REXW(instruction->REX)) {
                   return sub_83_64(st, instruction);
                }
                else {
                   return sub_83_32(st, instruction);
                }
            break;
            case 7:
                if (REXW(instruction->REX)) {
                   return cmp_83_64(st, instruction);
                }
                else {
                   return cmp_83_32(st, instruction);
                }
            break;
        }

        case 0x84:
            return test_85_8(st, instruction);
        break;

        case 0x85:
            if (REXW(instruction->REX)) {
                return test_85_64(st, instruction);
            }
            else {
                return test_85_32(st, instruction);
            }
        break;


        default:
            fprintf(stderr, "UNKNOWN %#04x\n", instruction->number);
            return 1;
    }
}