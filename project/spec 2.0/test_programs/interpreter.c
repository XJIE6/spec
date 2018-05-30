const char * program;

const char * get_program() {
	return program;
}

char is_letter(char c) {
	return (c >= 'a' && c <= 'z');
}

char is_space(char c) {
	return (c == ' ' || c == '\t' || c == '\n');
}

int readVar() {
	int res = *program - 'a';
	program++;
	while (is_space(*program)) {
		program++;
	}
	return res;
}

void read(const char * c) {
	while (*c == *program) {
		c++;
		program++;
		while (is_space(*program)) {
			program++;
		}
		while (is_space(*c)) {
			c++;
		}
	}
}

char check(const char * c) {
	const char * p = program;
	while (*c == *program) {
		c++;
		program++;
		while (is_space(*program)) {
			program++;
		}
		while (is_space(*c)) {
			c++;
		}
	}
	if (*c == 0) {
		return 1;
	}
	program = p;
	return 0;
}

char _check(const char * c) {
	const char * p = program;
	while (*c == *program) {
		c++;
		program++;
		while (is_space(*program)) {
			program++;
		}
		while (is_space(*c)) {
			c++;
		}
	}
	if (*c == 0) {
		program = p;
		return 1;
	}
	program = p;
	return 0;
}


char is_digit(char c) {
	return (c >= '0' && c <= '9');
}

int digit_or_var_val(int * state) {
	int val = 0;
	if (is_digit(*program)) {
		while(is_digit(*program)) {
			int tmp = 10;
			val = val * tmp + (*program) - '0';
			program++;
		}
		read(";");
	}
	else {
		int var = readVar();
		val = state[var];
		read(";");
	}
	while (is_space(*program)) {
		program++;
	}
	return val;
}

int cond(int * state) {
	read("(");
	int val_l = digit_or_var_val(state);
	int opcode;
	if (check("<")) {
		opcode = 0;
	}
	else if (check(">")) {
		opcode = 1;
	}
	else if (check("==")) {
		opcode = 2;
	}
	else if (check(">=")) {
		opcode = 3;
	}
	else if (check("<=")) {
		opcode = 4;
	}
	else {
		read("!=");
		opcode = 5;
	}
	int val_r = digit_or_var_val(state);
	int res;
	switch(opcode) {
		case 0:
			res = val_l < val_r;
			break;
		case 1:
			res = val_l > val_r;
			break;
		case 2:
			res = val_l == val_r;
			break;
		case 3:
			res = val_l >= val_r;
			break;
		case 4:
			res = val_l <= val_r;
			break;
		case 5:
			res = val_l != val_r;
			break;
	}
	read("){");
	while (is_space(*program)) {
		program++;
	}
	return res;
}

void go_back(){
	program--;
	int sum = 1;
	while (sum != 0) {
		if (*program == '}') {
			sum++;
		}
		else if (*program == '{') {
			sum--;
		}
		program--;
	}
	while (*program != '(') {
		program--;
	}
	while (*program != 'i' && *program != 'w') {
		program--;
	}
	if (program[1] == 'l') {
		while(*program != 'w') {
			program--;
		}
	}
}

void skip() {
	int sum = 1;
	while (sum != 0) {
		if (*program == '}') {
			sum--;
		}
		else if (*program == '{') {
			sum++;
		}
		program++;
	}
	while (is_space(*program)) {
		program++;
	}
}

int interpreter(const char * _program, int * input) {
	int * state = my_malloc(26 * sizeof(int));
	program = _program;
	read("int main(");	
	while(*program != ')') {
		read("int");
		int var = readVar();
		state[var] = *input;
		input++;
		read(",");
	}
	read("){");
	read("int");
	while(*program != ';') {
		int var = readVar();
		read(",");
	}
	read(";");
	while(1) {
		if (check("while")) {
			int res = cond(state);
			if (!res) {
				skip();
			}
		}
		else if (check("if")) {
			int res = cond(state);
			if (!res) {
				skip();
			}
		}
		else if (check("return")) {
			return digit_or_var_val(state);
		}
		else if(_check("}")) {
			const char * p = program + 1;
			go_back();
			if (_check("if")) {
				program = p;
			}
			while (is_space(*program)) {
				program++;
			}
		}
		else {
			int var = readVar();
			int opcode;
			if (check("=")) {
				opcode = 0;
			}
			else if (check("+=")) {
				opcode = 1;
			}
			else if (check("-=")) {
				opcode = 2;
			}
			else if (check("*=")) {
				opcode = 3;
			}
			else {
				read("/=");
				opcode = 4;
			}
			int val = digit_or_var_val(state);
			if (opcode == 0) {
				state[var] = val;
			}
			else if (opcode == 1) {
				state[var] += val;
			}
			else if (opcode == 2) {
				state[var] -= val;
			}
			else if (opcode == 3) {
				state[var] *= val;
			}
			else if (opcode == 4) {
				state[var] /= val;
			}
			read(";");
			while (is_space(*program)) {
				program++;
			}
		}
	}
}