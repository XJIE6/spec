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
#define assign assign_64
#define intCur int64
#define MAX LLONG_MAX
#define UMAX ULLONG_MAX
#define MIN LLONG_MIN
#endif

void cmp() {
    value cur = v;
    char dyn = 0;
    if (is_dynamic) {
        dyn = 1;
    }
    eval(&p1);
    if (is_dynamic) {
        dyn = 1;
    }
    if (dyn) {
        my_clf();
        st->info_flags.is_dynamic = 1;
        return;
    }
    if (cur.mem != v.mem) {
        fprintf(stderr, "MEM ARE NOT EQUAL\n");
        is_end = 1;
        return;
    }
    type a = v.base, b = cur.base;
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

void push() {
    RSP -= sizeof(type);
    p1.reg1 = 4; //rsp
    p1.reg2 = -1;
    p1.scale = 0;
    p1.base = 0;
    assign(&p1);
}

void pop() {
    if (RSP >= 0) {
        is_end = 1;
        return;
    }
    p1.reg1 = 4; //rsp
    p1.reg2 = -1;
    p1.scale = 0;
    p1.base = 0;
    eval(&p1);
    RSP += sizeof(type);
}

void assign(param* p) {
    if (p->scale == -1) {
        //fprintf(stderr, "pre\n");
        st->regs[p->reg1] = v.base;
        //fprintf(stderr, "pre\n");
        st->info_regs[p->reg1].mem = v.mem;
        //fprintf(stderr, "pre\n");
        st->info_regs[p->reg1].is_dynamic = is_dynamic;
        //fprintf(stderr, "pre\n");
        return;
    }
    value cur;
    if (p->scale != 0 && st->info_regs[p->reg2].mem != -1) {
        fprintf(stderr, "ERROR eval 34\n");
        return;
    }
    cur.base = st->regs[p->reg1] + p->base;
    cur.mem = st->info_regs[p->reg1].mem;
    
    if (p->reg2 != -1) {
        cur.base += st->regs[p->reg2] * (1 << p->scale);
        if (st->info_regs[p->reg2].mem != -1) {
            if (cur.mem != -1) {
                fprintf(stderr, "ERROR eval 342\n");
                return;
            }
            cur.mem = st->info_regs[p->reg2].mem;
        }
    }
    if (cur.mem == -1) {
        *((type*)cur.base) = (type)v.base;
        return;
    }
    //fprintf(stderr, "123 \n %lld %lld post\n", cur.mem, cur.base);
    //fprintf(stderr, "1234 \n %lld %lld %lld post\n", st->mem[1], st->mem[cur.mem], (st->mem[cur.mem] + cur.base));
    //fprintf(stderr, "WAHA \n %lld post\n", (int*)(st->mem[1] + 0));
    *((type*)(st->mem[cur.mem] + cur.base)) = v.base;
    st->info_mem[cur.mem][cur.base].mem = v.mem;
    st->info_mem[cur.mem][cur.base].is_dynamic = is_dynamic;
}


void print(param* p, char f) {
    value v1 = v;
    char d = is_dynamic;
    eval(p);
    if (f && !is_dynamic) {
        if (v.mem != -1) {
            fprintf(stderr, "%d(%d) ", v.base, v.mem);
        }
        else {
            fprintf(stderr, "%d ", v.base);
        }
        v = v1;
        is_dynamic = d;
        return;
    }
    if (p->scale == -1) {
        fprintf(stderr, "%%");
        print_reg(p->reg1);
        fprintf(stderr, " ");
        v = v1;
        is_dynamic = d;
        return;
    }
    param pp = *p;
    pp.reg2 = -1;
    pp.base = 0;
    pp.scale = -1;
    eval(&pp);
    if (is_dynamic) {
        if (p->reg2 == -1) {
            fprintf(stderr, "%d(", p->base);
            print_reg(p->reg1);
            fprintf(stderr, ") ");
        }
        else {
            if (p->base == 0) {
                fprintf(stderr, "(");
            }
            else {
                fprintf(stderr, "%d(", p->base);
            }
            param pp;
            pp.reg1 = p->reg2;
            pp.reg2 = -1;
            pp.base = 0;
            pp.scale = -1;
            eval(&pp);
            if (is_dynamic) {
                print_reg(p->reg2);
            }
            else {
                if (v.mem != -1) {
                    fprintf(stderr, "%d(%d) ", v.base, v.mem);
                }
                else {
                    fprintf(stderr, "%d ", v.base);
                }
            }
            fprintf(stderr, ", ");
            print_reg(p->reg1);
            fprintf(stderr, ", ");
            fprintf(stderr, "%d) ", p->scale);
        }
    }
    else {
        if (p->reg2 == -1) {
            if (v.mem != -1) {
                fprintf(stderr, "%d(%d) ", p->base + v.base, v.mem);
            }
            else {
                fprintf(stderr, "%d ", p->base + v.base);
            }
        }
        else {
            if (p->base == 0) {
                fprintf(stderr, "(");
            }
            else {
                fprintf(stderr, "%d(", p->base);
            }
            value v2 = v;
            param pp;
            pp.reg1 = p->reg2;
            pp.reg2 = -1;
            pp.base = 0;
            pp.scale = -1;
            eval(&pp);
            if (is_dynamic) {
                print_reg(p->reg2);
            }
            else {
                if (v.mem != -1) {
                    fprintf(stderr, "%d(%d) ", v.base, v.mem);
                }
                else {
                    fprintf(stderr, "%d ", v.base);
                }
                v = v2;
            }
            fprintf(stderr, ", ");
            if (v.mem != -1) {
                fprintf(stderr, "%d(%d) ", v.base, v.mem);
            }
            else {
                fprintf(stderr, "%d ", v.base);
            }
            fprintf(stderr, ", ");
            fprintf(stderr, "%d) ", p->scale);
        }
    }
    v = v1;
    is_dynamic = d;
    return;
}

void prefix(param* p) {
    value v1 = v;
    char d = is_dynamic;
    eval(p);
    if (!is_dynamic) {
        fprintf(stderr, "premov ");
        print(p, 1);
        fprintf(stderr, ", ");
        print(p, 0);
        fprintf(stderr, "\n");
        if (v.mem == 0) {
            write_byte(0x8b);
            param pp = *p;
            p1 = *p;
            p2.reg1 = 4; //RSP
            p2.reg2 = -1;
            p2.base = v.base;
            write_params();
            *p = pp;
        }
        else if (v.mem == -1) {
            write_byte(0xb8 + p->reg1);
            write_int(v.base);
        }
        else {
            fprintf(stderr, "ERROR PREFIX");
            is_end = 1;
            return;
        }
    }
    v = v1;
    is_dynamic = d;
    return;
    //fprintf(stderr, "print prefix\n");
}

void eval(param* p) {
    if ((p->reg1 != -1 && st->info_regs[p->reg1].is_dynamic) || (p->reg2 != -1 && st->info_regs[p->reg2].is_dynamic)) {
        is_dynamic = 1;
        return;
    }
    if (p->scale == -1) {
        v.base = st->regs[p->reg1];
        v.mem = st->info_regs[p->reg1].mem;
        is_dynamic = st->info_regs[p->reg1].is_dynamic;
        return;
    }
    char f = 0;
    if (p->scale < 0) {
        p->scale += 50;
        f = 1; 
    }
    value cur;
    if (p->scale != 0 && st->info_regs[p->reg2].mem != -1) {
        fprintf(stderr, "ERROR eval 34\n");
        return;
    }
    cur.base = st->regs[p->reg1] + p->base;
    cur.mem = st->info_regs[p->reg1].mem;
    //fprintf(stderr, "%lld %d\n", cur.base, cur.mem);
    if (p->reg2 != -1) {
        cur.base += st->regs[p->reg2] * (1 << p->scale);
        if (st->info_regs[p->reg2].mem != -1) {
            if (cur.mem != -1) {
                fprintf(stderr, "ERROR eval 342\n");
                return;
            }
            cur.mem = st->info_regs[p->reg2].mem;
        }
    }
    if (f) {
        v.base = cur.base;
        v.mem = cur.mem;
        return;
    }
    if (cur.mem == -1) {
        v.base = *((type*)cur.base);
        v.mem = -1;
        is_dynamic = 0;
        return;
    }
    
    //fprintf(stderr, "%lld %d % d %d\n", cur.base, cur.mem, st->mem[cur.mem][cur.base], *((type*)(st->mem[cur.mem] + cur.base)));
    v.base = *((type*)(st->mem[cur.mem] + cur.base));
    //fprintf(stderr, "ERROR eval 34\n");
    v.mem = st->info_mem[cur.mem][cur.base].mem;
    //fprintf(stderr, "ERROR eval 34\n");
    is_dynamic = st->info_mem[cur.mem][cur.base].is_dynamic;
    //fprintf(stderr, "%lld %d\n", v.base, v.mem);
}

void sib() {
    cur = get_char();
    scale = bit1_2(cur);
    _index = bit3_5(cur) + ((REXX() << 3));
    base = bit6_8(cur);
    if (base == 5) {
        base += REXB() << 3;
        switch (mod) {
            case 0:
                p2.reg1 = _index;
                p2.reg2 = -1;
                p2.scale = scale;
                p2.base = int_32S();
            break;

            case 1:                
                p2.reg1 = base;
                p2.reg2 = _index;
                //p2.reg1 = _index;
                //p2.reg2 = base;
                p2.scale = scale;
                p2.base = int_8S();
            break;

            case 2:
                p2.reg1 = base;
                p2.reg2 = _index;
                //p2.reg1 = _index;
                //p2.reg2 = base;
                p2.scale = scale;
                p2.base = int_32S();
            break;

            default:
                fprintf(stderr, "ERROR mod 2\n");
                //is_exit = 1;
        }
    }
    else if (_index == 4) {
        base += REXB() << 3;
        p2.reg1 = base;
        p2.reg2 = -1;
        p2.scale = scale;
        p2.base = 0;
    }
    else {
        base += REXB() << 3;
        p2.reg1 = base;
        p2.reg2 = _index;
        //p2.reg1 = _index;
        //p2.reg2 = base;
        p2.scale = scale;
        p2.base = 0;
    }
}

void parce_reg_mem() {
    cur = get_char();
    mod = bit1_2(cur);
    reg = bit3_5(cur) + (REXR() << 3);
    r_m = bit6_8(cur);
    p1.reg1 = reg;
    p1.reg2 = -1;
    p1.scale = -1;
    p1.base = 0;
    //fprintf(stderr, "%d : MOD\n", mod);
    switch(mod) {
        case 0:
            if (r_m == 4) {
                sib();
                return;
            }
            if (r_m == 5) {
                base = int_32S();
                p2.reg1 = 16;
                p2.reg2 = -1;
                p2.scale = 0;
                p2.base = base;
                return;
            }
            r_m += REXB() << 3;
            p2.reg1 = r_m;
            p2.reg2 = -1;
            p2.scale = 0;
            p2.base = 0;
        break;

        case 1:
            if (r_m == 4) {
                sib();
                p2.base = int_8S();
                return;
            }
            r_m += REXB() << 3;
            base = int_8S();
            p2.reg1 = r_m;
            p2.reg2 = -1;
            p2.scale = 0;
            p2.base = base;
        break;

        case 2:
            if (r_m == 4) {
                sib();
                p2.base = int_32S();
                return;
            }
            r_m += REXB() << 3;
            base = int_32S();
            p2.reg1 = r_m;
            p2.reg2 = -1;
            p2.scale = 0;
            p2.base = base;
        break;

        case 3:
            r_m += REXB() << 3;
            p2.reg1 = r_m;
            p2.reg2 = -1;
            p2.scale = -1;
            p2.base = 0;
        break;

        default:
            fprintf(stderr, "ERROR mod 12\n");
            //is_exit = 1;
    }
}
