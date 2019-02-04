#define assign assign_32
#define pop pop_32
#define push push_64
#define eval eval_32
#define assign assign_32

#define CF (st->flags[0])
#define ZF (st->flags[6])
#define SF (st->flags[7])
#define OF (st->flags[11])

void jump(state* st, int cur) {
    value v = eval(st, RIP);
    v.base += cur;
    assign(st, RIP, v);
}

code* jc(state* st, code* instruction) {
    int cur = instruction->number;
    if (instruction->pre != 0x0f) {
        cur += 0x10;
    }
    switch(cur) {
        case 0x80:
            if (OF) {
                jump(st, instruction->base);
            }
            break;
        case 0x81:
            if (!OF) {
                jump(st, instruction->base);
            }
            break;
        case 0x82:
            if (CF) {
                jump(st, instruction->base);
            }
            break;
        case 0x83:
            if (!CF) {
                jump(st, instruction->base);
            }
            break;
        case 0x84:
            if (ZF) {
                jump(st, instruction->base);
            }
            break;
        case 0x85:
            if (!ZF) {
                jump(st, instruction->base);
            }
            break;
        case 0x86:
            if (CF || ZF) {
                jump(st, instruction->base);
            }
            break;
        case 0x87:
            if (!CF && !ZF) {
                jump(st, instruction->base);
            }
            break;
        case 0x88:
            if (SF) {
                jump(st, instruction->base);
            }
            break;
        case 0x89:
            if (!SF) {
                jump(st, instruction->base);
            }
            break;
        case 0x8a:
            break;
        case 0x8b:
            break;
        case 0x8c:
            if (SF != OF) {
                jump(st, instruction->base);
            }
            break;
        case 0x8d:
            if (SF == OF) {
                jump(st, instruction->base);
            }
            break;
        case 0x8e:
            if (ZF || SF != OF) {
                jump(st, instruction->base);
            }
            break;
        case 0x8f:
            if (!ZF && SF == OF) {
                jump(st, instruction->base);
            }
            break;
    }
    return NULL;
}

code* jmp_e9(state* st, code* instruction) {
    jump(st, instruction->base);
    return NULL;
}

code* jmp_eb(state* st, code* instruction) {
    jump(st, instruction->base);
    return NULL;
}

code* movsxd_63(state* st, code* instruction) { 
    param p = instruction->p1;
    value v1 = eval_32(st, instruction->p1);
    if (v1.is_dynamic) {
        dynamic(st, instruction->p2);
        code* pref = prefix_64(st, instruction->p2);
        instruction->next = pref;
        return instruction;
    }
    value v2 = eval(st, instruction->p2);
    if (v2.is_dynamic) {
        //TODO if p2 registers are static, generating is ambigous
        code* pref1 = prefix_64(st, instruction->p1);
        code* pref2 = prefix_64(st, instruction->p2);
        if (pref1 == NULL) {
            instruction->next = pref2;
            return instruction;
        }
        pref1->next = pref2;
        instruction->next = pref1;
        return instruction;
    }

    assign_64(st, instruction->p2, v1);
    return NULL;
}