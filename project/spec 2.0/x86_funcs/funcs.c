#include "../machine_code.h"
#include "../state.h"
#include "funcs.h"
#include "../registers.h"
#include "../spec_funcs.h"
#include <stdio.h>

//#include "jmp_funcs.c"

#define BIT8
#include "add_funcs.c"
// #include "cmp_funcs.c"
// #include "imul_funcs.c"
// #include "mov_funcs.c"
// #include "pop_funcs.c"
// #include "push_funcs.c"
// #include "ret_funcs.c"
// #include "sub_funcs.c"
#undef BIT8
#define BIT32
#include "add_funcs.c"
// #include "cmp_funcs.c"
// #include "imul_funcs.c"
// #include "mov_funcs.c"
// #include "pop_funcs.c"
// #include "push_funcs.c"
// #include "ret_funcs.c"
// #include "sub_funcs.c"
#undef BIT32
#define BIT64
#include "add_funcs.c"
// #include "cmp_funcs.c"
// #include "imul_funcs.c"
// #include "mov_funcs.c"
// #include "pop_funcs.c"
// #include "push_funcs.c"
// #include "ret_funcs.c"
// #include "sub_funcs.c"
#undef BIT64

code* eval_instruction(state* st, code* instruction) {
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


    }
}