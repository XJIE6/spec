#include "run_config.h"
#include "../state.h"
#include "lang.h"
#include "interpreter2.h"
#include <stdio.h>

void * start;
info * inf;

void change(void ** ptr) {
    void* cur = *ptr;
    int n = cur - start;
    (*ptr) = (void*) n;
    n = (void *)ptr - start;
    inf[n].mem = 1;
}

void pretty_expr(Expr* e) {
    switch(e->type) {
        case TConst:
            break;
        case TVar:
            break;
        case TBinop:
            pretty_expr(((Binop *) e->p)->l);
            pretty_expr(((Binop *) e->p)->r);
            change(&(((Binop *) e->p)->l));
            change(&(((Binop *) e->p)->r));
            break;
        case TCall:
        {
            Call * c = (Call *) e -> p;
            Par** cur = &(c->params);
            while (*cur != NULL) {
                pretty_expr((*cur)->e);
                change(&((*cur)->e));
                Par** next = &((*cur) -> next);
                change(cur);
                cur = next;
            }
            break;
        }
    }
    change(&(e->p));
}

void pretty_stmt(Stmt* stmt) {
    switch (stmt->type) {
        case TSkip:
            break;
        case TAss:
            pretty_expr(((Ass *) stmt->s)->e);
            change(&(((Ass *) stmt->s)->e));
            change(&(stmt->s));
            break;
        case TSeq:
            pretty_stmt(((Seq *) stmt->s)->l);
            pretty_stmt(((Seq *) stmt->s)->r);
            change(&(((Seq *) stmt->s)->l));
            change(&(((Seq *) stmt->s)->r));
            change(&(stmt->s));
            break;
        case TIf:
            pretty_expr(((If *) stmt->s)->e);
            pretty_stmt(((If *) stmt->s)->l);
            pretty_stmt(((If *) stmt->s)->r);
            change(&(((If *) stmt->s)->e));
            change(&(((If *) stmt->s)->l));
            change(&(((If *) stmt->s)->r));
            change(&(stmt->s));
            break;
        case TWhile:
            pretty_expr(((While *) stmt->s)->e);
            pretty_stmt(((While *) stmt->s)->s);
            change(&(((While *) stmt->s)->e));
            change(&(((While *) stmt->s)->s));
            change(&(stmt->s));
            break;
        case TRun:
        {
            Run * c = (Run *) stmt -> s;
            Par** cur = &(c->params);
            while (*cur != NULL) {
                pretty_expr((*cur)->e);
                change(&((*cur)->e));
                Par** next = &((*cur) -> next);
                change(cur);
                cur = next;
            }
            change(&(stmt->s));
            break;
        }
        case TReturn:
            pretty_expr(((Return *) stmt->s)->e);
            change(&(((Return *) stmt->s)->e));
            change(&(stmt->s));
            break;
    }
}

void pretty_program(Program * program) {
    Def ** cur = &(program -> defs);
    while (*cur != NULL) {
        Arg ** a = &((*cur) -> args);
        while (*a != NULL) {
            Arg ** b = &((*a) -> next);
            change(a);
            a = b;
        }
        pretty_stmt((*cur)->s);
        change(&((*cur)->s));
        Def ** next = &((*cur) -> next);
        change(cur);
        cur = next;
    }
    pretty_stmt(program->s);
    change(&(program->s));
}

void run_interpreter2(char* program) {
    state* st = new_state();
    void * mem;
    int len;

    Program * p = NULL;
    parseLine(program, &mem, &len, &p);
    if (p == NULL) {
        fprintf(stderr, "ERR");
        return;
    }

    st->mem_len = 3;
    st->mem[1] = mem;
    st->info_mem[1] = malloc(len * sizeof(info));
    st->mem_mem_len[1] = len;


    for (int i = 0; i < len; ++i) {
        st->info_mem[1][i].is_dynamic = 0;
        st->info_mem[1][i].mem = -1;
    }

    start = mem;
    inf = st->info_mem[1];
    pretty_program(p);

    st->mem[2] = malloc(sizeof(ProgramState) + 30 * sizeof(int));
    st->info_mem[2] = malloc((sizeof(ProgramState) + 30 * sizeof(int)) * sizeof(info));
    st->mem_mem_len[2] = sizeof(ProgramState) + 30 * sizeof(int);

    for (int i = 0; i < st->mem_mem_len[2]; ++i) {
        st->mem[2][i] = 0;
        st->info_mem[2][i].is_dynamic = 0;
        st->info_mem[2][i].mem = -1;
    }

    ProgramState* s = (ProgramState*) st->mem[2];
    s->is_ret = 0;
    s->ret_val = 0;
    s->vars = (int *) sizeof(ProgramState);
    st->info_mem[2][0].mem = 2;

    st->info_mem[2][sizeof(ProgramState) + 23 * sizeof(int)].is_dynamic = 1;
    st->info_mem[2][sizeof(ProgramState) + 24 * sizeof(int)].is_dynamic = 1;
    st->info_mem[2][sizeof(ProgramState) + 25 * sizeof(int)].is_dynamic = 1;


    st->regs[16] = &eval_prog;
    st->regs[7] = (void *)p - mem;
    st->info_regs[7].mem = 1;
    st->info_regs[6].mem = 2;

    calc_hash(st);
    spec(st);


    //fprintf(stderr, "%d", eval_prog(p, &s));
}