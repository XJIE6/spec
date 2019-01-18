#include <stdio.h>
#include <limits.h>
#include <sys/mman.h>
#include <errno.h>
//#include <curses.h>
#include "spec.h"
#include "state.h"

void* my_malloc(long long n);

unsigned char REX;
state* st;
char* res;
char* last; 

const int MAX_MEM_SIZE = 12;
const int MAX_STACK_SIZE = 1000000;


int u_len = 1000000;

#define RIP st->regs[16]

// #define EAX egs[0]
// #define ECX egs[2]
// #define EDX egs[4]
// #define EBX egs[6]
// #define ESP egs[8]
// #define EBP egs[10]
// #define ESI egs[12]
// #define EDI egs[14]

// #define EIP egs[32]

// #define RAX regs[0]
// #define RCX regs[1]
// #define RDX regs[2]
// #define RBX regs[3]
#define RSP st->regs[4]
#define RBP st->regs[5]
// #define RSI regs[6]
// #define RDI regs[7]

// #define R8  regs[8]
// #define R9  regs[9]
// #define R10 regs[10]
// #define R11 regs[11]
// #define R12 regs[12]
// #define R13 regs[13]
// #define R14 regs[14]
// #define R15 regs[15]

// #define RIP regs[16]
// #define FLAGS regs[17]

#define ZF (1 << 6)
#define SF (1 << 7)
#define OF (1 << 11)

void my_clf() {
    st->flags[0] = 0;
    st->flags[6] = 0;
    st->flags[7] = 0;
    st->flags[11] = 0;
}

char is_end;

void compare(state* st1, state* st2) {
    fprintf(stderr, "\ncompare %d %d\n", st1->hash % u_len, st2->hash % u_len);
    for (int i = 0; i <= 16; ++i) {
        if (!st1->info_regs[i].is_dynamic) {
            if (st1->regs[i] != st2->regs[i] ||
                st1->info_regs[i].mem != st2->info_regs[i].mem) {
                fprintf(stderr, "FIRST %d\n", i);
            }    
        }
    }
    if (!st1->info_flags.is_dynamic) {
        for (int i = 0; i < 64; ++i) {
            if (st1->flags[i] != st2->flags[i]) {
                fprintf(stderr, "SECOND %d\n", i);
            }    
        }
    }

    for (int j = -st1->mem_mem_len[0]; j < 0; ++j) {
        if (!st1->info_mem[0][j].is_dynamic) {
            if (st1->mem[0][j] != st2->mem[0][j] ||
                st1->info_mem[0][j].mem != st2->info_mem[0][j].mem) {
                fprintf(stderr, "THIRD %d\n", j);
            }
        }
    }

    for (int i = 1; i < st1->mem_len; ++i) {
        for (int j = 0; j < st1->mem_mem_len[i]; ++j) {
            if (!st1->info_mem[i][j].is_dynamic) {
                if (st1->mem[i][j] != st2->mem[i][j] ||
                st1->info_mem[i][j].mem != st2->info_mem[i][j].mem) {
                fprintf(stderr, "FOURTH %d %d\n", i, j);
                }
            }
        }
    }
}

void calc_hash(state* cur) {
    //fprintf(stderr, "CALCED\n");
    int mod = 1000003;
    int mul = 1009;
    int res = 0;

    for (int i = 0; i <= 16; ++i) {
        if (!cur->info_regs[i].is_dynamic) {
            res *= mul;
            res %= mod;
            res += cur->regs[i] % mod;
            res %= mod;

            res *= mul;
            res %= mod;
            res += cur->info_regs[i].mem % mod;
            res %= mod;
        }
    }
    if (!cur->info_flags.is_dynamic) {
        for (int i = 0; i < 64; ++i) {
            res *= mul;
            res %= mod;
            res += cur->flags[i] % mod;
            res %= mod;
        }
    }

    for (int j = -cur->mem_mem_len[0]; j < 0; ++j) {
        if (!cur->info_mem[0][j].is_dynamic) {
            res *= mul;
            res %= mod;
            res += cur->mem[0][j] % mod;
            res %= mod;

            res *= mul;
            res %= mod;
            res += cur->info_mem[0][j].mem % mod;
            res %= mod;
        }
    }

    for (int i = 1; i < cur->mem_len; ++i) {
        for (int j = 0; j < cur->mem_mem_len[i]; ++j) {
            if (!cur->info_mem[i][j].is_dynamic) {
                res *= mul;
                res %= mod;
                res += cur->mem[i][j] % mod;
                res %= mod;

                res *= mul;
                res %= mod;
                res += cur->info_mem[i][j].mem % mod;
                res %= mod;
            }
        }
    }
    cur->hash = res;
} 

