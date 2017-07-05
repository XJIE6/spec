#pragma once
#include "flowChart.h"
void interpret(Program);
Expr interpretExpr(Expr, char**, int, int**);
BasicBlock findBlock(BasicBlock*, int, Label);
int find(char**, int, char*);