#include <stdio.h>
#include <limits.h>
#include <sys/mman.h>
#include <errno.h>
//#include <curses.h>
#include "spec.h"
#include "state.h"
#include "machine_code.h"

typedef struct _code {
    char name[10];
    param p1;
    param p2;
    struct _code* next;
} code;

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

char* spec(state* _state) {
    specialized_part* specialized = NULL;

    state_stack* stack = init_state_stack(_state);

    main_loop: while(stack != NULL) {
        //get top state from stack
        state_stack* current = stack;
        stack = stack->next;
        current->next = NULL;
        //check if it is already specialized
        calc_hash(current->current_state);
        specialized_part* tmp = specialized;
        while (tmp != NULL) {
            if (tmp->start_state->hash == current->current_state->hash) {
                if (tmp->end_state == NULL) {
                    //means that current function is in specialization stack
                    //TODO
                    //MAKE ALL DYNAMIC
                    //ONLY RAX DYNAMIC FOR NOW
                    //tmp->end_state = copy(tmp->start_state);
                    fprintf(stderr, "ERROR RECURSIVE\n");
                    return 0;
                }
                //means that already specialized
                *(current->result_place) = copy(tmp->end_state);
                goto main_loop;
            }
            tmp = tmp->next;
        }
        //mark state as specialized
        specialized_part* used = malloc(sizeof(specialized_part));
        used->start_state = copy(current->start_state);
        used->generated_code = NULL;
        used->end_state = NULL;
        used->next = specialized;
        specialized = used;
        current->specialized = used;

        state_loop: while(1) {
            if (current->state_after_call != NULL) {
                //means that specialization of this block was interrupted by specializeion call
                //need to continue specialization basing on state after call
                //TODO inline check
                //TODO move call writing here (it may be ambigious because of inlining)
                current->state_after_call->mem[0] = current->current_state->mem[0];
                current->state_after_call->mem_mem_len[0] = current->current_state->mem_mem_len[0];
                current->state_after_call->regs[16] = current->current_state->regs[16];
                current->state_after_call->regs[4] = current->current_state->regs[4];
                current->state_after_call->regs[5] = current->current_state->regs[5];
                current->current_state = current->state_after_call;
                current->state_after_call = NULL;
            }

            char REX = 0;
            char current_instrction = get_char(current->current_state);
            if (cur >= 0x40 && cur <= 0x4f) {
                REX = cur;
                cur = get_char(current->current_state);
            }

            //call
            if (current_instruction == 0xe8) {
                value v = eval(current->current_state, &RIP);
                long long address = v->base + int_32(current->current_state);
                if (address == my_malloc) {
                    //TODO generate malloc instruction
                    v = eval(current->current_state, &RDI);
                    st->mem[st->mem_len] = my_malloc(v.base);
                    st->info_mem[st->mem_len] = my_malloc(sizeof(info) * v.base);
                    st->mem_mem_len[st->mem_len] = v.base;
                    v.base = 0;
                    v.mem = st->mem_len;
                    v.is_dynamic = false;
                    assign(current->current_state, &RAX, v);
                    ++(st->mem_len);
                    continue;
                }
                //TODO
                // put current state on stack
                current->next = stack;
                stack = current;
                // create new state
                state* start = copy(current->current_state);
                push_64(start);
                v = eval(&RIP);
                v.base += code->address;
                assign(start, &RIP, v);
                crop(start);
                calc_hash(start);
                //check if recursive call
                //prbably redunant
                specialized_part* tmp = specialized;
                while (tmp != NULL) {
                    if (tmp->start_state->hash == st->hash) {
                        if (tmp->end_state == NULL) {
                            current->current_state->info_regs[0].is_dynamic = 1;
                            goto state_loop;
                        }
                    }
                    tmp = tmp->next;
                }

                state* start_copy = copy(start);
                start_copy->hash = start->hash;

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
            //jump while flags is dynamic
            if (0x70 <= current_instruction && current_instruction <= 0x7f && 
                current->current_state->info_flags.is_dynamic) {
                //put current state on stack
                current->next = stack;
                stack = current;
                state* start = copy(current->current_state);
                value v = eval(start, &RIP);
                v.base += int_8S();
                assign(start, &RIP, v);
                calc_hash(start);

                state* start_copy = copy(start);
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
                break;
            }
            //jump while flags is dynamic
            if (current_instruction == 0x0f && 
                current->current_state->info_flags.is_dynamic) {
                int cur2 = try_get_char();
                if (0x80 <= cur2 && cur2 <= 0x8f) {
                    cur = get_char();
                    //put current state on stack
                    current->next = stack;
                    stack = current;
                    state* start = copy(current->current_state);
                    value v = eval(start, &RIP);
                    v.base += int_32();
                    assign(start, &RIP, v);
                    calc_hash(start);

                    state* start_copy = copy(start);
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
                    break;
                }
            }

            if (current_instruction == 0xc3) {
                prefix(&RAX);
                state* new = copy(current->current_state);
                state* result_state = unite(new, current->parallel_state);
                *(current->result_place) = result_state;

                current->specialized->generated_code = current->generated_code;
                current->specialized->end_state = current->current_state;
                break;
            }
            cmd[cur](cur);
        }
    }
}