#include <stdio.h>
#include <limits.h>
#include <sys/mman.h>
#include <errno.h>
//#include <curses.h>
#include "spec.h"
#include "state.h"
#include "machine_code.h"

typedef struct _specialized_part {
    state* start_state;
    code_block* generated_code;
    state* end_state;
    struct _specialized_part* next;
} specialized_part;

typedef struct _state_stack {
    state* start_state;
    state* current_state;
    state* state_after_call;
    code_block* generated_code;
    state* parallel_state;
    state** result_place;
    struct _state_stack* next;
    specialized_part* specialized;
} state_stack;

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
    
    //init(cmd);
    
    unsigned char cur;
    
    state* last;

    end: while(stack != NULL) {
        state_stack* current = stack;
        stack = stack->next;
        current->next = NULL;
        calc_hash(current->current_state);
        specialized_part* tmp = specialized;
        while (tmp != NULL) {
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

            //fprintf(stderr, "%d cmd %#04x\n", i, cur);
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