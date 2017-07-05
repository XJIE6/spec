#include "flowChart_interpreter.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int find(char** arr, int len, char* val) {
	for (int i = 0; i < len; ++i) {
		if (!strcmp(arr[i], val)) {
			return i;
		}
	}
	return -1;
}

Expr interpretExpr (Expr, char**, int, int**);

int interpretIf(If i, char** varNames, int len, int** state) {
	if (*((int*) interpretExpr(i.expr, varNames, len, state).expr)) {
		return i.ifTrue;
	}
	return i.ifFalse;
}

int interpretJump(Jump j, char** varNames, int len, int** state) {
	switch (j.type) {
		case GOTO:
			return *((GoTo*)j.jump);
		case RETURN:
			return -*((int*) interpretExpr(*((Expr*)j.jump), varNames, len, state).expr);
		case IF:
			return interpretIf(*((If*)j.jump), varNames, len, state);
	} 
}

Expr interpretBop(Bop b, char** varNames, int len, int** state) {
	Expr leftE = interpretExpr(b.left, varNames, len, state);
	Expr rightE = interpretExpr(b.right, varNames, len, state);
	Expr ee;
	if (leftE.type == CONST && rightE.type == CONST) {
		int left = *((int*) leftE.expr);
		int right = *((int*) rightE.expr);
		ee.type = CONST;
		int *res = malloc(sizeof(int));
		if (!strcmp(b.op, "_==")) {
			*(res) = left == right;
		}
		if (!strcmp(b.op, "_*")) {
			*(res) = left * right;
		}
		if (!strcmp(b.op, "_-")) {
			*(res) = left - right;
		}
		if (!strcmp(b.op, "_+")) {
			*(res) = left + right;
		}
		ee.expr = res;
		return ee;
	}
	ee.type = BOP;
	Bop* bb = malloc(sizeof(Bop));
	ee.expr = bb;
	(*bb).op = b.op;
	(*bb).left = leftE;
	(*bb).right = rightE;
	return ee;
}

Expr interpretExpr (Expr e, char** varNames, int len, int** state) {
	int i;
	switch (e.type) {
		case CONST:
			return e;
		case VAR:
			i = find(varNames, len, (Var)e.expr);
			if (i != -1 && (*state)[i] != -1) {
				Expr ee;
				ee.type = CONST;
				int *res = malloc(sizeof(int));
				*res = (*state)[i];
				ee.expr = res;
				return ee;
			}
			else {
				return e;
			}
		case BOP:
			return interpretBop(*((Bop*)e.expr), varNames, len, state);
	} 
}

void interpretAssignment(Assignment a, char** varNames, int len, int** state) {	
	(*state)[find(varNames, len, a.var)] = *((int*)interpretExpr(a.expr, varNames, len, state).expr);
}

int interpretBlock(BasicBlock b, char** varNames, int len, int** state) {
	for (int i = 0; i < b.assignmentCount; ++i) {
		interpretAssignment(b.assignments[i], varNames, len, state);
	}
	return interpretJump(b.jump, varNames, len, state);
}

BasicBlock findBlock(BasicBlock* arr, int len, Label label) {
	for (int i = 0; i < len; ++i) {
		if (arr[i].label == label) {
			return arr[i];
		}
	}
}

int interpretProgram(Program p) {
	int* state = malloc(sizeof(int) * p.varCount);
	for (int i = 0; i < p.varCount; ++i) {
		state[i] = 0;
	}
	for (int i = 0; i < p.inputCount; ++i) {
		scanf("%d", &state[find(p.varNames, p.varCount, p.input[i])]);
	}
	BasicBlock cur = p.basicBlocks[0];
	while (1) {
		int res = interpretBlock(cur, p.varNames, p.varCount, &state);
		if (res < 0) {
			return -res;
		}
		else {
			cur = findBlock(p.basicBlocks, p.blockCount, res);
		}
	}
}

void interpret(Program p) {
	printf("%d", interpretProgram(p));
}