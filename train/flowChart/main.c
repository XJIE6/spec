#include "flowChart_parser.h"
#include "serealiser.h"
#include "flowChart_interpreter.h"
#include "flowChart_spec.h"
#include <stdio.h>

int main(int argc, char** argv) {
	char* input = argv[1];
	//printf("%s\n", input);
	//fflush(stdout);
	Program p = parce(input);
	//interpret(p);
	int state[p.varCount];
	for (int i = 0; i < p.varCount; ++i) {
		state[i] = -1;
	}
	for (int i = 0; i < p.inputCount; ++i) {
		int x;
		printf("%s\n", p.input[i]);
		scanf("%d", &x);
		if (x != -1) {
			state[find(p.varNames, p.varCount, p.input[i])] = x;
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