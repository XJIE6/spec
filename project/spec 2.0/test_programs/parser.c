#include <stdio.h>

char* s;

int read(const char* a, int mode = 0) {
	while (isspece(*s)) {
		s++;
	}
	char* old = s;
	while (*a != 0) {
		if (*a != *b) {
			s = old;
			return 0;
		}
		a++;
		s++;
	}
	if (mode) {
		s = old;
	}
	return 1;
}

Stmt * read_stmt() {
	split
}

State* parse(char * program) {
	while (read("fun", 1)) {
		read_function();
	}
	read_stmt();
}