#ifdef BIT8
#define cmp_3d cmp_3d_8
#define cmp_81 cmp_81_8
#define cmp_83 cmp_83_8
#define cmp_39 cmp_39_8
#define cmp_3b cmp_3b_8
#define test_85 test_85_8
#define parce_reg_mem parce_reg_mem_8
#define assign assign_8
#define eval eval_8
#define cmp cmp_8
#define int_u int_8
#define int_s int_8
#define int_cur int_8
#define print print_8
#define prefix prefix_8
#endif
#ifdef BIT32
#define cmp_3d cmp_3d_32
#define cmp_81 cmp_81_32
#define cmp_83 cmp_83_32
#define cmp_39 cmp_39_32
#define cmp_3b cmp_3b_32
#define test_85 test_85_32
#define parce_reg_mem parce_reg_mem_32
#define assign assign_32
#define eval eval_32
#define cmp cmp_32
#define int_u int_32
#define int_s int_32
#define int_cur int_32
#define print print_32
#define prefix prefix_32
#endif
#ifdef BIT64
#define cmp_3d cmp_3d_64
#define cmp_81 cmp_81_64
#define cmp_83 cmp_83_64
#define cmp_39 cmp_39_64
#define cmp_3b cmp_3b_64
#define test_85 test_85_64
#define parce_reg_mem parce_reg_mem_64
#define assign assign_64
#define eval eval_64
#define cmp cmp_64
#define int_u int_32
#define int_s int_32S
#define int_cur int_64
#define print print_64
#define prefix prefix_64
#endif

void cmp_3d(unsigned char cmd) {
    p1.reg1 = 0;
    p1.reg2 = -1;
    p1.base = 0;
    p1.scale = -1;
    int a = int_u();
    v.base = a;
    v.mem = -1;
    is_dynamic = 0;
    cmp();
    if (is_dynamic) {
        fprintf(stderr, "cmp_3d rax %d\n", a);
    }
}

void cmp_81(unsigned char cmd) {
    parce_reg_mem();
    p1 = p2;
    int a = int_s();
    v.base = a;
    v.mem = -1;
    is_dynamic = 0;
    cmp();
    if (is_dynamic) {
        fprintf(stderr, "cmp81 ");
        print(&p2);
        fprintf(stderr, " %d\n", a);
    }
}

void cmp_83(unsigned char cmd) {
    parce_reg_mem();
    p1 = p2;
    int a = int_8S();
    v.base = a;
    v.mem = -1;
    is_dynamic = 0;
    cmp();
    if (is_dynamic) {
        fprintf(stderr, "cmp83 ");
        print(&p2);
        fprintf(stderr, " %d\n", a);
    }
}

void cmp_39(unsigned char cmd) {
    parce_reg_mem();
    //print_params();
    eval(&p1);
    //print_value();
    if (is_dynamic) {
        fprintf(stderr, "cmp39 ");
        print(&p2, 1);
        fprintf(stderr, " ");
        print(&p1, 1);
        fprintf(stderr, "\n");
    }
    p1 = p2;
    cmp();
    if (is_dynamic) {
        fprintf(stderr, "cmp39 ");
        print(&p2, 1);
        fprintf(stderr, " ");
        print(&p1, 1);
        fprintf(stderr, "\n");
    }
}

void cmp_3b(unsigned char cmd) {
    parce_reg_mem();
    eval(&p2);
    if (is_dynamic) {
        fprintf(stderr, "cmp3b ");
        print(&p2);
        fprintf(stderr, " ");
        print(&p1);
        fprintf(stderr, "\n");
    }
    cmp();
    if (is_dynamic) {
        fprintf(stderr, "cmp3b ");
        print(&p2);
        fprintf(stderr, " ");
        print(&p1);
        fprintf(stderr, "\n");
    }
}

void test_85(unsigned char cmd) {
    parce_reg_mem();
    print_params();
    eval(&p1);
    print_value();
    long long res = v.base;
    if(is_dynamic) {
        st->info_flags.is_dynamic = 1;
        fprintf(stderr, "test \n");
        return;
    }
    eval(&p2);
    print_value();
    v.base &= res;
    if (is_dynamic) {
        st->info_flags.is_dynamic = 1;
        fprintf(stderr, "test \n");
        return;
    }
    st->flags[0] = 0;
    st->flags[11] = 0;
    if (v.base) {
        st->flags[6] = 0;
    }
    else {
        st->flags[6] = 1;
    }
    if (v.base < 0) {
        st->flags[7] = 1;
    }
    else {
        st->flags[7] = 0;
    }
    st->info_flags.is_dynamic = 0;
}