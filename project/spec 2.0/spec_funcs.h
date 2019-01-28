#pragma once

value eval_8(state* st, param p);
value eval_32(state* st, param p);
value eval_64(state* st, param p);

void assign_8(state* st, param p, value v);
void assign_32(state* st, param p, value v);
void assign_64(state* st, param p, value v);

void dynamic(state* st, param p);