state* new_state() {
    state* cur = malloc(sizeof(state));
    for (int i = 0; i < 17; ++i) {
        cur->regs[i] = 0;
        cur->info_regs[i].is_dynamic = 0;
        cur->info_regs[i].mem = -1;
    }
    for (int i = 0; i < 64; ++i) {
        cur->flags[i] = 0;
    }
    cur->info_flags.is_dynamic = 0;
    cur->info_flags.mem = -1;

    cur->mem_len = 1;
    cur->mem_mem_len[0] = MAX_STACK_SIZE;

    cur->mem = malloc(sizeof(char*) * MAX_MEM_SIZE);
    cur->info_mem = malloc(sizeof(info*) * MAX_MEM_SIZE);
    cur->mem[0] = malloc(sizeof(char) * MAX_STACK_SIZE) + MAX_STACK_SIZE;
    cur->info_mem[0] = malloc(sizeof(info) * MAX_STACK_SIZE) + MAX_STACK_SIZE * sizeof(info);
    for (int i = -MAX_STACK_SIZE; i < 0; ++i) {
        cur->mem[0][i] = 0;
        cur->info_mem[0][i].mem = -1;
        cur->info_mem[0][i].is_dynamic = 0;
    }
    cur->info_regs[4].mem = 0;
    cur->info_regs[5].mem = 0;
    cur->next = NULL;
    cur->hash = 0;
    return cur;
}

state* crop(state* cur) {
    //MAYBE BUG
    //fprintf(stderr, "IN CROP\n");
    int delta = cur->regs[4];
    cur->mem_mem_len[0] += delta;
    cur->mem[0] += delta;
    for (int i = 0; i < 17; ++i) {
        if (cur->info_regs[i].mem == 0) {
            cur->regs[i] -= delta;
            //fprintf(stderr, "reg %d\n", i);
        }
    }

    for (int j = -cur->mem_mem_len[0]; j < 0; ++j) {
        if (cur->info_mem[0][j].mem == 0) {
            cur->mem[0][j] -= delta;
            //fprintf(stderr, "stack %d\n", j);
        }
    }
    //fprintf(stderr, "OUT CROP\n");
    // for (int i = 1; i < cur->mem_len; ++i) {
    //     for (int j = 0; j < cur->mem_mem_len[i]; ++j) {
    //         if (cur->info_mem[i][j].mem == 0) {
    //             cur->mem[i][j] += delta;
    //         }
    //     }
    // }
}

state* copy(state* cur) {
    state* new = new_state();
    for (int i = 0; i < 17; ++i) {
        new->regs[i] = cur->regs[i];
        new->info_regs[i].mem = cur->info_regs[i].mem;
        new->info_regs[i].is_dynamic = cur->info_regs[i].is_dynamic;
    }

    for (int i = 0; i < 64; ++i) {
        new->flags[i] = cur->flags[i];
    }

    new->info_flags.is_dynamic = cur->info_flags.is_dynamic;

    new->mem_len = cur->mem_len;
    for (int i = 0; i < cur->mem_len; ++i) {
        new->mem_mem_len[i] = cur->mem_mem_len[i];
    }

    int k = 1; 
    while (cur->mem_mem_len[k]) {
        new->mem[k] = malloc(cur->mem_mem_len[k]);
        new->info_mem[k] = malloc(sizeof(info) * cur->mem_mem_len[k]);
        k++;
    }
    for (int j = -cur->mem_mem_len[0]; j < 0; ++j) {
        new->mem[0][j] = cur->mem[0][j];
        new->info_mem[0][j].mem = cur->info_mem[0][j].mem;
        new->info_mem[0][j].is_dynamic = cur->info_mem[0][j].is_dynamic;
    }
    for (int i = 1; i < cur->mem_len; ++i) {
        for (int j = 0; j < cur->mem_mem_len[i]; ++j) {
            new->mem[i][j] = cur->mem[i][j];
            new->info_mem[i][j].mem = cur->info_mem[i][j].mem;
            new->info_mem[i][j].is_dynamic = cur->info_mem[i][j].is_dynamic;
        }
    }
    cur->next = new;
    new->next = cur;
    return new;
}

