#include "flowChart_interpreter2.h"
#include "flowChart_parser.h"
#include "serealiser.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int MAX_BUFF_SIZE = 10000000;
long long opCount = 0;

int find(char** arr, int len, char* val) {
	for (int i = 0; i < len; ++i) {
		if (!strcmp(arr[i], val)) {
			return i;
		}
	}
	return -1;
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
			int f = 1;
			while ((int)l1 && (int)l2) {
				if (!constCmp(*(*l1).val, *(*l2).val)) {
					f = 0;
					break;
				}
				l1 = (*(l1)).next;
				l2 = (*(l2)).next;
			}
			if (!f) {
				return 0;
			}
			return (int)l1 == (int)l2;
		}
	}
}

// Const* copyConst(Const* c) {
// 	Const* res = malloc(sizeof(Const));
// 	if ((*c).type == NUMBER) {
// 		(*res).type = NUMBER;
// 		int* copy = malloc(sizeof(int));
// 		(*copy) = *((int*)(*c).expr);
// 		(*res).expr = copy;
// 		return res;
// 	}
// 	(*res).type = LIST;
// 	List l = *((List*)(*c).expr);
// 	List* list = malloc(sizeof(List));
// 	(*list).listLen = l.listLen;
// 	(*list).list = malloc(sizeof(Const*) * l.listLen);
// 	for (int i = 0; i < l.listLen; ++i) {
// 		(*list).list[i] = copyConst(l.list[i]);
// 	}
// 	(*res).expr = list;
// 	return res;
// }

// void freeConst(Const* c) {
// 	if ((*c).type == NUMBER) {
// 		free((*c).expr);
// 		free(c);
// 		return;
// 	}
// 	List* l = (List*)(*c).expr;
// 	for (int i = 0; i < (*l).listLen; ++i) {
// 		freeConst((*l).list[i]);
// 	}
// 	free((*l).list);
// 	free(l);
// 	free(c);
// }

// int find(char** arr, int len, char* val) {
// 	for (int i = 0; i < len; ++i) {
// 		if (!strcmp(arr[i], val)) {
// 			return i;
// 		}
// 	}
// 	return -1;
// }

int interpretExprInt (Expr, char**, int, Const***);
Const* interpretExpr (Expr, char**, int, Const***);

int interpretBopInt(Bop b, char** varNames, int len, Const*** state) {
	//fprintf(stderr, "BOP\n");
	++opCount;
	if (!strcmp(b.op, "_==")) {
		Const* left = interpretExpr(b.left, varNames, len, state);
		Const* right = interpretExpr(b.right, varNames, len, state);
		int res = constCmp((*left), (*right));
		//freeConst(left);
		//freeConst(right);
		return res;
	}
	int left = interpretExprInt(b.left, varNames, len, state);
	int right = interpretExprInt(b.right, varNames, len, state);
	if (!strcmp(b.op, "_*")) {
		return left * right;

	}
	if (!strcmp(b.op, "_<")) {
		return left < right;

	}
	if (!strcmp(b.op, "_-")) {
		return left - right;

	}
	// if (!strcmp(b.op, "_+")) {
		return left + right;
	//}
}

int interpretUopInt(Uop b, char** varNames, int len, Const*** state) {
	//fprintf(stderr, "UOP %s\n", b.op);
	++opCount;
	Const* left = interpretExpr(b.left, varNames, len, state);
	return *((int*)(*((*(*((List*)(*left).expr))).val)).expr);
}

int interpretExprInt (Expr e, char** varNames, int len, Const*** state) {
	//fprintf(stderr, "EXPR\n");
	++opCount;
	int i = 0;
	switch (e.type) {
		case CONST:
			return *((int*)(*((Const*)e.expr)).expr);
		case VAR:
			return *((int*)(*((*state)[find(varNames, len, (Var)e.expr)])).expr);
		case UOP:
			return interpretUopInt(*((Uop*)e.expr), varNames, len, state);
		case BOP:
			return interpretBopInt(*((Bop*)e.expr), varNames, len, state);
	}
}

int interpretIf(If i, char** varNames, int len, Const*** state) {
	//fprintf(stderr, "IF\n");
	int res = interpretExprInt(i.expr, varNames, len, state);
	if (res) {
		return i.ifTrue;
	}
	return i.ifFalse;
}

int interpretJump(Jump j, char** varNames, int len, Const*** state, Const** ans) {
	//fprintf(stderr, "JUMP\n");
	++opCount;
	Const* res;
	switch (j.type) {
		case GOTO:
			return *((GoTo*)j.jump);
		case RETURN:
			res = interpretExpr(*((Expr*)j.jump), varNames, len, state);
			*ans = res;
			return 0;
		case IF:
			return interpretIf(*((If*)j.jump), varNames, len, state);
	} 
}

