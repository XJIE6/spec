#pragma once

value eval_8(state* st, param p);
value eval_32(state* st, param p);
value eval_64(state* st, param p);

void assign_8(state* st, param p, value v);
void assign_32(state* st, param p, value v);
void assign_64(state* st, param p, value v);

value pop_8(state* st);
value pop_32(state* st);
value pop_64(state* st);

void push_8(state* st, value v);
void push_32(state* st, value v);
void push_64(state* st, value v);

value lea_8(state* st, param p);
value lea_32(state* st, param p);
value lea_64(state* st, param p);

void dynamic(state* st, param p);