#include "state.h"
#include "stddef.h"
#include "stdio.h"

const int MAX_STACK_SIZE = 1000;
const int MAX_MEM_SIZE = 10;

state_stack* init_state_stack(state* start_state) {
    state_stack* stack = malloc(sizeof(state_stack));
    stack->start_state = copy(start_state);
    stack->current_state = start_state;
    stack->state_after_call = NULL;
    stack->generated_code = NULL; // empty
    stack->parallel_state = NULL;
    stack->result_place = malloc(sizeof(state));
    stack->next = NULL;
    return stack;
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

void crop(state* cur) {
    //MAYBE BUG
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
            char c = cur->info_mem[i][j].mem;
            new->info_mem[i][j].mem = c;
            new->info_mem[i][j].is_dynamic = cur->info_mem[i][j].is_dynamic;
        }
    }
    cur->next = new;
    new->next = cur;
    return new;
}


void compare(state* st1, state* st2) {
    fprintf(stderr, "\ncompare %d %d\n", st1->hash, st2->hash);
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