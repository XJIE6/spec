#include "flowChart.h"
#include "flowChart_interpreter.h"
#include "flowChart_spec.h"
#include "serealiser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int MAX_PP_COUNT = 100;
int MAX_BB_COUNT = 100;
int MAX_ASS_COUNT = 100;
int magic_number = 71;

int number(Const c) {
	if (c.type == NUMBER) {
		return *((int*)c.expr);
	}
	List l = *((List*)c.expr);
	int res = 0;
	for (int i = 0; i < l.listLen; ++i) {
		res *= magic_number;
		res += number(l.list[i]);
	}
	if (res < 0) {
		return -res;
	}
	return res;
}

Label getLabel(Label label, Const* state, int len) {
	int res = label;
	for (int i = 0; i < len; ++i) {
		res *= magic_number;
		res += number(state[i]);
	}
	if (res < 0) {
		return -res;
	}
	return res;
}

typedef struct _pp {
	Label label;
	Const* state;
} pp;

void add(pp* pending, int* pendingLen, Label label, Const* state, int stateLen) {
	int f = 1;
	for (int i = 0; i < *pendingLen; ++i) {
		if (pending[i].label == label && !memcmp(pending[i].state, state, sizeof(int) * stateLen)) {
			f = 0;
			break;
		}
	}
	if (f) {
		pending[*pendingLen].label = label;
		pending[*pendingLen].state = malloc (sizeof(Const) * stateLen);
		memcpy(pending[*pendingLen].state, state, sizeof(Const) * stateLen);
		++(*pendingLen);
	}

}

Program spec(Program p, char** stat, int statLen, char** names, int len, Const* state){
	pp* pending = malloc(sizeof(pp) * MAX_PP_COUNT);
	BasicBlock* result = malloc(sizeof(BasicBlock) * MAX_BB_COUNT);
	int lastBB = 0;
	int pendingLen = 0;
	Label cur = p.basicBlocks[0].label;
	add(pending, &pendingLen, cur, state, len);
	int scaned = 0;
	while (scaned < pendingLen) {
		cur = pending[scaned].label;
		memcpy(state, pending[scaned].state, sizeof(Const) * len);
		BasicBlock bb = findBlock(p.basicBlocks, p.blockCount, cur);
		BasicBlock* curbb = &(result[lastBB]);
		(*curbb).label = getLabel(cur, state, len);
		(*curbb).assignments = malloc(sizeof(Assignment) * MAX_ASS_COUNT);
		(*curbb).assignmentCount = 0;

		compute:
		for (int i = 0; i < bb.assignmentCount; ++i) {
			if (find(stat, statLen, bb.assignments[i].var) != -1) {
				state[find(names, len, bb.assignments[i].var)] = *((Const*)interpretExpr(bb.assignments[i].expr, names, len, &state).expr);
			}
			else {
				Assignment* curass = &((*curbb).assignments[(*curbb).assignmentCount]);
				(*curass).var = bb.assignments[i].var;
				(*curass).expr = interpretExpr(bb.assignments[i].expr, names, len, &state);
				++(*curbb).assignmentCount;
			}
		}
		If curIf;
		Expr eval;
		switch (bb.jump.type) {
			case (GOTO):
				bb = findBlock(p.basicBlocks, p.blockCount, *((int*)bb.jump.jump));
				goto compute;
			case (RETURN):
				(*curbb).jump.type = RETURN;
				Return* res = malloc(sizeof(Return));
				*res = interpretExpr(*((Return*)bb.jump.jump), names, len, &state);
				(*curbb).jump.jump = res;
				break;
			case (IF):
				curIf = *((If*)bb.jump.jump);
				eval = interpretExpr(curIf.expr, names, len, &state);
				if (eval.type == CONST) {
					if(*((int*)(*((Const*)eval.expr)).expr)){
						bb = findBlock(p.basicBlocks, p.blockCount, curIf.ifTrue);
						goto compute;
					}
					else {
						bb = findBlock(p.basicBlocks, p.blockCount, curIf.ifFalse);
						goto compute;
					}
				}
				else {
					add(pending, &pendingLen, curIf.ifTrue, state, len);
					add(pending, &pendingLen, curIf.ifFalse, state, len);
					(*curbb).jump.type = IF;
					If* res = malloc(sizeof(If));
					(*res).expr = eval;
					(*res).ifTrue = getLabel(curIf.ifTrue, state, len);
					(*res).ifFalse = getLabel(curIf.ifFalse, state, len);
					(*curbb).jump.jump = res;
				}
				break;
		}

		++lastBB;
		++scaned;
	}
	printf("%d\n", pendingLen);
	Program ans;
	ans.varNames = p.varNames;
	ans.varCount = p.varCount;
	ans.input = p.input; //FIXME
	ans.inputCount = p.inputCount; //FIXME
	ans.basicBlocks = result;
	ans.blockCount = lastBB;
	return ans;
}