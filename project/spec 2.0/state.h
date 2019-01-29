#pragma once

typedef struct _info {
    char is_dynamic;
    int mem;
} info;

struct _state {
    long long regs[17];
    info info_regs[17];
    char flags[64];
    info info_flags;
    char** mem;
    info** info_mem;
    int mem_len;
    int mem_mem_len[100];
    struct _state* next;
    long long hash;
};

typedef struct _param {
    char reg1;
    char reg2;
    char scale;
    long long base;
} param;

typedef struct _value {
    long long base;
    char mem;
    unsigned char is_dynamic;
} value;

typedef struct _code {
    unsigned char REX;
    unsigned char pre;
    unsigned char number;
    param p1;
    param p2;
    long long base;
    struct _code* next;
} code;

typedef struct _state state;

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

state* copy(state* cur);