Const* interpretBop(Bop b, char** varNames, int len, Const*** state) {
	//fprintf(stderr, "BOP\n");
	++opCount;
	if (!strcmp(b.op, "_==")) {
		Const* left = interpretExpr(b.left, varNames, len, state);
		Const* right = interpretExpr(b.right, varNames, len, state);
		Const* res = malloc(sizeof(Const));
		(*res).type = NUMBER;
		int* ans = malloc(sizeof(int));
		*ans = constCmp((*left), (*right));
		(*res).expr = ans;
		//freeConst(left);
		//freeConst(right);
		return res;
	}
	if (!strcmp(b.op, "_*")) {
		int left = interpretExprInt(b.left, varNames, len, state);
		int right = interpretExprInt(b.right, varNames, len, state);
		Const* res = malloc(sizeof(Const));
		(*res).type = NUMBER;
		int* ans = malloc(sizeof(int));
		*ans = left * right;
		(*res).expr = ans;
		return res;

	}
	if (!strcmp(b.op, "_<")) {
		int left = interpretExprInt(b.left, varNames, len, state);
		int right = interpretExprInt(b.right, varNames, len, state);
		Const* res = malloc(sizeof(Const));
		(*res).type = NUMBER;
		int* ans = malloc(sizeof(int));
		*ans = left < right;
		(*res).expr = ans;
		return res;

	}
	if (!strcmp(b.op, "_-")) {
		int left = interpretExprInt(b.left, varNames, len, state);
		int right = interpretExprInt(b.right, varNames, len, state);
		Const* res = malloc(sizeof(Const));
		(*res).type = NUMBER;
		int* ans = malloc(sizeof(int));
		*ans = left - right;
		(*res).expr = ans;
		return res;

	}
	if (!strcmp(b.op, "_+")) {
		int left = interpretExprInt(b.left, varNames, len, state);
		int right = interpretExprInt(b.right, varNames, len, state);
		Const* res = malloc(sizeof(Const));
		(*res).type = NUMBER;
		int* ans = malloc(sizeof(int));
		*ans = left + right;
		(*res).expr = ans;
		return res;
	}
	Const* left = interpretExpr(b.left, varNames, len, state);
	Const* right = interpretExpr(b.right, varNames, len, state);
	Const* res = malloc(sizeof(Const));
	(*res).type = LIST;
	List l = *((List*)(*right).expr);
	List* list = malloc(sizeof(List));
	Node* new = malloc(sizeof(Node));
	*list = new;
	(*new).next = l;
	(*new).val = left;
	(*res).expr = list;
	return res;
}

Const* interpretUop(Uop b, char** varNames, int len, Const*** state) {
	//fprintf(stderr, "UOP %s\n", b.op);
	++opCount;
	Const* left = interpretExpr(b.left, varNames, len, state);
	if (!strcmp(b.op, "^hd")) {
		Const* res = (*(*((List*)(*left).expr))).val;
		//Const* res = copyConst((*((List*)(*left).expr)).list[0]);
		//freeConst(left);
		return res;
	}
	if (!strcmp(b.op, "^tl")) {
		Const* res = malloc(sizeof(Const));
		(*res).type = LIST;
		List l = *((List*)(*left).expr);
		List* list = malloc(sizeof(Node*));
		*list = (*l).next;
		(*res).expr = list;
		return res;
	}
	//if (!strcmp(b.op, "^print")) {
	serealiseConst(*left);
	printf("\n");
	fflush(stdout);
	return left;
	//}
}

Const* interpretExpr (Expr e, char** varNames, int len, Const*** state) {
	//fprintf(stderr, "EXPR\n");
	++opCount;
	int i = 0;
	switch (e.type) {
		case CONST:
			return (Const*)e.expr;
		case VAR:
			++opCount;
			Const * res = (*state)[find(varNames, len, (Var)e.expr)];
			return res;
		case UOP:
			return interpretUop(*((Uop*)e.expr), varNames, len, state);
		case BOP:
			return interpretBop(*((Bop*)e.expr), varNames, len, state);
	}
}

void interpretAssignment(Assignment a, char** varNames, int len, Const*** state) {	
	//fprintf(stderr, "ASS\n");
	//serealiseAssignment(a);
	//fflush(stdout);
	int i = find(varNames, len, a.var);
	++opCount;
	Const* res = interpretExpr(a.expr, varNames, len, state);
	//freeConst((*state)[i]);
	(*state)[i] = res;
	++opCount;
}

int interpretBlock(BasicBlock b, char** varNames, int len, Const*** state, Const** ans) {
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
	Const** state = malloc(sizeof(Const*) * p.varCount);
	char* buff = malloc(sizeof(char) * MAX_BUFF_SIZE);
	for (int i = 0; i < p.varCount; ++i) {
		state[i] = malloc(sizeof(Const));
		(*(state[i])).type = NUMBER;
		(*(state[i])).expr = malloc(sizeof(int));
		*((int*)(*(state[i])).expr) = 0;
	}
	for (int i = 0; i < p.inputCount; ++i) {
		++opCount;
		char *b = buff;
		getline(&b, &MAX_BUFF_SIZE, stdin);
		state[find(p.varNames, p.varCount, p.input[i])] = parceConst(&buff);
	}
	Const* ans;
	BasicBlock cur = p.basicBlocks[0];
	while (1) {
		int res = interpretBlock(cur, p.varNames, p.varCount, &state, &ans);
		if (res == 0) {
			return *ans;
		}
		else {
			cur = findBlock(p.basicBlocks, p.blockCount, res);
		}
	}
}

void interpretTWO(Program p) {
	serealiseConst(interpretProgram(p));
	fprintf(stderr, "\nSTEPS: %lld\n", opCount);
}