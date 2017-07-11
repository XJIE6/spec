#include "bToS.h"
#include "flowChart_parser.h"
#include "serealiser.h"
#include <stdlib.h>
#include <stdio.h>

List toList(Const * c) {
	return *((List*)(*c).expr);
}
int toInt(Const * c) {
	return *((int*)(*c).expr);
}
void toVar(Const * v) {
	List l = toList(v);
	while ((int)l) {
		printf("%c", toInt((*l).val));
		l = (*l).next;
	}
}

void toExpr(Const * e) {
	List exp = toList(e);
	switch(toInt((*exp).val)) {
		case 0:
			serealiseConst(*((*((*exp).next)).val));
			break;
		case 1:
			toVar((*((*exp).next)).val);
			break;
		case 2:
			switch(toInt((*((*exp).next)).val)) {
				case 0:
					printf(" ^hd ");
					break;
				case 1:
					printf(" ^tl ");
					break;
				case 2:
					printf(" ^print ");
					break;
			}
			exp = (*((*exp).next)).next;
			toExpr((*exp).val);
			break;
		case 3:
			switch(toInt((*((*exp).next)).val)) {
				case 0:
					printf(" _== ");
					break;
				case 1:
					printf(" _* ");
					break;
				case 2:
					printf(" _< ");
					break;
				case 3:
					printf(" _- ");
					break;
				case 4:
					printf(" _+ ");
					break;
				case 5:
					printf(" _cons ");
					break;
			}
			exp = (*((*exp).next)).next;
			toExpr((*exp).val);
			printf(" ");
			toExpr((*((*exp).next)).val);
			break;
	}
}

void toJump(Const * j) {
	List jmp = toList(j);
	switch(toInt((*jmp).val)) {
		case 0:
			printf("goto ");
			serealiseConst(*((*((*jmp).next)).val));
			printf(".\n");
			break;
		case 1:
			printf("if ");
			toExpr((*((*jmp).next)).val);
			jmp = (*((*jmp).next)).next;
			printf(" then ");
			serealiseConst(*((*jmp).val));
			printf(" else ");
			serealiseConst(*((*((*jmp).next)).val));
			printf(".\n");
			break;
		case 2:
			printf("return ");
			toExpr((*((*jmp).next)).val);
			printf(".\n");
			break;
	}
}

void toAss(Const * a) {
	List ass = toList(a);
	toVar((*ass).val);
	printf(" := ");
	toExpr((*((*ass).next)).val);
	printf(";\n");
}
int blk = 0;
void toBlock(Const * b) {
	fprintf(stderr, "BLOCK %d", blk);
	++blk;
	List block = toList(b);
	serealiseConst(*((*block).val));
	printf(":\n");
	block = (*block).next;
	List ass = toList((*block).val);
	while ((int)ass) {
		toAss((*ass).val);
		ass = (*ass).next;
	}
	block = (*block).next;
	toJump((*block).val);
	printf("\n");
}


void bToS(char* input) {
	Const* p = parceConst(&input);
	printf("read ");
	List l = toList(p);
	Const* inp = (*l).val;
	Const* blocks = (*((*l).next)).val;
	List inputList = toList(inp);
	toVar((*inputList).val);
	inputList = (*inputList).next;
	while ((int)inputList) {
		printf(", ");
		toVar((*inputList).val);
		inputList = (*inputList).next;
	}
	printf(";\n");
	fprintf(stderr, "VARS");
	List blockList = toList(blocks);
	while ((int)blockList) {
		toBlock((*blockList).val);
		blockList = (*blockList).next;
	}
}