#ifdef BIT8
#define push_50 push_50_8
#define push_68 push_68_8
#define push_ff push_ff_8
#define parce_reg_mem parce_reg_mem_8
#define push push_8
#define eval eval_8
#endif
#ifdef BIT32
#define push_50 push_50_32
#define push_68 push_68_32
#define push_ff push_ff_32
#define parce_reg_mem parce_reg_mem_32
#define push push_32
#define eval eval_32
#endif
#ifdef BIT64
#define push_50 push_50_64
#define push_68 push_68_64
#define push_ff push_ff_64
#define parce_reg_mem parce_reg_mem_64
#define push push_64
#define eval eval_64
#endif

void push_50(unsigned char cmd) {
	p1.reg1 = cmd - 0x50 + (REXR() << 3);
	p1.reg2 = -1;
	p1.base = 0;
	p1.scale = -1;
	eval(&p1);
	if (is_dynamic) {
		fprintf(stderr, "push%d\n", cmd);
	}
	push_64();
}

void push_ff(unsigned char cmd) {
	parce_reg_mem();
	eval(&p2);
	if (is_dynamic) {
		fprintf(stderr, "push_ff\n");
	}
	push_64();
}

void push_68(unsigned char cmd) {
	v.base = int_cur();
	v.mem = -1;
	push();
}