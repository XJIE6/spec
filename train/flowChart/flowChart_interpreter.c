#include "flowChart_interpreter.h"
#include "flowChart_parser.h"
#include "serealiser.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int MAX_BUFF_SIZE = 100000;

int find(char** arr, int len, char* val) {
	for (int i = 0; i < len; ++i) {
		if (!strcmp(arr[i], val)) {
			return i;
		}
	}
	return -1;
}

Expr interpretExpr (Expr, char**, int, Const**);

int interpretIf(If i, char** varNames, int len, Const** state) {
	//fprintf(stderr, "IF\n");
	if (*((int *)((*((Const*) interpretExpr(i.expr, varNames, len, state).expr)).expr))) {
		return i.ifTrue;
	}
	return i.ifFalse;
}

int interpretJump(Jump j, char** varNames, int len, Const** state, Const* ans) {
	//fprintf(stderr, "JUMP\n");
	switch (j.type) {
		case GOTO:
			return *((GoTo*)j.jump);
		case RETURN:
			(*ans) = *((Const*) interpretExpr(*((Expr*)j.jump), varNames, len, state).expr);
			return 0;
		case IF:
			return interpretIf(*((If*)j.jump), varNames, len, state);
	} 
}

int constCmp(Const c1, Const c2) {
	if (c1.type == NUMBER) {
		if(c2.type == NUMBER) {
			return *((int*)c1.expr) == *((int*)c2.expr);
		}
		else {
			return 0;
		}
	}
	else {
		if(c2.type == NUMBER) {
			return 0;
		}
		else {
			List l1 = *((List*) c1.expr);
			List l2 = *((List*) c2.expr);
			if (l1.listLen != l2.listLen) {
				return 0;
			}
			int f = 1;
			for (int i = 0; i < l1.listLen; ++i) {
				if (!constCmp(l1.list[i], l2.list[i])) {
					f = 0;
					break;
				}
			}
			return f;
		}
	}
}

