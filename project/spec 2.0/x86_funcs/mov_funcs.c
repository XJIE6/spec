#ifdef BIT8
#define mov_89 mov_89_8
#define mov_8b mov_8b_8
#define mov_b8 mov_b8_8
#define mov_be mov_be_8
#define mov_b6 mov_b6_8
#define mov_c7 mov_c7_8
#define lea_8d lea_8d_8
#define assign assign_8
#define eval eval_8
#define lea lea_8
#define mov mov_8
#define int_cur int_8
#define int_c7 int_8
#define print print_8
#define prefix prefix_8
#endif
#ifdef BIT32
#define mov_89 mov_89_32
#define mov_8b mov_8b_32
#define mov_b8 mov_b8_32
#define mov_be mov_be_32
#define mov_b6 mov_b6_32
#define mov_c7 mov_c7_32
#define lea_8d lea_8d_32
#define assign assign_32
#define eval eval_32
#define lea lea_32
#define mov mov_32
#define int_cur int_32
#define int_c7 int_32
#define print print_32
#define prefix prefix_32
#endif
#ifdef BIT64
#define mov_89 mov_89_64
#define mov_8b mov_8b_64
#define mov_b8 mov_b8_64
#define mov_be mov_be_64
#define mov_b6 mov_b6_64
#define mov_c7 mov_c7_64
#define lea_8d lea_8d_64
#define assign assign_64
#define eval eval_64
#define lea lea_64
#define mov mov_64
#define int_cur int_64
#define int_c7 int_32S
#define print print_64
#define prefix prefix_64
#endif

code* mov_89(state* st, code* instruction) {    
    parce_reg_mem(st, instruction);   
    param p = instruction->p1;
    value v1 = eval(st, instruction->p1);
    if (v1.is_dynamic) {
        dynamic(st, instruction->p2);
        code* pref = prefix(st, instruction->p2);
        instruction->next = pref;
        return instruction;
    }
    value v2 = eval(st, instruction->p2);
    if (v2.is_dynamic) {
        //TODO if p2 registers are static, generating is ambigous
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

    assign(st, instruction->p2, v1);
    return NULL;
}

code* mov_8b(state* st, code* instruction) {
    parce_reg_mem(st, instruction);
    value v1 = eval(st, instruction->p1);
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
    assign(st, instruction->p1, v2);
    return NULL;
}

code* mov_b8(state* st, code* instruction) {
    value v = {int_cur(st), -1, 0};
    char reg = instruction->number - 0xb8 + (REXR(instruction->REX) << 3);
    instruction->p1 = (param) {reg, -1, -1, 0};
    assign(st, instruction->p1, v);
    return NULL;
}

code* mov_c7(state* st, code* instruction) {
    parce_reg_mem(st, instruction);
    instruction->base = int_c7(st);
    value v = eval(st, instruction->p2);
    if (v.is_dynamic) {
        //TODO if p2 registers are static, generating is ambigous
        code* pref2 = prefix(st, instruction->p2);
        instruction->next = pref2;
        return instruction;
    }
    v = (value){instruction->base, -1, 0};
    assign(st, instruction->p2, v);
    return NULL;
}

code* mov_be(state* st, code* instruction) {
    parce_reg_mem(st, instruction);
    value v1 = eval(st, instruction->p1);
    value v2 = eval_8(st, instruction->p2);
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
    assign(st, instruction->p1, v2);
    return NULL;
}

code* mov_b6(state* st, code* instruction) {
    parce_reg_mem(st, instruction);
    value v1 = eval_8(st, instruction->p1);
    if (v1.is_dynamic) {
        dynamic(st, instruction->p2);
        code* pref = prefix(st, instruction->p2);
        instruction->next = pref;
        return instruction;
    }
    value v2 = eval(st, instruction->p2);
    if (v2.is_dynamic) {
        //TODO if p2 registers are static, generating is ambigous
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
    assign(st, instruction->p2, v1);
    return NULL;
}

code* lea_8d(state* st, code* instruction) {
    parce_reg_mem(st, instruction);
    value v = lea(st, instruction->p2);
    if (v.is_dynamic) {
        code* pref = prefix(st, instruction->p2);
        instruction->next = pref;
        return instruction;
    }
    assign(st, instruction->p1, v);
    return NULL;
}