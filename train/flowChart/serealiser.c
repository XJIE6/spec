#include "serealiser.h"
#include <stdio.h>

void serealiseExpr (Expr);

void serealiseIf(If i) {
	printf("If\n");
	serealiseExpr(i.expr);
	printf("IFTRUE\n%d\n", i.ifTrue);
	printf("IFFALSE\n%d\n", i.ifFalse);
}

void serealiseJump(Jump j) {
	printf("JUMP\n");
	switch (j.type) {
		case GOTO:
			printf("GOTO\n%d\n", *((GoTo*)j.jump));
			break;
		case RETURN:
			printf("RETURN\n");
			serealiseExpr(*((Expr*)j.jump));
			break;
		case IF:
			serealiseIf(*((If*)j.jump));
			break;
	} 
}

void serealiseBop(Bop b) {
	printf("BOP\n%s\n", b.op);
	serealiseExpr(b.left);
	serealiseExpr(b.right);
}

void serealiseExpr (Expr e) {
	printf("EXPR\n");
	switch (e.type) {
		case CONST:
			printf("CONST\n%d\n", *((Const*)e.expr));
			break;
		case VAR:
			printf("VAR\n%s\n", (Var)e.expr);
			break;
		case BOP:
			serealiseBop(*((Bop*)e.expr));
			break;
	} 
}

void serealiseAssignment(Assignment a) {	
	printf("ASSIGNMENT\n");
	printf("VAR\n");
	printf("%s\n", a.var);
	serealiseExpr(a.expr);	
}

void serealiseBlock(BasicBlock b) {
	printf("BLOCK\n");
	printf("LABEL\n");
	printf("%d\n", b.label);
	printf("ASSIGNMENTS\n");
	printf("%d\n", b.assignmentCount);
	for (int i = 0; i < b.assignmentCount; ++i) {
		serealiseAssignment(b.assignments[i]);
	}
	serealiseJump(b.jump);
}

void serealiseProgram(Program p) {	
	printf("PROGRAM\n");
	printf("VARS\n");
	printf("%d\n", p.varCount);
	for (int i = 0; i < p.varCount; ++i) {
		printf("%s\n", p.varNames[i]);
	}
	printf("INPUT\n");
	printf("%d\n", p.inputCount);
	for (int i = 0; i < p.inputCount; ++i) {
		printf("%s\n", p.input[i]);
	}
	printf("BLOCKS\n");
	printf("%d\n", p.blockCount);
	for (int i = 0; i < p.blockCount; ++i) {
		serealiseBlock(p.basicBlocks[i]);
	}
}

void serealise(Program p) {
	serealiseProgram(p);
}