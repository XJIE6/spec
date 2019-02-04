#ifdef BIT8
#define sub_29 sub_29_8
#define sub_2b sub_2b_8
#define sub_2d sub_2d_8
#define sub_81 sub_81_8
#define sub_83 sub_83_8
#define assign assign_8
#define eval eval_8
#define prefix prefix_8
#endif
#ifdef BIT32
#define sub_29 sub_29_32
#define sub_2b sub_2b_32
#define sub_2d sub_2d_32
#define sub_81 sub_81_32
#define sub_83 sub_83_32
#define assign assign_32
#define eval eval_32
#define prefix prefix_32
#endif
#ifdef BIT64
#define sub_29 sub_29_64
#define sub_2b sub_2b_64
#define sub_2d sub_2d_64
#define sub_81 sub_81_64
#define sub_83 sub_83_64
#define assign assign_64
#define eval eval_64
#define prefix prefix_64
#endif

code* sub_2d(state* st, code* instruction) {
    instruction->p1 = RAX;
    value v = eval(st, instruction->p1);
    if (v.is_dynamic) {
        return instruction;
    }
    v.base -= instruction->base;
    assign(st, instruction->p1, v);
    return NULL;
}

code* sub_81(state* st, code* instruction) {
    value v = eval(st, instruction->p2);
    if (v.is_dynamic) {
        return instruction;
    }
    v.base -= instruction->base;
    assign(st, instruction->p2, v);
    return NULL;
}

code* sub_83(state* st, code* instruction) {
    value v = eval(st, instruction->p2);
    if (v.is_dynamic) {
        return instruction;
    }
    v.base -= instruction->base;
    assign(st, instruction->p2, v);
    return NULL;
}

code* sub_29(state* st, code* instruction) {
    value v1 = eval(st, instruction->p1);
    if (v1.is_dynamic) {
        dynamic(st, instruction->p2);
        code* pref = prefix(st, instruction->p2);
        instruction->next = pref;
        return instruction;
    }
    value v2 = eval(st, instruction->p2);
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
    v2.base -= v1.base;
    if (v2.mem == -1) {
        v2.mem = v1.mem;
    }
    else if (v1.mem != -1) {
        fprintf(stderr, "ERROR 832\n");
        return NULL;
    }
    assign(st, instruction->p2, v2);
    return NULL;
}

code* sub_2b(state* st, code* instruction) {
    value v1 = eval(st, instruction->p1);
    if (v1.is_dynamic) {
        code* pref = prefix(st, instruction->p2);
        instruction->next = pref;
        return instruction;
    }
    value v2 = eval(st, instruction->p2);
    if (v2.is_dynamic) {
        dynamic(st, instruction->p1);
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
    v1.base -= v2.base;
    if (v1.mem == -1) {
        v1.mem = v2.mem;
    }
    else if (v2.mem != -1) {
        fprintf(stderr, "ERROR 115\n");
        return NULL;
    }
    assign(st, instruction->p1, v1);
    return NULL;
}