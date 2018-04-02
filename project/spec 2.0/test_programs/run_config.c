
#include <sys/mman.h>
void sort(int len, int* a);
int my_pow(int a, int b);
void dict(int len, int* a, int* b, int c);
void kmp(int len1, char* s1, int len2, char* s2);
char* eratosphen(int a);
int call_kmp(char* p, char* d, char* free1, char* free2);
int call_kmp3(char* p, char* d, char* free1, char* free2);
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

    int len1 = 3;
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

    strcpy(st->mem[1], "aba");

    calc_hash(st);

    spec(st);
}