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

typedef struct _state state;

state* state_init(long long len, long long* args);