#pragma once

#include "state.h"

unsigned char get_char(state* st);

unsigned char try_get_char(state* st);

char read_reg(state* st);

char get_schar(state* st);

int int_8S(state* st);

int int_8(state* st);

int int_16S(state* st);

int int_16(state* st);

int int_32S(state* st);

unsigned int int_32(state* st);

long long int_64S(state* st);

unsigned long long int_64(state* st);

void parce_reg_mem(state* st, code* instruction);

void my_clf(state* st);

unsigned char bit3_5(unsigned char b);

unsigned char bit1_2(unsigned char b);

unsigned char bit6_8(unsigned char b);

unsigned char REXW(char REX);

unsigned char REXR(char REX);

unsigned char REXX(char REX);

unsigned char REXB(char REX);