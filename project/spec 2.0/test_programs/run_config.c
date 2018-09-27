#include <sys/mman.h>
#include "lang.h"
void sort(int len, int* a);
int my_pow(int a, int b);
void dict(int len, int* a, int* b, int c);
void kmp(int len1, char* s1, int len2, char* s2);
char* eratosphen(int a);
int call_kmp(char* p, char* d, char* free1, char* free2);
int call_kmp3(char* p, char* d, char* free1, char* free2);
void interpreter(char * program, char * param);
int eval_prog(Program* prog, ProgramState* s);
state* new_state();

void run_sort(int n) {
    state* st = new_state();

    st->info_mem[1] = malloc(sizeof(info) * n * sizeof(int));
    st->mem[1] = malloc(sizeof(int) * n);
    st->mem_len = 2;
    st->mem_mem_len[1] = sizeof(int) * n;

    st->regs[16] = &sort;
    st->regs[7] = n;
    st->info_regs[6].mem = 1;
    int * res = st->mem[1];
    srand(12345);
    for (int i = 0; i < n; ++i) {
        res[i] = rand() % 100000;
    }

    calc_hash(st);

    for (int i = 0; i < n; ++i) {
        fprintf(stderr, "%d ", res[i]);
    }

    spec(st);

    for (int i = 0; i < n; ++i) {
        fprintf(stderr, "%d ", res[i]);
    }
}

void run_pow(int a, int b) {
    state* st = new_state();

    st->regs[16] = &my_pow;
    st->regs[7] = a;
    st->info_regs[7].is_dynamic = 1;
    st->regs[6] = b;

    calc_hash(st);

    int (*f)(int, int) = spec(st);
    //unsigned char * f = spec(st);

    fprintf(stderr, "WOW\n");
    fprintf(stderr, "%d\n", st->regs[0]);
    int res = f(2, b);
    fprintf(stderr, "WOW\n");
    fprintf(stderr, "%d\n", res);
    fprintf(stderr, "WOW\n");
    res = f(3, b);
    fprintf(stderr, "WOW\n");
    fprintf(stderr, "%d\n", res);
    res = f(4, b);
    fprintf(stderr, "WOW\n");
    fprintf(stderr, "%d\n", res);
    // for (int i = 0; i < 100; ++i) {
    //     fprintf(stderr, "%02X ", *f);
    //     f++;
    // }
}

void run_dict() {
    state* st = new_state();

    int n = 3;

    st->info_mem[1] = malloc(sizeof(info) * n * sizeof(int));
    st->mem[1] = malloc(sizeof(int) * n);
    st->mem_len = 2;
    st->mem_mem_len[1] = sizeof(int) * n;

    st->regs[16] = &dict;
    st->regs[7] = n;
    st->info_regs[6].mem = 1;
    st->info_regs[2].is_dynamic = 1;
    st->regs[1] = 1;
    int * res = st->mem[1];
    res[0] = 0;
    st->info_mem[1][0].mem = -1;
    res[1] = 1;
    st->info_mem[1][4].mem = -1;
    res[2] = 2;
    st->info_mem[1][8].mem = -1;

    calc_hash(st);

    spec(st);
}

void run_prime(int n) {
    state* st = new_state();

    st->regs[16] = &eratosphen;
    st->regs[7] = n;

    calc_hash(st);

    spec(st);

    char* res = st->regs[0];
    for (int i = 0; i < n; ++i) {
        if (res[i]) {
            fprintf(stderr, "%d ", i);
        }
    }
}

#include <string.h>

void run_kmp() {
    state* st = new_state();

    int len1 = 3;

    st->info_mem[1] = malloc(sizeof(info) * len1);
    st->mem[1] = malloc(len1);
    st->mem_len = 2;
    st->mem_mem_len[1] = len1;


    st->regs[16] = &kmp;
    st->regs[7] = len1;
    st->info_regs[6].mem = 1;
    st->regs[2] = 5;
    st->info_regs[1].is_dynamic = 1;

    for (int i = 0; i < len1; ++i) {
        st->info_mem[1][i].mem = -1;
        st->info_mem[1][i].is_dynamic = 0;
    }

    strcpy(st->mem[1], "aba");

    calc_hash(st);

    spec(st);
}

void run_kmp2() {
    state* st = new_state();

    int len1 = 8;
    int n = 100;

    st->mem_len = 4;

    st->info_mem[1] = malloc(sizeof(info) * len1);
    st->mem[1] = malloc(len1);

    st->info_mem[2] = malloc(sizeof(info) * n);
    st->mem[2] = malloc(n);

    st->info_mem[3] = malloc(sizeof(info) * n);
    st->mem[3] = malloc(n);

    st->mem_mem_len[1] = len1;
    st->mem_mem_len[2] = n;
    st->mem_mem_len[3] = n;

    st->regs[16] = &call_kmp3;
    st->info_regs[7].mem = 1;
    st->info_regs[6].is_dynamic = 1;
    st->info_regs[2].mem = 2;
    st->regs[2] = n / 2;
    st->info_regs[1].mem = 3;
    st->regs[1] = n / 2;

    for (int i = 0; i < len1; ++i) {
        st->mem[1][i] = 0;
        st->info_mem[1][i].mem = -1;
        st->info_mem[1][i].is_dynamic = 0;
    }
    for (int i = 0; i < n; ++i) {
        st->mem[2][i] = 0;
        st->info_mem[2][i].mem = -1;
        st->info_mem[2][i].is_dynamic = 0;
    }    
    for (int i = 0; i < n; ++i) {
        st->mem[3][i] = 0;
        st->info_mem[3][i].mem = -1;
        st->info_mem[3][i].is_dynamic = 0;
    }

    strcpy(st->mem[1], "aaaaa");

    calc_hash(st);

    spec(st);
}

void run_interpreter(char * program, int len) {
    state* st = new_state();

    st->mem_len = 2;

    st->info_mem[1] = malloc(sizeof(info) * len);
    st->mem[1] = malloc(len);

    st->mem_mem_len[1] = len;

    st->regs[16] = &interpreter;
    st->info_regs[7].mem = 1;
    st->info_regs[6].is_dynamic = 1;

    strcpy(st->mem[1], program);

    calc_hash(st);

    spec(st);
}

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