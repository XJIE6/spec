#pragma once
#include "flowChart.h"
void interpret(Program);
int find(char**, int, char*);
Expr interpretExpr (Expr, char**, int, Const**);
BasicBlock findBlock(BasicBlock*, int, Label);
int MAX_BUFF_SIZE;