void print_reg(int i) {
    switch(i) {
        case(0):
            fprintf(stderr, "rax");
            break;
        case(1):
            fprintf(stderr, "rcx");
            break;
        case(2):
            fprintf(stderr, "rdx");
            break;
        case(3):
            fprintf(stderr, "rbx");
            break;
        case(4):
            fprintf(stderr, "rsp");
            break;
        case(5):
            fprintf(stderr, "rbp");
            break;
        case(6):
            fprintf(stderr, "rsi");
            break;
        case(7):
            fprintf(stderr, "rdi");
            break;
    }
}

unsigned char bit3_5(unsigned char b) {
    return (b & (7 << 3)) >> 3;
}

unsigned char bit1_2(unsigned char b) {
    return (b & (3 << 6)) >> 6;
}

unsigned char bit6_8(unsigned char b) {
    return b & 7;
}

unsigned char REXW() {
    return (REX & 8) >> 3;
}

unsigned char REXR() {
    return (REX & 4) >> 2;
}

unsigned char REXX() {
    return (REX & 2) >> 1;
}

unsigned char REXB() {
    return REX & 1;
}

unsigned char get_char() {
    unsigned char res = *((unsigned char*)RIP);
    RIP += sizeof(unsigned char);
    //fprintf(stderr, "read %#04x\n", res);
    return res;
}

unsigned char try_get_char() {
    unsigned char res = *((unsigned char*)RIP);
    //fprintf(stderr, "read %#04x\n", res);
    return res;
}

char read_reg() {
    unsigned char res = *((unsigned char*)RIP);
    return bit3_5(res);
}

char get_schar() {
    char res = *((char*)RIP);
    RIP += sizeof(char);
    //fprintf(stderr, "read %#04x\n", res);
    return res;
}

int int_8S() {
    return get_schar();
}

int int_8() {
    return get_char();
}

int int_16S() {
    int res = get_char();
    res += get_schar() << 8;
    return res;
}

int int_16() {
    int res = get_char();
    res += get_char() << 8;
    return res;
}

int int_32S() {
    int res = get_char();
    res += get_char() << 8;
    res += get_char() << 16;
    res += get_schar() << 24;
    return res;
}

unsigned int int_32() {
    int res = get_char();
    res += get_char() << 8;
    res += get_char() << 16;
    res += get_char() << 24;
    return res;
}

long long int_64S() {
    long res = get_char();
    res += get_char() << 8;
    res += get_char() << 16;
    res += get_char() << 24;
    res += (long long) get_char() << 32;
    res += (long long) get_char() << 40;
    res += (long long) get_char() << 48;
    res += (long long) get_schar() << 56;
    return res;
}

unsigned long long int_64() {
    long res = get_char();
    res += get_char() << 8;
    res += get_char() << 16;
    res += get_char() << 24;
    res += (long long) get_char() << 32;
    res += (long long) get_char() << 40;
    res += (long long) get_char() << 48;
    res += (long long) get_char() << 56;
    return res;
}

unsigned char cur, mod, reg, r_m, scale, _index, is_dynamic;
long long base;

value v;
param p1, p2;

char f;

void set_f() {
    f = 1;
}

void print_state() {
    return;
    fprintf(stderr, "RIP = val: %lld\n", st->regs[16]);
    for (int i = 0; i < 8; ++i) {
        fprintf(stderr, "%d) val: %lld, mem: %d, dyn: %d\n", i, st->regs[i], st->info_regs[i].mem, st->info_regs[i].is_dynamic);
    }
    for (int i = 0; i < 250; i += 4) {
        fprintf(stderr, "%d) val: %lld, mem: %d, dyn: %d\n", i, (long long)st->mem[0][-i], (int)st->info_mem[0][-i].mem, (int)st->info_mem[0][-i].is_dynamic);
    }
}

void print_params() {
    return;
    fprintf(stderr, "p1: reg1 = %d, reg2 = %d, base = %lld, scale = %d\n", (int) p1.reg1, (int) p1.reg2, (long long) p1.base, (int) p1.scale);
    fprintf(stderr, "p2: reg1 = %d, reg2 = %d, base = %lld, scale = %d\n", (int) p2.reg1, (int) p2.reg2, (long long) p2.base, (int) p2.scale);    
}

void print_value() {
    return;
    fprintf(stderr, "v: base = %lld, mem = %d, is_dynamic = %d\n", (long long) v.base, (int) v.mem, (int) is_dynamic);        
}

void write_byte(char byte) {
    return;
    *last = byte;
    //fprintf(stderr, "write_byte %02X\n", *last);
    last++;
}

