#include "flowChart_parser.h"
#include "serealiser.h"
#include <stdio.h>

int main(int argc, char** argv) {
	char* input = argv[1];
	printf("%s\n", input);
	fflush(stdout);
	Program p = parce(input);
	serealise(p);
	return 0;
}