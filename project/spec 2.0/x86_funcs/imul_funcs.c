#ifdef BIT8
#define imul_af imul_af_8
#define idiv_f7 idiv_f7_8
#define parce_reg_mem parce_reg_mem_8
#define assign assign_8
#define eval eval_8
#define int_s int_8
#define int_cur int_8
#define print print_8
#define prefix prefix_8
#endif
#ifdef BIT32
#define imul_af imul_af_32
#define idiv_f7 idiv_f7_32
#define parce_reg_mem parce_reg_mem_32
#define assign assign_32
#define eval eval_32
#define int_s int_32
#define int_cur int_32
#define print print_32
#define prefix prefix_32
#endif
#ifdef BIT64
#define imul_af imul_af_64
#define idiv_f7 idiv_f7_64
#define parce_reg_mem parce_reg_mem_64
#define assign assign_64
#define eval eval_64
#define int_s int_32S
#define int_cur int_64
#define print print_64
#define prefix prefix_64
#endif

void imul_af(unsigned char cmd) {
    parce_reg_mem();
    eval(&p2);
    value cur;
    cur.base = v.base;
    cur.mem = v.mem;
    char dyn = is_dynamic;
    eval(&p1);
    v.base *= cur.base;
    is_dynamic = is_dynamic || dyn;
    if (is_dynamic) {
        prefix(&p1);
        fprintf(stderr, "imul_af ");
        print(&p2, 1);
        fprintf(stderr, " ");
        print(&p1, 0);
        fprintf(stderr, "\n");


        write_byte(0x0f);
        write_byte(0xaf);
        write_params();


    }
    assign(&p1);
}

void idiv_f7(unsigned char cmd) {
    parce_reg_mem();
    print_params();
    eval(&p2);
    print_value();
    long long div = v.base;
    //assign rdx;
    p1.reg1 = 2;
    p1.reg2 = -1;
    p1.base = 0;
    p1.scale = -1;
/*
    eval(&p1);
    print_value();
    long long a = v.base;*/

    //assign rax;
    p2.reg1 = 0;
    p2.reg2 = -1;
    p2.base = 0;
    p2.scale = -1;

    eval(&p2);
    print_value();
    long long b = v.base;

    v.base = b % div;
    v.mem = -1;
    is_dynamic = 0;

    assign(&p1);

    v.base = b / div;
    v.mem = -1;
    is_dynamic = 0;

    assign(&p2);

    /*eval(&p2);
    value cur;
    cur.base = v.base;
    cur.mem = v.mem;
    char dyn = is_dynamic;
    eval(&p1);
    v.base *= cur.base;
    is_dynamic = is_dynamic || dyn;
    if (is_dynamic) {
        prefix(&p1);
        fprintf(stderr, "imul_af ");
        print(&p2, 1);
        fprintf(stderr, " ");
        print(&p1, 0);
        fprintf(stderr, "\n");


        write_byte(0x0f);
        write_byte(0xaf);
        write_params();


    }
    assign(&p1);*/
    //int x = 1 / (1 - 1);
}