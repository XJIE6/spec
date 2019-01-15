#define parce_reg_mem parce_reg_mem_32
#define assign assign_32
#define pop pop_32
#define push push_64
#define eval eval_32
#define assign assign_32

#define CF (st->flags[0])
#define ZF (st->flags[6])
#define SF (st->flags[7])
#define OF (st->flags[11])

void jump(int cur) {
    //fprintf(stderr, "JUMP %d\n", cur);
    p1.reg1 = 16; //rip
    p1.reg2 = -1;
    p1.scale = -1;
    p1.base = 0;
    eval(&p1);
    v.base += cur;
    assign(&p1);
}

void dy_jump(int to) {
    copy(st);
    jump(to);
    calc_hash(st->next);
    //fprintf(stderr, "%lld\n", st->hash);
    st->next->next = st;
    st = st->next;
    calc_hash(st->next);
    //fprintf(stderr, "%lld\n", st->hash);
}

void jc(unsigned char cmd, char sh) {
    //fprintf(stderr, "fff %#04x to %d\n", cmd, sh);
    int cur = 0;
    if (sh) {
        cur = int_8S();
    }
    else {
        cur = int_32();
    }
    //fprintf(stderr, "TRY JUMP %d\n", cur);
    if (st->info_flags.is_dynamic) {
        dy_jump(cur);
        fprintf(stderr, "cjump %#04x to %d\n", cmd, st->next->hash);
        return;
    }
    switch(cmd) {
        case 0x80:
            if (OF) {
                jump(cur);
            }
            break;
        case 0x81:
            if (!OF) {
                jump(cur);
            }
            break;
        case 0x82:
            if (CF) {
                jump(cur);
            }
            break;
        case 0x83:
            if (!CF) {
                jump(cur);
            }
            break;
        case 0x84:
            if (ZF) {
                jump(cur);
            }
            break;
        case 0x85:
            if (!ZF) {
                jump(cur);
            }
            break;
        case 0x86:
            if (CF || ZF) {
                jump(cur);
            }
            break;
        case 0x87:
            if (!CF && !ZF) {
                jump(cur);
            }
            break;
        case 0x88:
            if (SF) {
                jump(cur);
            }
            break;
        case 0x89:
            if (!SF) {
                jump(cur);
            }
            break;
        case 0x8a:
            break;
        case 0x8b:
            break;
        case 0x8c:
            if (SF != OF) {
                jump(cur);
            }
            break;
        case 0x8d:
            if (SF == OF) {
                jump(cur);
            }
            break;
        case 0x8e:
            if (ZF || SF != OF) {
                jump(cur);
            }
            break;
        case 0x8f:
            if (!ZF && SF == OF) {
                jump(cur);
            }
            break;
    }
}

void jmp_e9(unsigned char cmd) {
    int cur = int_32();
    jump(cur);
}

void jmp_eb(unsigned char cmd) {
    int cur = int_8();
    jump(cur);
}

void movsxd_63(unsigned char cmd) {
    parce_reg_mem();
    eval(&p2);
    assign(&p1);
}

void* my_malloc(long long n);

int kmp2();

void call_e8(unsigned char cmd) {
    p1.reg1 = 16; //rip
    p1.reg2 = -1;
    p1.scale = -1;
    p1.base = 0;
    p2 = p1;
    int cur = int_32();
    //fprintf(stderr, "CALL %d\n", cur);
    //print_params();
    //fprintf(stderr, "%lld %lld\n", cur, &kmp2);
    eval(&p1);
    //print_value();
    if (v.base + cur == my_malloc) {
        fprintf(stderr, "call malloc\n");
        p1.reg1 = 7; //rdi
        p1.reg2 = -1;
        p1.scale = -1;
        p1.base = 0;
        eval(&p1);
        st->mem[st->mem_len] = my_malloc(v.base);
        st->info_mem[st->mem_len] = my_malloc(sizeof(info) * v.base);
        st->mem_mem_len[st->mem_len] = v.base;
        p1.reg1 = 0; //rax
        p1.reg2 = -1;
        p1.scale = -1;
        p1.base = 0;
        v.base = 0;
        v.mem = st->mem_len;
        assign(&p1);
        ++(st->mem_len);
        return;
    }

    copy(st);
    //fprintf(stderr, "CALL VALUE %d %d\n", st->regs[4], st->regs[5]);
    //print_value();
    push_64();
    st->mem_mem_len[0] += st->regs[4];
    st->mem[0] += st->regs[4];
    eval(&p2);
    v.base += cur;
    //fprintf(stderr, "FFF CALL %d\n", v.base % 20);
    assign(&p2);

    calc_hash(st->next);
    st->next->next = st;
    st = st->next;
    st->info_regs[0].is_dynamic = 1;
    calc_hash(st->next);
}
