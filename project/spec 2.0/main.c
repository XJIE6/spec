//#define TEST

#include <stdio.h>
#include <stdlib.h>
#include "spec.h"
#include "state.h"

#include "test_programs/run_config.c"

//#include "kmp3.c"

int eval_prog(Program* prog, ProgramState* s);


int main(int argc, char const *argv[]) {

    FILE *f = fopen(argv[1], "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  //same as rewind(f);

    char *string = malloc(fsize + 1);
    fread(string, fsize, 1, f);
    fclose(f);

    string[fsize] = 0;

    // void * mem;
    // int len;

    // Program * p = NULL;
    // parseLine(string, &mem, &len, &p);

    // int a[200];

    // ProgramState s;
    // s.is_ret = 0;
    // s.ret_val = 0;
    // s.vars = &a;
    // eval_prog(p, &s);


    run_interpreter2(string);
















    // int n = 30000;
    // int res[n];
    // srand(12345);
    // for (int i = 0; i < n; ++i) {
    //     res[i] = rand() % 100000;
    // }

    // for (int i = 0; i < n; ++i) {
    //     fprintf(stderr, "%d ", res[i]);
    // }

    // sort(n, res);

    // for (int i = 0; i < n; ++i) {
    //     fprintf(stderr, "%d ", res[i]);
    // }

    //run_pow(3, 5);
    //run_sort(300);
    //run_dict();

    // const char* p = argv[1];
    // int len = 0;
    // while (*p != 0) {
    //     p++;
    //     len++;
    // }

    //run_prime(1000);
    //run_kmp();
    //run_kmp2();
    //run_kmp2();
    // const char* s = "aba";
    // const char* S = "aabbabbaabaaaa";
    // int n = 100;
    // int m = n / 2;
    // char a[n], b[n];
    // a[m] = 0;
    // b[m] = 0;
    // fprintf(stderr, "%d\n", call_kmp3(s, S, a + m, b + m));
}