void write_int(int value) {
    return;
    *((int *)last) = value;
    //fprintf(stderr, "write_byte %02X\n", *last);
    last += 4;
}

void write_params() {
    return;
    if (p1.reg2 != -1 || p1.scale != -1 || p1.base != 0) {
        fprintf(stderr, "ERROR print_params wrong p1\n");
        is_end = 1;
        return;
    }
    if ((p2.scale != 0 && p1.scale != -1) || p2.reg2 != -1) {
        fprintf(stderr, "ERROR print_params wrong p2\n");
        is_end = 1;
        return;
    }
    //fprintf(stderr, "p1: reg1 = %d, reg2 = %d, base = %lld, scale = %d\n", (int) p1.reg1, (int) p1.reg2, (long long) p1.base, (int) p1.scale);
    //fprintf(stderr, "p2: reg1 = %d, reg2 = %d, base = %lld, scale = %d\n", (int) p2.reg1, (int) p2.reg2, (long long) p2.base, (int) p2.scale);    

    if (p2.scale == 0) {
        if (p2.base == 0) {
            *last = (p1.reg1 << 3) + p2.reg1;
            //fprintf(stderr, "param1 %02X\n", *last);
            last++;
            return;
        }
        else if (p2.base < (1 << 8) && p2.base > -(1 << 8)) {
            *last = (1 << 6) + (p1.reg1 << 3) + p2.reg1;
            //fprintf(stderr, "param1 %02X\n", *last);
            last++;
            *last = p2.base;
            //fprintf(stderr, "param2 %02X\n", *last);
            last++;
            return;
        }
        else {
            *last = (2 << 6) + (p1.reg1 << 3) + p2.reg1;
            //fprintf(stderr, "param1 %02X\n", *last);
            last++;
            *((int*)last) = p2.base;
            //fprintf(stderr, "param2 %02X\n", *last);
            last += 4;
        }
    }
    if (p2.scale == -1) {
        *last = (3 << 6) + (p1.reg1 << 3) + p2.reg1;
        //fprintf(stderr, "param1 %02X\n", *last);
        last++;
    }

}


int (*cmd[256])();
state* que;

#define BIT8
#include "spec_funcs.c"
#undef BIT8
#define BIT32
#include "spec_funcs.c"
#undef BIT32
#define BIT64
#include "spec_funcs.c"
#undef BIT64
int i = 0;

const char * get_program();

typedef struct _code {
    char name[10];
    param p1;
    param p2;
    struct _code* next;
} code;

typedef struct _specialized_part {
    state* start_state;
    code* generated_code;
    state* end_state;
    struct _specialized_part* next;
} specialized_part;

typedef struct _state_stack {
    state* start_state;
    state* current_state;
    state* state_after_call;
    code* generated_code;
    state* parallel_state;
    state** result_place;
    struct _state_stack* next;
    specialized_part* specialized;
} state_stack;



int my_pow(int a, int b);

state* unite(state* a, state* b) {
    if (b == NULL) {
        return a;
    }
    for (int i = 0; i < 17; ++i) {
        if (a->regs[i]                 != b->regs[i]              ||
            a->info_regs[i].mem        != b->info_regs[i].mem     ||
            a->info_regs[i].is_dynamic != b->info_regs[i].is_dynamic) {
            a->info_regs[i].is_dynamic = 1;
        }
    }

    char f = a->info_flags.is_dynamic == b->info_flags.is_dynamic;
    for (int i = 0; i < 64; ++i) {
        f = f && (a->flags[i] == b->flags[i]);
    }
    if (!f) {
        a->info_flags.is_dynamic = 1;
    }
    // hard question 
    // new->mem_len = cur->mem_len;
    // for (int i = 0; i < cur->mem_len; ++i) {
    //     new->mem_mem_len[i] = cur->mem_mem_len[i];
    // }

    // int k = 1; 
    // while (cur->mem_mem_len[k]) {
    //     new->mem[k] = malloc(cur->mem_mem_len[k]);
    //     new->info_mem[k] = malloc(sizeof(info) * cur->mem_mem_len[k]);
    //     k++;
    // }
    for (int j = -a->mem_mem_len[0]; j < 0; ++j) {
        if (a->mem[0][j]                 != b->mem[0][j]              ||
            a->info_mem[0][j].mem        != b->info_mem[0][j].mem     ||
            a->info_mem[0][j].is_dynamic != b->info_mem[0][j].is_dynamic) {
            a->info_mem[0][j].is_dynamic = 1;
        }
    }
    for (int i = 1; i < a->mem_len; ++i) {
        for (int j = 0; j < a->mem_mem_len[i]; ++j) {
            if (a->mem[i][j]                 != b->mem[i][j]              ||
                a->info_mem[i][j].mem        != b->info_mem[i][j].mem     ||
                a->info_mem[i][j].is_dynamic != b->info_mem[i][j].is_dynamic) {
                a->info_mem[i][j].is_dynamic = 1;
            }
        }
    }
    return a;
}


