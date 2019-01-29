#ifdef BIT8
#define pop_58 pop_58_8
#define pop_8f pop_8f_8
#define leave_c9 leave_c9_8
#define assign assign_8
#define eval eval_8
#define pop pop_8
#endif
#ifdef BIT32
#define pop_58 pop_58_32
#define pop_8f pop_8f_32
#define leave_c9 leave_c9_32
#define assign assign_32
#define eval eval_32
#define pop pop_32
#endif
#ifdef BIT64
#define pop_58 pop_58_64
#define pop_8f pop_8f_64
#define leave_c9 leave_c9_64
#define assign assign_64
#define eval eval_64
#define pop pop_64
#endif

code* pop_58(state* st, code* instruction) {
    value v = pop_64(st);
    int reg = instruction->number - 0x58 + (REXR(instruction->REX) << 3);
    instruction->p1 = (param) {reg, -1, -1, 0};
    assign(st, instruction->p1, v);
    if (v.is_dynamic) {
        fprintf(stderr, "OPS, NOT IMPLEMENTED 264\n");
        // generate mov from stack to p1
    }
    return NULL;
}

code* pop_8f(state* st, code* instruction) {
    value v = pop(st);
    parce_reg_mem(st, instruction);
    assign(st, instruction->p2, v);
    if (v.is_dynamic) {
        code* pref = prefix(instruction->p2);
        printf(stderr, "OPS, NOT IMPLEMENTED 216\n");
        // generate mov from stack to p2
        return pref;
    }
    return NULL;
}

code* leave_c9(state* st, code* instruction) {
    value v1 = eval(st, RBP);
    if (v1.is_dynamic) {
        printf(stderr, "OPS, NOT IMPLEMENTED 846\n");
    }
    assign(st, RSP, v1);
    value v2 = pop_64(st);
    if (v2.is_dynamic) {
        printf(stderr, "OPS, NOT IMPLEMENTED 258\n");
    }
    assign(st, RBP, v2);
    return NULL;
}