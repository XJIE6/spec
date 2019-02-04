#ifdef BIT8
#define add_01 add_01_8
#define add_03 add_03_8
#define add_05 add_05_8
#define add_81 add_81_8
#define add_83 add_83_8
#define assign assign_8
#define eval eval_8
#define prefix prefix_8
#endif
#ifdef BIT32
#define add_01 add_01_32
#define add_03 add_03_32
#define add_05 add_05_32
#define add_81 add_81_32
#define add_83 add_83_32
#define assign assign_32
#define eval eval_32
#define prefix prefix_32
#endif
#ifdef BIT64
#define add_01 add_01_64
#define add_03 add_03_64
#define add_05 add_05_64
#define add_81 add_81_64
#define add_83 add_83_64
#define assign assign_64
#define eval eval_64
#define prefix prefix_64
#endif

code* add_05(state* st, code* instruction) {
    instruction->p1 = RAX;
    value v = eval(st, instruction->p1);
    if (v.is_dynamic) {
        return instruction;
    }
    v.base += instruction->base;
    assign(st, instruction->p1, v);
    return NULL;
}

code* add_81(state* st, code* instruction) {
    value v = eval(st, instruction->p2);
    if (v.is_dynamic) {
        return instruction;
    }
    v.base += instruction->base;
    assign(st, instruction->p2, v);
    return NULL;
}

code* add_83(state* st, code* instruction) {
    value v = eval(st, instruction->p2);
    if (v.is_dynamic) {
        return instruction;
    }
    v.base += instruction->base;
    assign(st, instruction->p2, v);
    return NULL;
}

code* add_01(state* st, code* instruction) {
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
    v1.base += v2.base;
    if (v1.mem == -1) {
        v1.mem = v2.mem;
    }
    else if (v2.mem != -1) {
        fprintf(stderr, "ERROR 1236\n");
        return NULL;
    }
    assign(st, instruction->p2, v1);
    return NULL;
}

code* add_03(state* st, code* instruction) {
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
    v1.base += v2.base;
    if (v1.mem == -1) {
        v1.mem = v2.mem;
    }
    else if (v2.mem != -1) {
        fprintf(stderr, "ERROR 11\n");
        return NULL;
    }
    assign(st, instruction->p1, v1);
    return NULL;
}