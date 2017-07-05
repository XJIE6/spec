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

Label getLabel(Label label, int* state, int len) {
	int res = label;
	for (int i = 0; i < len; ++i) {
		res *= 71;
		res += state[i];
	}
	if (res < 0) {
		return -res;
	}
	return res;
}

typedef struct _pp {
	Label label;
	int* state;
} pp;

void add(pp* pending, int* pendingLen, Label label, int* state, int stateLen) {
	printf("pl %d\n", *pendingLen);
	printf("%d\n", label);
	for (int i = 0; i < stateLen; ++i) {
		printf("%d ", state[i]);
	}
	printf("\n");
	int f = 1;
	for (int i = 0; i < *pendingLen; ++i) {
		if (pending[i].label == label && !memcmp(pending[i].state, state, sizeof(int) * stateLen)) {
			f = 0;
			break;
		}
	}
	if (f) {
		pending[*pendingLen].label = label;
		pending[*pendingLen].state = malloc (sizeof(int) * stateLen);
		memcpy(pending[*pendingLen].state, state, sizeof(int) * stateLen);
		++(*pendingLen);
	}

}

Program spec(Program p, char** stat, int statLen, char** names, int len, int* state){
	pp* pending = malloc(sizeof(pp) * MAX_PP_COUNT);
	BasicBlock* result = malloc(sizeof(BasicBlock) * MAX_BB_COUNT);
	int lastBB = 0;
	int pendingLen = 0;
	Label cur = p.basicBlocks[0].label;
	add(pending, &pendingLen, cur, state, len);
	int scaned = 0;
	//printf("START\n");
	while (scaned < pendingLen) {
		//printf("WHILE\n");
		cur = pending[scaned].label;
		memcpy(state, pending[scaned].state, sizeof(int) * len);
		BasicBlock bb = findBlock(p.basicBlocks, p.blockCount, cur);
		BasicBlock* curbb = &(result[lastBB]);
		(*curbb).label = getLabel(cur, state, len);
		(*curbb).assignments = malloc(sizeof(Assignment) * MAX_ASS_COUNT);
		(*curbb).assignmentCount = 0;

		//printf("PRECOMPUTE\n");
		compute:
		//printf("PREFOR\n");
		for (int i = 0; i < bb.assignmentCount; ++i) {
			//printf("INFOR\n");
			if (find(stat, statLen, bb.assignments[i].var) != -1) {
				state[find(names, len, bb.assignments[i].var)] = *((int*)interpretExpr(bb.assignments[i].expr, names, len, &state).expr);
			}
			else {
				Assignment* curass = &((*curbb).assignments[(*curbb).assignmentCount]);
				(*curass).var = bb.assignments[i].var;
				(*curass).expr = interpretExpr(bb.assignments[i].expr, names, len, &state);
				++(*curbb).assignmentCount;
			}
		}
		//printf("AFTERFOR\n");
		If curIf;
		Expr eval;
		//serealiseJump(bb.jump);
		switch (bb.jump.type) {
			case (GOTO):
				//printf("$$$$$$$$$$$$$$$$$$\n");
				for (int i = 0; i < statLen; ++i) {
					//printf("%s %d\n", p.varNames[i], state[i]);
				}
				//printf("$$$$$$$$$$$$$$$$$$\n");
				//serealiseBlock(bb);
				bb = findBlock(p.basicBlocks, p.blockCount, *((int*)bb.jump.jump));
				//printf("$$$$$$$$$$$$$$$$$$\n");
				//serealiseBlock(bb);
				//printf("$$$$$$$$$$$$$$$$$$\n");
				//printf("GOTO\n");
				goto compute;
			case (RETURN):

				//printf("RETURN\n");
				(*curbb).jump.type = RETURN;
				Return* res = malloc(sizeof(Return));
				*res = interpretExpr(*((Return*)bb.jump.jump), names, len, &state);
				//printf("$$$$$$$$$$$$$$$$$$\n");
				//serealiseExpr(*res);
				(*curbb).jump.jump = res;
				//printf("$$$$$$$$$$$$$$$$$$\n");
				//fflush(stdout);
				//serealiseBlock((*curbb));
				//printf("$$$$$$$$$$$$$$$$$$\n");
				//fflush(stdout);
				//serealiseBlock(result[lastBB]);
				//printf("$$$$$$$$$$$$$$$$$$\n");
				//fflush(stdout);
				break;
			case (IF):
				curIf = *((If*)bb.jump.jump);
				eval = interpretExpr(curIf.expr, names, len, &state);
				if (eval.type == CONST) {
					if(*((int*) eval.expr)){
						//printf("$$$$$$$$$$$$$$$$$$\n");
						for (int i = 0; i < statLen; ++i) {
							//printf("%s %d\n", p.varNames[i], state[i]);
						}
						//printf("$$$$$$$$$$$$$$$$$$\n");
						//serealiseBlock(bb);
						bb = findBlock(p.basicBlocks, p.blockCount, curIf.ifTrue);
						//printf("$$$$$$$$$$$$$$$$$$\n");
						//serealiseBlock(bb);
						//printf("$$$$$$$$$$$$$$$$$$\n");
						//printf("EVALTRUE\n");
						goto compute;
					}
					else {
						bb = findBlock(p.basicBlocks, p.blockCount, curIf.ifFalse);
						
						//printf("EVALFALSE\n");
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

		//printf("END\n");
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