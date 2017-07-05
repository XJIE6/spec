#include "serealiser.h"
#include <stdio.h>

void serealiseExpr (Expr);

void serealiseIf(If i) {
	printf("if ");
	serealiseExpr(i.expr);
	printf(" then %d ", i.ifTrue);
	printf("else %d.\n\n", i.ifFalse);
}

void serealiseJump(Jump j) {
	switch (j.type) {
		case GOTO:
			printf("goto %d.\n\n", *((GoTo*)j.jump));
			break;
		case RETURN:
			printf("return ");
			serealiseExpr(*((Expr*)j.jump));
			printf(".\n\n");
			break;
		case IF:
			serealiseIf(*((If*)j.jump));
			break;
	} 
}

void serealiseBop(Bop b) {
	printf("%s ", b.op);
	serealiseExpr(b.left);
	printf(" ", b.op);
	serealiseExpr(b.right);
}

void serealiseExpr (Expr e) {
	switch (e.type) {
		case CONST:
			printf("%d", *((Const*)e.expr));
			break;
		case VAR:
			printf("%s", (Var)e.expr);
			break;
		case BOP:
			serealiseBop(*((Bop*)e.expr));
			break;
	} 
}

void serealiseAssignment(Assignment a) {	
	printf("%s := ", a.var);
	serealiseExpr(a.expr);
	printf(";\n");
}

void serealiseBlock(BasicBlock b) {
	printf("%d:\n", b.label);
	for (int i = 0; i < b.assignmentCount; ++i) {
		serealiseAssignment(b.assignments[i]);
	}
	serealiseJump(b.jump);
}

void serealiseProgram(Program p) {	
	printf("read ");
	for (int i = 0; i < p.inputCount; ++i) {
		if (i != 0) {
			printf(",");
		}
		printf("%s", p.input[i]);
	}
	printf(";\n\n");
	for (int i = 0; i < p.blockCount; ++i) {
		serealiseBlock(p.basicBlocks[i]);
	}
}

void serealise(Program p) {
	serealiseProgram(p);
}