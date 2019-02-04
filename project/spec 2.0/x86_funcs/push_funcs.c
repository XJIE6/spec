#ifdef BIT8
#define push_50 push_50_8
#define push_68 push_68_8
#define push_ff push_ff_8
#define push push_8
#define eval eval_8
#endif
#ifdef BIT32
#define push_50 push_50_32
#define push_68 push_68_32
#define push_ff push_ff_32
#define push push_32
#define eval eval_32
#endif
#ifdef BIT64
#define push_50 push_50_64
#define push_68 push_68_64
#define push_ff push_ff_64
#define push push_64
#define eval eval_64
#endif

code* push_50(state* st, code* instruction) {
    int reg = instruction->number - 0x50 + (REXR(instruction->REX) << 3);
    instruction->p1 = (param) {reg, -1, -1, 0};
    value v = eval(st, instruction->p1);
    push_64(st, v);
    if (v.is_dynamic) {
        fprintf(stderr, "OPS, NOT IMPLEMENTED 563\n");
        // generate mov from p1 to stack
    }
    return NULL;
}

code* push_ff(state* st, code* instruction) {
    value v = eval(st, instruction->p2);
    push_64(st, v);
    if (v.is_dynamic) {
        fprintf(stderr, "OPS, NOT IMPLEMENTED 938\n");
        // generate mov from p2 to stack
    }
    return NULL;
}

code* push_68(state* st, code* instruction) {
    value v = {instruction->base, -1, 0};
    push(st, v);
}