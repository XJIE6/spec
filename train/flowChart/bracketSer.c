#include "bracketSer.h"
#include "flowChart_interpreter2.h"
#include <stdio.h>
#include <string.h>

char** vars;
int len;

int findVar(char* var) {
	return find(vars, len, var);
}

void serealiseBExpr (Expr);

void serealiseBIf(If i) {
	printf("(1 ");
	serealiseBExpr(i.expr);
	printf(" %d ", i.ifTrue);
	printf(" %d)", i.ifFalse);
}

void serealiseBJump(Jump j) {
	switch (j.type) {
		case GOTO:
			printf("(0 %d)", *((GoTo*)j.jump));
			break;
		case RETURN:
			printf("(2 ");
			serealiseBExpr(*((Expr*)j.jump));
			printf(")");
			break;
		case IF:
			serealiseBIf(*((If*)j.jump));
			break;
	} 
}

void serealiseBBop(Bop b) {
	if (!strcmp(b.op, "_==")) {
		printf("(3 0 ");
	}
	if (!strcmp(b.op, "_*")) {
		printf("(3 1 ");
	}
	if (!strcmp(b.op, "_<")) {
		printf("(3 2 ");
	}
	if (!strcmp(b.op, "_-")) {
		printf("(3 3 ");
	}
	if (!strcmp(b.op, "_+")) {
		printf("(3 4 ");
	}
	if (!strcmp(b.op, "_cons")) {
		printf("(3 5 ");
	}
	serealiseBExpr(b.left);
	printf(" ");
	serealiseBExpr(b.right);
	printf(")");
}

void serealiseBUop(Uop b) {
	if (!strcmp(b.op, "^hd")) {
		printf("(2 0 ");
	}
	if (!strcmp(b.op, "^tl")) {
		printf("(2 1 ");
	}
	if (!strcmp(b.op, "^print")) {
		printf("(2 2 ");
	}
	serealiseBExpr(b.left);
	printf(")");
}

void serealiseBConst (Const c);

void serealiseBList (List l) {
	printf("(0 (");
	while (l != NULL) {
		printf(" ");
		serealiseBConst(*((*l).val));
		l = (*l).next;
	}
	printf("))");
}

void serealiseBConst (Const c) {
	switch (c.type) {
		case NUMBER:
			printf("(0 %d)", *((int*)c.expr));
			break;
		case LIST:
			serealiseBList(*((List*)c.expr));
			break;
	}
}

void serealiseBVar(Var v) {
	printf("(");
	while (*v != 0) {
		printf("%d ", *v);
		++v;
	}
	printf(")");
}

void serealiseBExpr (Expr e) {
	switch (e.type) {
		case CONST:
			serealiseBConst(*((Const*)e.expr));
			break;
		case VAR:
			printf("(1 ");
			serealiseBVar((Var)e.expr);
			printf(")");
			break;
		case BOP:
			serealiseBBop(*((Bop*)e.expr));
			break;
		case UOP:
			serealiseBUop(*((Uop*)e.expr));
			break;
	} 
}

void serealiseBAssignment(Assignment a) {
	printf("( ");
	serealiseBVar(a.var);
	serealiseBExpr(a.expr);
	printf(")");
}

void serealiseBBlock(BasicBlock b) {
	printf("(%d(", b.label);
	for (int i = 0; i < b.assignmentCount; ++i) {
		serealiseBAssignment(b.assignments[i]);
	}
	printf(")");
	serealiseBJump(b.jump);	
	printf(")");
}

void serealiseBProgram(Program p) {
	vars = p.varNames;
	len = p.varCount;
	printf("((");
	for (int i = 0; i < p.inputCount; ++i) {
		serealiseBVar(p.input[i]);
	}
	printf(")(");
	for (int i = 0; i < p.blockCount; ++i) {
		serealiseBBlock(p.basicBlocks[i]);
	}
	printf("))\n\n\n");
	for (int i = 0; i < p.varCount; ++i) {
		printf("%s ", p.varNames[i]);
		serealiseBVar(p.varNames[i]);
		printf("\n");
	}
}

void serealiseB(Program p) {
	serealiseBProgram(p);
}