#include <stdio.h>
#include <limits.h>
#include <sys/mman.h>
#include <errno.h>
//#include <curses.h>
#include "spec.h"
#include "state.h"
#include "machine_code.h"
#include "registers.h"
#include "spec_funcs.h"
#include "my_malloc.h"

char* spec(state* _state) {
    int i = 0;
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
            unsigned char current_instruction = get_char(current->current_state);
            if (current_instruction >= 0x40 && current_instruction <= 0x4f) {
                REX = current_instruction;
                current_instruction = get_char(current->current_state);
            }

            fprintf(stderr, "%d cmd %#04x, %d\n", i++, current_instruction, current->current_state->regs[4]);

            //call
            if (current_instruction == 0xe8) {
                long long alignment = int_32S(current->current_state);
                value v = eval_64(current->current_state, RIP);
                if (v.base + alignment == my_malloc) {
                    //TODO generate malloc instruction
                    v = eval_64(current->current_state, RDI);
                    current->current_state->mem[current->current_state->mem_len] = my_malloc(v.base);
                    current->current_state->info_mem[current->current_state->mem_len] = my_malloc(sizeof(info) * v.base);
                    current->current_state->mem_mem_len[current->current_state->mem_len] = v.base;
                    v.base = 0;
                    v.mem = current->current_state->mem_len;
                    v.is_dynamic = 0;
                    assign_64(current->current_state, RAX, v);
                    ++(current->current_state->mem_len);
                    continue;
                }

                //TODO
                // put current state on stack
                current->next = stack;
                stack = current;
                // create new state
                state* start = copy(current->current_state);
                push_64(start, v);
                v.base += alignment;
                assign_64(start, RIP, v);
                crop(start);
                calc_hash(start);
                //check if recursive call
                //prbably redunant
                specialized_part* tmp = specialized;
                while (tmp != NULL) {
                    if (tmp->start_state->hash == start->hash) {
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
                long long alignment = int_8S(start);
                value v = eval_64(start, RIP);
                v.base += alignment;
                assign_64(start, RIP, v);
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
                int cur2 = try_get_char(current->current_state);
                if (0x80 <= cur2 && cur2 <= 0x8f) {
                    current_instruction = get_char(current->current_state);
                    //put current state on stack
                    current->next = stack;
                    stack = current;
                    state* start = copy(current->current_state);
                    long long alignment = int_32S(start);
                    value v = eval_64(start, RIP);
                    v.base += alignment;
                    assign_64(start, RIP, v);
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
                prefix_64(current->current_state, RAX);
                state* new = copy(current->current_state);
                state* result_state = unite(new, current->parallel_state);
                *(current->result_place) = result_state;

                current->specialized->generated_code = current->generated_code;
                current->specialized->end_state = current->current_state;
                break;
            }
            code* cur = malloc(sizeof(code));
            cur->number = current_instruction;
            int x = eval_instruction(current->current_state, cur);
            if (x == 1) {
                return NULL;
            }
        }
    }
    return NULL;
}