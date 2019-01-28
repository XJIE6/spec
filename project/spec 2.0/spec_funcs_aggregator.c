#include "state.h"
#include "machine_code.h"
#include "registers.h"
#include "stdio.h"
#include "spec_funcs.h"
#include <limits.h>

#define BIT8
#include "spec_funcs.c"
#undef BIT8
#define BIT32
#include "spec_funcs.c"
#undef BIT32
#define BIT64
#include "spec_funcs.c"
#undef BIT64

void dynamic(state* st, param p) {
    if ((p.reg1 != -1 && st->info_regs[p.reg1].is_dynamic) || (p.reg2 != -1 && st->info_regs[p.reg2].is_dynamic)) {
        return;
    }
    if (p.scale == -1) {
        st->info_regs[p.reg1].is_dynamic = 1;
    }
               
    value cur;
    cur.base = st->regs[p.reg1] * (1 << p.scale) + p.base;
    cur.mem = st->info_regs[p.reg1].mem;
    if (p.reg2 != -1) {
        cur.base += st->regs[p.reg2] * (1 << p.scale);
        if (st->info_regs[p.reg2].mem != -1) {
            if (cur.mem != -1) {
                fprintf(stderr, "ERROR eval 342\n");
                return;
            }
            cur.mem = st->info_regs[p.reg2].mem;
        }
    }
    st->info_mem[cur.mem][cur.base].is_dynamic = 1;
}