#ifdef BIT8
#define pop_58 pop_58_8
#define pop_8f pop_8f_8
#define leave_c9 leave_c9_8
#define parce_reg_mem parce_reg_mem_8
#define assign assign_8
#define eval eval_8
#define pop pop_8
#endif
#ifdef BIT32
#define pop_58 pop_58_32
#define pop_8f pop_8f_32
#define leave_c9 leave_c9_32
#define parce_reg_mem parce_reg_mem_32
#define assign assign_32
#define eval eval_32
#define pop pop_32
#endif
#ifdef BIT64
#define pop_58 pop_58_64
#define pop_8f pop_8f_64
#define leave_c9 leave_c9_64
#define parce_reg_mem parce_reg_mem_64
#define assign assign_64
#define eval eval_64
#define pop pop_64
#endif

void pop_58(unsigned char cmd) {
	pop();
	p1.reg1 = cmd - 0x58 + (REXR() << 3);
	p1.reg2 = -1;
	p1.base = 0;
	p1.scale = -1;
	if (is_dynamic) {
		fprintf(stderr, "pop%d\n", cmd);
	}
	assign(&p1);
}

void pop_8f(unsigned char cmd) {
	pop();
	parce_reg_mem();
	if (is_dynamic) {
		fprintf(stderr, "pop%d\n", cmd);
	}
	assign(&p2);
}

void leave_c9(unsigned char cmd) {
	p1.reg1 = 5; //RBP
	p1.reg2 = -1;
	p1.base = 0;
	p1.scale = -1;
	eval(&p1);
	p2.reg1 = 4; //RSP
	p2.reg2 = -1;
	p2.base = 0;
	p2.scale = -1;
	if (is_dynamic) {
		fprintf(stderr, "leave_c9\n", cmd);
	}
	assign(&p2);
	pop_64();
	if (is_dynamic) {
		fprintf(stderr, "leave_c9\n", cmd);
	}
	p1.reg1 = 5; //RBP
	p1.reg2 = -1;
	p1.base = 0;
	p1.scale = -1;
	assign(&p1);
}