char* spec(state* _state) {
    state* result = NULL;
    specialized_part* specialized = NULL;

    state_stack* stack = malloc(sizeof(state_stack));
    stack->start_state = copy(_state);
    stack->current_state = _state;
    stack->state_after_call = NULL;
    stack->generated_code = NULL; // empty
    stack->parallel_state = NULL;
    stack->result_place = &result;
    stack->next = NULL;

    res = mmap(NULL, 4096, PROT_EXEC | PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
    //res = malloc(1000);
    //fprintf(stderr, "res = %d\n", (int) res);
    if (res == -1) {
        fprintf(stderr, "%d", errno);
        return 0;
    }
    
    init(cmd);
    
    unsigned char cur;
    
    state* last;

    end: while(stack != NULL) {
        state_stack* current = stack;
        stack = stack->next;
        current->next = NULL;
        calc_hash(current->current_state);
        specialized_part* tmp = specialized;
        //fprintf(stderr, "OUR %d\n", current->current_state->regs[16]);
        while (tmp != NULL) {
            //fprintf(stderr, "IN QUEUE %d\n", tmp->start_state->regs[16]);
            if (tmp->start_state->hash == current->current_state->hash) {
                fprintf(stderr, "REPEAT\n");

                if (tmp->end_state == NULL) {
                    //MAKE ALL DYNAMIC
                    //ONLY RAX DYNAMIC FOR NOW
                    //tmp->end_state = copy(tmp->start_state);
                    fprintf(stderr, "ERROR 23k\n");
                    return 0;
                }

                *(current->result_place) = copy(tmp->end_state);
                goto end;
            }
            tmp = tmp->next;
        }

        fprintf(stderr, "\nStart block\n");
        is_end = 0;
        long long last = st->regs[16];
        specialized_part* used = malloc(sizeof(specialized_part));
        fprintf(stderr, "USED %d\n", current->start_state->regs[16]);
        used->start_state = current->start_state;
        used->generated_code = NULL;
        used->end_state = NULL;
        used->next = specialized;
        specialized = used;
        current->specialized = used;
        inner: while(1) {
            if (current->state_after_call != NULL) {

            //fprintf(stderr, "pp read %d \n", current->current_state->regs[16]);
                current->state_after_call->mem[0] = current->current_state->mem[0];
                current->state_after_call->info_mem[0] = current->current_state->info_mem[0];
                current->state_after_call->mem_mem_len[0] = current->current_state->mem_mem_len[0];
                current->state_after_call->regs[16] = current->current_state->regs[16];
                current->state_after_call->regs[4] = current->current_state->regs[4];
                current->state_after_call->regs[5] = current->current_state->regs[5];
                current->current_state = current->state_after_call;
                current->state_after_call = NULL;

            //fprintf(stderr, "pp read %d \n", RIP);
                //fprintf(stderr, "START FROM %d\n", RIP);
            }
            st = current->current_state;

            REX = 0;
            cur = get_char();
            if (cur >= 0x40 && cur <= 0x4f) {
                REX = cur;
                cur = get_char();
            }
            
            //fprintf(stderr, "%d cmd %#04x info: %d %d\n", i, cur, st->mem[0][-112], st->info_mem[0][-112].mem);

            i++;

            //new code
            if (cur == 0xe8) {
                fprintf(stderr, "IT'S NEW CALL HERE!!!\n");
                p1.reg1 = 16; //rip
                p1.reg2 = -1;
                p1.scale = -1;
                p1.base = 0;
                p2 = p1;

                int cur = int_32();
                eval(&p1);
                if (v.base + cur == my_malloc) {
                    
                    fprintf(stderr, "call malloc\n");
                    p1.reg1 = 7; //rdi
                    p1.reg2 = -1;
                    p1.scale = -1;
                    p1.base = 0;
                    eval(&p1);
                    st->mem[st->mem_len] = my_malloc(v.base);
                    st->info_mem[st->mem_len] = my_malloc(sizeof(info) * v.base);
                    st->mem_mem_len[st->mem_len] = v.base;
                    p1.reg1 = 0; //rax
                    p1.reg2 = -1;
                    p1.scale = -1;
                    p1.base = 0;
                    v.base = 0;
                    v.mem = st->mem_len;
                    assign(&p1);
                    ++(st->mem_len);

                    continue;
                }

                //fprintf(stderr, "END IN %d\n", RIP);

                current->next = stack;
                stack = current;

                copy(current->current_state);
                state* start = current->current_state->next; //do call in current stack copy and crop it to start
                st = start;
                push_64();
                eval(&p2);
                v.base += cur;
                assign(&p2);

                crop(st);
                //crop?

                //fprintf(stderr, "pp read %d \n", RIP);
                calc_hash(st);
                copy(start);
                state* start_copy = start->next;
                start_copy->hash = start->hash;

                specialized_part* tmp = specialized;

                while (tmp != NULL) {
                    if (tmp->start_state->hash == st->hash) {
                        fprintf(stderr, "RECURSIVE\n");

                        if (tmp->end_state == NULL) {
                            //
                            current->current_state->info_regs[0].is_dynamic = 1;
                            goto inner;
                        }
                    }
                    tmp = tmp->next;
                }

                state_stack* new = malloc(sizeof(state_stack));
                new->start_state = start;
                new->current_state = start_copy;
                new->state_after_call = NULL;
                new->generated_code = NULL; //empty
                new->parallel_state = NULL;
                new->result_place = & (current->state_after_call);

                new->next = stack;
                stack = new;
                break;
            }

            if (0x70 <= cur && cur <= 0x7f && st->info_flags.is_dynamic) {
                fprintf(stderr, "IT'S NEW DYJUMP HERE!!!\n");
                int cur = int_8S();

                current->next = stack;
                stack = current;

                copy(current->current_state);
                state* start = current->current_state->next; // do jump in current stack copy named start
                st = start;
                p1.reg1 = 16; //rip
                p1.reg2 = -1;
                p1.scale = -1;
                p1.base = 0;
                eval(&p1);
                v.base += cur;
                assign(&p1);

                calc_hash(st);

                copy(start);
                state* start_copy = start->next;
                start_copy->hash = start->hash;
                state_stack* new = malloc(sizeof(state_stack));
                new->start_state = start;
                new->current_state = start_copy;
                new->state_after_call = NULL;
                new->generated_code = NULL; //empty
                new->parallel_state = NULL;
                new->result_place = & (current->parallel_state);

                new->next = stack;
                stack = new;

                st = current->current_state;

                break;
            }

            if (cur == 0x0f && st->info_flags.is_dynamic) {
                int cur2 = try_get_char();
                if (0x80 <= cur2 && cur2 <= 0x8f) {
                    cur = get_char();
                    fprintf(stderr, "IT'S NEW DYJUMP HERE!!! %d\n", cur);
                    int cur = int_32();
                    
                    //copy

                    current->next = stack;
                    stack = current;

                    copy(current->current_state);
                    state* start = current->current_state->next; // do jump in current stack copy named start
                    st = start;
                    p1.reg1 = 16; //rip
                    p1.reg2 = -1;
                    p1.scale = -1;
                    p1.base = 0;
                    eval(&p1);
                    v.base += cur;
                    assign(&p1);

                    calc_hash(st->next);

                    copy(start);
                    state* start_copy = start->next;
                    start_copy->hash = start->hash;
                    
                    state_stack* new = malloc(sizeof(state_stack));
                    new->start_state = start;
                    new->current_state = start_copy;
                    new->state_after_call = NULL;
                    new->generated_code = NULL; //empty
                    new->parallel_state = NULL;
                    new->result_place = & (current->parallel_state);

                    new->next = stack;
                    stack = new;

                    st = current->current_state;

                    break;
                }
            }

            if (cur == 0xc3) {
                p1.reg1 = 0;//rax
                p1.reg2 = -1;
                p1.scale = -1;
                p1.base = 0;
                prefix(&p1);

                fprintf(stderr, "IT'S NEW RET HERE!!!\n");
                copy(current->current_state);
                state* result_state = unite(current->current_state->next, current->parallel_state);
                *(current->result_place) = result_state;

                current->specialized->generated_code = current->generated_code;
                current->specialized->end_state = current->current_state;
                break;
            }
            cmd[cur](cur);
            //new code
            

            if (is_end) {
                break;
            }
        }
    }
    //fprintf(stderr, "USED: %d\n", xxx);
    return res;
}