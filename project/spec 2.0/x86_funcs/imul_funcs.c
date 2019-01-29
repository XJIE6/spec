#ifdef BIT8
#define imul_af imul_af_8
#define idiv_f7 idiv_f7_8
#define assign assign_8
#define eval eval_8
#define int_s int_8
#define int_cur int_8
#define print print_8
#define prefix prefix_8
#define INST "idivb %%bl;"
#endif
#ifdef BIT32
#define imul_af imul_af_32
#define idiv_f7 idiv_f7_32
#define assign assign_32
#define eval eval_32
#define int_s int_32
#define int_cur int_32
#define print print_32
#define prefix prefix_32
#define INST "idivl %%ebx;"
#endif
#ifdef BIT64
#define imul_af imul_af_64
#define idiv_f7 idiv_f7_64
#define assign assign_64
#define eval eval_64
#define int_s int_32S
#define int_cur int_64
#define print print_64
#define prefix prefix_64
#define INST "idivq %%rbx;"
#endif

code* imul_af(state* st, code* instruction) {
    parce_reg_mem(st, instruction);
    value v1 = eval(st, instruction->p1);
    value v2 = eval(st, instruction->p2);
    v1.base *= v2.base;
    if (v1.is_dynamic) {
        code* pref = prefix(instruction->p2);
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
    assign(st, instruction->p1, v1);
    return NULL;
}

code* idiv_f7(state* st, code* instruction) {
    parce_reg_mem(st, instruction);
    value v = eval(st, instruction->p2);
    if (v.is_dynamic) {
        dynamic(st, RDX);
        dynamic(st, RAX);
        return instruction;
    }
    value v1 = eval(st, RDX);
    value v2 = eval(st, RAX);
    if (v1.is_dynamic) {
        code* pref = prefix(st, RAX);
        instruction->next = pref;
        return instruction;
    }
    if (v2.is_dynamic) {
        code* pref = prefix(st, RDX);
        instruction->next = pref;
        return instruction;        
    }
    asm(INST
        : "=d" (v1.base), "=a" (v2.base)
        : "d"  (v1.base), "a"  (v2.base), "b" (v.base));
    long long div = v.base;
    assign(st, RDX, v1);
    assign(st, RAX, v2);
    return NULL;
}