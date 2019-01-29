#ifdef BIT8
#define type char
#define eval eval_8
#define parce_reg_mem parce_reg_mem_8
#define sib sib_8
#define push push_8
#define print print_8
#define prefix prefix_8
#define pop pop_8
#define cmp cmp_8
#define lea lea_8
#define assign assign_8
#define intCur int8
#define MAX CHAR_MAX
#define UMAX UCHAR_MAX
#define MIN CHAR_MIN
#endif
#ifdef BIT32
#define type int
#define eval eval_32
#define parce_reg_mem parce_reg_mem_32
#define sib sib_32
#define push push_32
#define print print_32
#define prefix prefix_32
#define pop pop_32
#define cmp cmp_32
#define lea lea_32
#define assign assign_32
#define intCur int32
#define MAX INT_MAX
#define UMAX UINT_MAX
#define MIN INT_MIN
#endif
#ifdef BIT64
#define type long long
#define eval eval_64
#define parce_reg_mem parce_reg_mem_64
#define sib sib_64
#define push push_64
#define print print_64
#define prefix prefix_64
#define pop pop_64
#define cmp cmp_64
#define lea lea_64
#define assign assign_64
#define intCur int64
#define MAX LLONG_MAX
#define UMAX ULLONG_MAX
#define MIN LLONG_MIN
#endif

void cmp(state* st, value v1, value v2) {
    if (v1.is_dynamic || v2.is_dynamic) {
        my_clf(st);
        st->info_flags.is_dynamic = 1;
        return;
    }
    if (v1.mem != v2.mem) {
        if (v1.mem == -1 && v1.base == 0) {
            v2.base = 1;
        }
        else if (v2.mem == -1 && v2.base == 0) {
            v1.base = 1;
        }
        else {
            fprintf(stderr, "MEM ARE NOT EQUAL\n");
            return;
        }
    }
    type a = v1.base, b = v2.base;
    //fprintf(stderr, "CMP %d %d\n", a, b);
    if (a < b) {
        st->flags[0] = 1;
        //fprintf(stderr, "SET CF\n");
    }
    else {
        st->flags[0] = 0;
        //fprintf(stderr, "UNSET CF\n");
    }

    if (a == b) {
        st->flags[6] = 1;
        //fprintf(stderr, "SET ZF\n");
    }
    else {
        st->flags[6] = 0;
        //fprintf(stderr, "UNSET ZF\n");
    }
    if (a - b < 0) {
        st->flags[7] = 1;
        //fprintf(stderr, "SET SF\n");
    }
    else {
        st->flags[7] = 0;
        //fprintf(stderr, "UNSET SF\n");
    }
    if ((b > 0 && a < MIN + b) || (b < 0 && a > MAX + b)) {
        st->flags[11] = 1;
        //fprintf(stderr, "SET OF\n");
    }
    else {
        st->flags[11] = 0;
        //fprintf(stderr, "UNSET OF\n");
    }
    st->info_flags.is_dynamic = 0;
}

void push(state* st, value v) {
    st->regs[4] -= sizeof(type);
    assign(st, (param){4, -1, 0, 0}, v);
}

value pop(state* st) {
    value v = eval(st, (param){4, -1, 0, 0});
    st->regs[4] += sizeof(type);
    return v;
}

void assign(state* st, param p, value v) {
    if (p.scale == -1) {
        st->regs[p.reg1] = v.base;
        st->info_regs[p.reg1].mem = v.mem;
        st->info_regs[p.reg1].is_dynamic = v.is_dynamic;
        return;
    }
    value cur;
    // if (p.scale != 0 && st->info_regs[p.reg2].mem != -1) {
    //     fprintf(stderr, "ERROR %d eval 34\n", p.scale);
    //     return;
    // }
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
    if (cur.mem == -1) {
    	if (v.mem == -1) {
        	*((type*)cur.base) = (type)v.base;
    	}
    	else {
    		*((type*)cur.base) = (type)v.base + st->mem[v.mem];
    	}
        return;
    }
    *((type*)(st->mem[cur.mem] + cur.base)) = v.base;
    st->info_mem[cur.mem][cur.base].mem = v.mem;
    st->info_mem[cur.mem][cur.base].is_dynamic = v.is_dynamic;
}



code* prefix(state* st, param p) {
    value v = eval(st, p);
    if (!v.is_dynamic) {
        //fprintf(stderr, "premov ");
        //print(st, p, 1);
        //fprintf(stderr, ", ");
        //print(st, p, 0);
        //fprintf(stderr, "\n");





        // if (v.mem == 0) {
        //     write_byte(0x8b);
        //     param pp = *p;
        //     p1 = *p;
        //     p2.reg1 = 4; //RSP
        //     p2.reg2 = -1;
        //     p2.base = v.base;
        //     write_params();
        //     *p = pp;
        // }
        // else if (v.mem == -1) {
        //     write_byte(0xb8 + p.reg1);
        //     write_int(v.base);
        // }
        // else {
        //     fprintf(stderr, "ERROR PREFIX");
        //     is_end = 1;
        //     return;
        // }
    }
    return NULL;
}

value lea(state* st, param p) {
    value v;
    if ((p.reg1 != -1 && st->info_regs[p.reg1].is_dynamic) || (p.reg2 != -1 && st->info_regs[p.reg2].is_dynamic)) {
        v.is_dynamic = 1;
        return v;
    }
    v.base = st->regs[p.reg1] * (1 << p.scale) + p.base;
    v.mem = st->info_regs[p.reg1].mem;
    if (p.reg2 != -1) {
        v.base += st->regs[p.reg2] * (1 << p.scale);
        if (st->info_regs[p.reg2].mem != -1) {
            if (v.mem != -1) {
                fprintf(stderr, "ERROR eval 342\n");
                return v;
            }
            v.mem = st->info_regs[p.reg2].mem;
        }
    }
    v.is_dynamic = 0;
    return v;
}

value eval(state* st, param p) {
    value v;
    if ((p.reg1 != -1 && st->info_regs[p.reg1].is_dynamic) || (p.reg2 != -1 && st->info_regs[p.reg2].is_dynamic)) {
        v.is_dynamic = 1;
        return v;
    }
    if (p.scale == -1) {
        v.base = st->regs[p.reg1];
        v.mem = st->info_regs[p.reg1].mem;
        v.is_dynamic = st->info_regs[p.reg1].is_dynamic;
        return v;
    }
    value cur;
    cur.base = st->regs[p.reg1] * (1 << p.scale) + p.base;
    cur.mem = st->info_regs[p.reg1].mem;
    if (p.reg2 != -1) {
        cur.base += st->regs[p.reg2] * (1 << p.scale);
        if (st->info_regs[p.reg2].mem != -1) {
            if (cur.mem != -1) {
                fprintf(stderr, "ERROR eval 342\n");
                return v;
            }
            cur.mem = st->info_regs[p.reg2].mem;
        }
    }
    if (cur.mem == -1) {
        v.base = *((type*)cur.base);
        v.mem = -1;
        v.is_dynamic = 0;
        return v;
    }
    v.base = *((type*)(st->mem[cur.mem] + cur.base));
    v.mem = st->info_mem[cur.mem][cur.base].mem;
    v.is_dynamic = st->info_mem[cur.mem][cur.base].is_dynamic;
    return v;
}