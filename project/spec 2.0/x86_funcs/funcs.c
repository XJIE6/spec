#include "../machine_code.h"
#include "../state.h"
#include "funcs.h"
#include "../registers.h"
#include "../spec_funcs.h"
#include <stdio.h>

#include "jmp_funcs.c"

#define BIT8
#include "add_funcs.c"
#include "cmp_funcs.c"
#include "imul_funcs.c"
#include "mov_funcs.c"
#include "pop_funcs.c"
#include "push_funcs.c"
#include "sub_funcs.c"
#undef BIT8
#define BIT32
#include "add_funcs.c"
#include "cmp_funcs.c"
#include "imul_funcs.c"
#include "mov_funcs.c"
#include "pop_funcs.c"
#include "push_funcs.c"
#include "sub_funcs.c"
#undef BIT32
#define BIT64
#include "add_funcs.c"
#include "cmp_funcs.c"
#include "imul_funcs.c"
#include "mov_funcs.c"
#include "pop_funcs.c"
#include "push_funcs.c"
#include "sub_funcs.c"
#undef BIT64

code* eval_instruction(state* st, code* instruction) {
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
                return jc(st, instruction);
            break;

            case 0xaf:
                return imul_af(st, instruction);
            break;

            case 0xbe:
                if (REXW(instruction->REX)) {
                    return mov_be_64(st, instruction);
                }
                else {
                    return mov_be_32(st, instruction);
                }
            break;

            case 0xb6:
                if (REXW(instruction->REX)) {
                    return mov_b6_64(st, instruction);
                }
                else {
                    return mov_b6_32(st, instruction);
                }
            break;

            case 0x1f:
                return NULL;
            break;

            default:
                fprintf(stderr, "UNKNOWN 0x0f %#04x\n", instruction->number);
                return 1;
            break;
        }
    }
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

        case 0x50:
        case 0x51:
        case 0x52:
        case 0x53:
        case 0x54:
        case 0x55:
        case 0x56:
        case 0x57:
            if (REXW(instruction->REX)) {
                return push_50_64(st, instruction);
            }
            else {
                return push_50_32(st, instruction);
            }
        break;

        case 0x58:
        case 0x59:
        case 0x5a:
        case 0x5b:
        case 0x5c:
        case 0x5d:
        case 0x5e:
        case 0x5f:
            if (REXW(instruction->REX)) {
                return pop_58_64(st, instruction);
            }
            else {
                return pop_58_32(st, instruction);
            }
        break;

        case 0x63:
            return movsxd_63(st, instruction);
        break;

        case 0x68:
             if (REXW(instruction->REX)) {
                return push_68_64(st, instruction);
            }
            else {
                return push_68_32(st, instruction);
            }
        break;

        case 0x6a:
            return push_68_8(st, instruction);
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
            return jc(st, instruction);
        break;
        

        case 0x80:
        switch(instruction->p1.reg1) {
            case 0:
                return add_81_8(st, instruction);
                break;
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
                return cmp_81_8(st, instruction);
                break;
        }
        break;

        case 0x81:
        switch(instruction->p1.reg1) {
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
        switch(instruction->p1.reg1) {
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

        case 0x88:
            return mov_89_8(st, instruction);
        break;

        case 0x89:
            if (REXW(instruction->REX)) {
                return mov_89_64(st, instruction);
            }
            else {
                return mov_89_32(st, instruction);
            }
        break;

        case 0x8a:
            return mov_8b_8(st, instruction);
        break;

        case 0x8b:
            if (REXW(instruction->REX)) {
                return mov_8b_64(st, instruction);
            }
            else {
                return mov_8b_32(st, instruction);
            }
        break;

        case 0x8d:
            if (REXW(instruction->REX)) {
                return lea_8d_64(st, instruction);
            }
            else {
                return lea_8d_32(st, instruction);
            }
        break;

        case 0x8f:
            switch(instruction->p1.reg1) {
                case 0:
                    if (REXW(instruction->REX)) {
                        return pop_8f_64(st, instruction);
                    }
                    else {
                        return pop_8f_32(st, instruction);
                    }
                break;
            }
        break;

        case 0x90:
            return NULL;
        break;

        case 0x98:
            return NULL;
        break;

        case 0x99:
            return NULL;
        break;

        case 0xb0:
        case 0xb1:
        case 0xb2:
        case 0xb3:
        case 0xb4:
        case 0xb5:
        case 0xb6:
        case 0xb7:
            return mov_b8_8(st, instruction);
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
                return mov_b8_64(st, instruction);
            }
            else {
                return mov_b8_32(st, instruction);
            }
        break;

        case 0xc6:
            return mov_c7_8(st, instruction);
        break;

        case 0xc7:
            if (REXW(instruction->REX)) {
                return mov_c7_64(st, instruction);
            }
            else {
                return mov_c7_32(st, instruction);
            }
        break;

        case 0xc9:
            return leave_c9(st, instruction);
        break;

        case 0xe9:
            return jmp_e9(st, instruction);
        break;

        case 0xeb:
            return jmp_eb(st, instruction);
        break;

        case 0xf7:
            switch(instruction->p1.reg1) {
                case 7:
                    if (REXW(instruction->REX)) {
                        return idiv_f7_64(st, instruction);
                    }
                    else {
                        return idiv_f7_32(st, instruction);
                    }
                break;
            }
        break;

        case 0xff:
            switch(instruction->p1.reg1) {
                case 6:
                    if (REXW(instruction->REX)) {
                        return push_ff_64(st, instruction);
                    }
                    else {
                        return push_ff_32(st, instruction);
                    }
                break;
            }
        break;

        default:
            fprintf(stderr, "UNKNOWN %#04x\n", instruction->number);
            st->regs[16] = 0;
            return 1;
    }
}