Expr interpretBop(Bop b, char** varNames, int len, Const** state) {
	//fprintf(stderr, "BOP\n");
	Expr leftE = interpretExpr(b.left, varNames, len, state);
	Expr rightE = interpretExpr(b.right, varNames, len, state);
	Expr ee;
	if (leftE.type == CONST && rightE.type == CONST) {
		Const left = *((Const*) leftE.expr);
		Const right = *((Const*) rightE.expr);
		ee.type = CONST;
		Const* res = malloc(sizeof(Const));
		if (!strcmp(b.op, "_==")) {
			(*res).type = NUMBER;
			int* ans = malloc(sizeof(int));
			*ans = constCmp(left, right);
			(*res).expr = ans;
		}
		if (!strcmp(b.op, "_*")) {
			(*res).type = NUMBER;
			int* ans = malloc(sizeof(int));
			*ans = *((int*)left.expr) * *((int*)right.expr);
			(*res).expr = ans;
		}
		if (!strcmp(b.op, "_<")) {
			(*res).type = NUMBER;
			int* ans = malloc(sizeof(int));
			*ans = *((int*)left.expr) < *((int*)right.expr);
			(*res).expr = ans;
		}
		if (!strcmp(b.op, "_-")) {
			(*res).type = NUMBER;
			int* ans = malloc(sizeof(int));
			*ans = *((int*)left.expr) - *((int*)right.expr);
			(*res).expr = ans;
		}
		if (!strcmp(b.op, "_+")) {
			(*res).type = NUMBER;
			int* ans = malloc(sizeof(int));
			*ans = *((int*)left.expr) + *((int*)right.expr);
			(*res).expr = ans;
		}
		if (!strcmp(b.op, "_cons")) {
			(*res).type = LIST;
			List l = *((List*)right.expr);
			List* new = malloc(sizeof(List));
			(*new).listLen = l.listLen + 1;
			(*new).list = malloc(sizeof(Const) * (*new).listLen);
			(*new).list[0] = left;
			memcpy(&((*new).list[1]), l.list, sizeof(Const) * l.listLen);
			(*res).expr = new;
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

Expr interpretUop(Uop b, char** varNames, int len, Const** state) {
	//fprintf(stderr, "UOP\n");
	Expr leftE = interpretExpr(b.left, varNames, len, state);
	Expr ee;
	if (leftE.type == CONST) {
		Const left = *((Const*) leftE.expr);
		ee.type = CONST;
		Const* res;
		if (!strcmp(b.op, "^hd")) {
			res = (*((List*) left.expr)).list;
		}
		if (!strcmp(b.op, "^tl")) {
			res = malloc(sizeof(Const));
			(*res).type = LIST;
			List l = *((List*)left.expr);
			List* new = malloc(sizeof(List));
			(*new).listLen = l.listLen - 1;
			(*new).list = &(l.list[1]);
			(*res).expr = new;
		}
		if (!strcmp(b.op, "^print")) {
			serealiseConst(left);
			printf("\n");
			fflush(stdout);
			res = &left;
		}
		ee.expr = res;
		return ee;
	}
	ee.type = UOP;
	Uop* bb = malloc(sizeof(Uop));
	ee.expr = bb;
	(*bb).op = b.op;
	(*bb).left = leftE;
	return ee;
}

Expr interpretExpr (Expr e, char** varNames, int len, Const** state) {
	//fprintf(stderr, "EXPR\n");
	int i;
	switch (e.type) {
		case CONST:
			return e;
		case VAR:
			i = find(varNames, len, (Var)e.expr);
			if (i != -1 && ((*state)[i].type != NUMBER || *((int*)(*state)[i].expr) != -1)) {
				Expr ee;
				ee.type = CONST;
				Const *res = malloc(sizeof(Const));
				*res = (*state)[i];
				ee.expr = res;
				return ee;
			}
			else {
				return e;
			}
		case UOP:
			return interpretUop(*((Uop*)e.expr), varNames, len, state);
		case BOP:
			return interpretBop(*((Bop*)e.expr), varNames, len, state);
	}
}

void interpretAssignment(Assignment a, char** varNames, int len, Const** state) {	
	//fprintf(stderr, "ASS\n");
	(*state)[find(varNames, len, a.var)] = *((Const*)interpretExpr(a.expr, varNames, len, state).expr);
}

int interpretBlock(BasicBlock b, char** varNames, int len, Const** state, Const* ans) {
	//fprintf(stderr, "BLOCK\n%d\n", b.label);
	for (int i = 0; i < b.assignmentCount; ++i) {
		interpretAssignment(b.assignments[i], varNames, len, state);
	}
	return interpretJump(b.jump, varNames, len, state, ans);
}

BasicBlock findBlock(BasicBlock* arr, int len, Label label) {
	for (int i = 0; i < len; ++i) {
		if (arr[i].label == label) {
			return arr[i];
		}
	}
}

Const interpretProgram(Program p) {
	//fprintf(stderr, "PROGRAM\n");
	Const* state = malloc(sizeof(Const) * p.varCount);
	int* minus = malloc(sizeof(int));
	*minus = -1;
	for (int i = 0; i < p.varCount; ++i) {
		state[i].type = NUMBER;
		state[i].expr = minus;
	}
	for (int i = 0; i < p.inputCount; ++i) {
		char* buff = malloc(sizeof(char) * MAX_BUFF_SIZE);
		char *b = buff;
		getline(&b, &MAX_BUFF_SIZE, stdin);
		state[find(p.varNames, p.varCount, p.input[i])] = parceConst(&buff);
	}
	Const* ans = malloc(sizeof(Const));
	BasicBlock cur = p.basicBlocks[0];
	while (1) {
		int res = interpretBlock(cur, p.varNames, p.varCount, &state, ans);
		if (res == 0) {
			return *ans;
		}
		else {
			cur = findBlock(p.basicBlocks, p.blockCount, res);
		}
	}
}

void interpret(Program p) {
	serealiseConst(interpretProgram(p));
}