#ifdef BIT8
#define sub_29 sub_29_8
#define sub_2b sub_2b_8
#define sub_2d sub_2d_8
#define sub_81 sub_81_8
#define sub_83 sub_83_8
#define parce_reg_mem parce_reg_mem_8
#define assign assign_8
#define eval eval_8
#define int_s int_8
#define int_cur int_8
#define print print_8
#define prefix prefix_8
#endif
#ifdef BIT32
#define sub_29 sub_29_32
#define sub_2b sub_2b_32
#define sub_2d sub_2d_32
#define sub_81 sub_81_32
#define sub_83 sub_83_32
#define parce_reg_mem parce_reg_mem_32
#define assign assign_32
#define eval eval_32
#define int_s int_32
#define int_cur int_32
#define print print_32
#define prefix prefix_32
#endif
#ifdef BIT64
#define sub_29 sub_29_64
#define sub_2b sub_2b_64
#define sub_2d sub_2d_64
#define sub_81 sub_81_64
#define sub_83 sub_83_64
#define parce_reg_mem parce_reg_mem_64
#define assign assign_64
#define eval eval_64
#define int_s int_32S
#define int_cur int_64
#define print print_64
#define prefix prefix_64
#endif

void sub_2d(unsigned char cmd) {
    p1.reg1 = 0;
    p1.reg2 = -1;
    p1.base = 0;
    p1.scale = -1;
    eval(&p1);
    v.base -= int_s();
    if (is_dynamic) {
        fprintf(stderr, "sub2d\n");
    }
    assign(&p1);
}

void sub_81(unsigned char cmd) {
    parce_reg_mem();
    eval(&p2);
    v.base -= int_s();
    if (is_dynamic) {
        fprintf(stderr, "sub81\n");
    }
    assign(&p2);
}

void sub_83(unsigned char cmd) {
    parce_reg_mem();
    eval(&p2);
    int d = int_8S();
    v.base -= d;
    if (is_dynamic) {
        fprintf(stderr, "sub83\n");
    }
    assign(&p2);
}

void sub_29(unsigned char cmd) {
    parce_reg_mem();
    eval(&p2);
    value cur;
    cur.base = v.base;
    cur.mem = v.mem;
    if (is_dynamic) {
        prefix(&p1);
        fprintf(stderr, "sub29 ");
        print(&p2);
        fprintf(stderr, " ");
        print(&p1);
        fprintf(stderr, "\n");
    }
    eval(&p1);
    v.base -= cur.base;
    if (v.mem == -1) {
        v.mem = cur.mem;
    }
    else if (cur.mem != -1) {
        fprintf(stderr, "ERROR 1236\n");
        return;
    }
    if (is_dynamic) {
        prefix(&p1);
        fprintf(stderr, "sub29 ");
        print(&p2);
        fprintf(stderr, " ");
        print(&p1);
        fprintf(stderr, "\n");
    }
    assign(&p2);
}

void sub_2b(unsigned char cmd) {
    parce_reg_mem();
    eval(&p2);
    value cur;
    cur.base = v.base;
    cur.mem = v.mem;
    if (is_dynamic) {
        prefix(&p1);
        fprintf(stderr, "sub2b ");
        print(&p2);
        fprintf(stderr, " ");
        print(&p1);
        fprintf(stderr, "\n");
    }
    eval(&p1);
    v.base -= cur.base;
    if (v.mem == -1) {
        v.mem = cur.mem;
    }
    else if (cur.mem != -1) {
        fprintf(stderr, "ERROR 11\n");
        return;
    }
    if (is_dynamic) {
        prefix(&p1);
        fprintf(stderr, "sub2b ");
        print(&p2);
        fprintf(stderr, " ");
        print(&p1);
        fprintf(stderr, "\n");
    }
    assign(&p1);
}