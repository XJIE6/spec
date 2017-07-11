#include "flowChart_parser.h"
#include "bracketSer.h"
#include "serealiser.h"
#include "flowChart_interpreter2.h"
#include "flowChart_spec.h"
#include "bToS.h"
#include <stdio.h>
#include <stdlib.h>
int bufsize = 10000000;
int main(int argc, char** argv) {
	char* input = argv[1];
	FILE *in = fopen(input, "r");
	fseek(in, 0, SEEK_END);
	long fsize = ftell(in);
	fseek(in, 0, SEEK_SET);  //same as rewind(f);

	char *string = malloc(fsize + 1);
	fread(string, fsize, 1, in);
	fclose(in);

	string[fsize] = 0;

	Program p = parce(string);
	serealiseB(p);
	//interpretTWO(p);



	// char* buff = malloc(sizeof(char) * bufsize);
	// char *b = buff;
	// getline(&b, &bufsize, in);
	// bToS(buff);
	return 0;
}