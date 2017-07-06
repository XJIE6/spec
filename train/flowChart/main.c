#include "flowChart_parser.h"
#include "serealiser.h"
#include "flowChart_interpreter.h"
#include "flowChart_spec.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	char* input = argv[1];
	//printf("%s\n", input);
	//fflush(stdout);
	Program p = parce(input);
	// interpret(p);
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
		printf("%s\n", p.input[i]);
		getline(&b, &MAX_BUFF_SIZE, stdin);
		Const in = parceConst(&buff);
		if (in.type != CONST || *((int*) in.expr) != -1){
			state[find(p.varNames, p.varCount, p.input[i])] = in;
		}
	}

	int k;
	scanf("%d", &k);
	char* c[k];
	for (int i = 0; i < k; ++i) {
		c[i] = (char*)malloc(sizeof(char) * 100);
		scanf("%s", c[i]);
	}
	Program p2 = spec(p, c, k, p.varNames, p.varCount, state);
	serealise(p2);
	return 0;
}