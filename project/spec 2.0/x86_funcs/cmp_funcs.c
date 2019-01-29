#ifdef BIT8
#define cmp_3d cmp_3d_8
#define cmp_81 cmp_81_8
#define cmp_83 cmp_83_8
#define cmp_39 cmp_39_8
#define cmp_3b cmp_3b_8
#define test_85 test_85_8
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
#define assign assign_64
#define eval eval_64
#define cmp cmp_64
#define int_u int_32
#define int_s int_32S
#define int_cur int_64
#define print print_64
#define prefix prefix_64
#endif

code* cmp_3d(state* st, code* instruction) {
    instruction->p1 = RAX;
    value v1 = eval(st, instruction->p1);
    value v2 = {int_u(st), -1, 0};
    instruction->base = v2.base;
    cmp(st, v1, v2);
    if (v1.is_dynamic) {
        return instruction;
    }
    return NULL;
}

code* cmp_81(state* st, code* instruction) {
    parce_reg_mem(st, instruction);
    value v1 = eval(st, instruction->p2);
    value v2 = {int_s(st), -1, 0};
    instruction->base = v2.base;
    cmp(st, v1, v2);
    if (v1.is_dynamic) {
        return instruction;
    }
    return NULL;
}

code* cmp_83(state* st, code* instruction) {
    parce_reg_mem(st, instruction);
    value v1 = eval(st, instruction->p2);
    value v2 = {int_8S(st), -1, 0};
    instruction->base = v2.base;
    cmp(st, v1, v2);
    if (v1.is_dynamic) {
        return instruction;
    }
    return NULL;
}

code* cmp_39(state* st, code* instruction) {
    parce_reg_mem(st, instruction);
    value v1 = eval(st, instruction->p1);
    value v2 = eval(st, instruction->p2);
    cmp(st, v1, v2);
    if (v1.is_dynamic) {
        code* pref = prefix(st, instruction->p2);
        instruction->next = pref;
        return instruction;
    }
    if (v2.is_dynamic) {
        code* pref1 = prefix(st, instruction->p1);
        code* pref2 = prefix(st, instruction->p2);
        if (pref1 == NULL) {
            instruction->next = pref2;
            return instruction;
        }
        pref1->next = pref2;
        instruction->next = pref1;
        return instruction;
    }
    return NULL;
}

code* cmp_3b(state* st, code* instruction) {
    parce_reg_mem(st, instruction);
    value v1 = eval(st, instruction->p1);
    value v2 = eval(st, instruction->p2);
    cmp(st, v2, v1);
    if (v1.is_dynamic) {
        code* pref = prefix(st, instruction->p2);
        instruction->next = pref;
        return instruction;
    }
    if (v2.is_dynamic) {
        code* pref1 = prefix(st, instruction->p1);
        code* pref2 = prefix(st, instruction->p2);
        if (pref1 == NULL) {
            instruction->next = pref2;
            return instruction;
        }
        pref1->next = pref2;
        instruction->next = pref1;
        return instruction;
    }
    return NULL;
}

code* test_85(state* st, code* instruction) {
    parce_reg_mem(st, instruction);
    value v1 = eval(st, instruction->p1);
    value v2 = eval(st, instruction->p2);
    if (v1.is_dynamic) {
        st->info_flags.is_dynamic = 1;
        code* pref = prefix(st, instruction->p2);
        instruction->next = pref;
        return instruction;
    }
    if (v2.is_dynamic) {
        st->info_flags.is_dynamic = 1;
        code* pref1 = prefix(st, instruction->p1);
        code* pref2 = prefix(st, instruction->p2);
        if (pref1 == NULL) {
            instruction->next = pref2;
            return instruction;
        }
        pref1->next = pref2;
        instruction->next = pref1;
        return instruction;
    }
    v1.base &= v2.base;
    st->flags[0] = 0;
    st->flags[11] = 0;
    if (v1.base) {
        st->flags[6] = 0;
    }
    else {
        st->flags[6] = 1;
    }
    if (v1.base < 0) {
        st->flags[7] = 1;
    }
    else {
        st->flags[7] = 0;
    }
    st->info_flags.is_dynamic = 0;
}