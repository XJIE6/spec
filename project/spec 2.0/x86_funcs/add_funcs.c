#ifdef BIT8
#define add_01 add_01_8
#define add_03 add_03_8
#define add_05 add_05_8
#define add_81 add_81_8
#define add_83 add_83_8
#define parce_reg_mem parce_reg_mem_8
#define assign assign_8
#define eval eval_8
#define int_s int_8
#define int_cur int_8
#define print print_8
#define prefix prefix_8
#endif
#ifdef BIT32
#define add_01 add_01_32
#define add_03 add_03_32
#define add_05 add_05_32
#define add_81 add_81_32
#define add_83 add_83_32
#define parce_reg_mem parce_reg_mem_32
#define assign assign_32
#define eval eval_32
#define int_s int_32
#define int_cur int_32
#define print print_32
#define prefix prefix_32
#endif
#ifdef BIT64
#define add_01 add_01_64
#define add_03 add_03_64
#define add_05 add_05_64
#define add_81 add_81_64
#define add_83 add_83_64
#define parce_reg_mem parce_reg_mem_64
#define assign assign_64
#define eval eval_64
#define eval eval_64
#define int_s int_32S
#define int_cur int_64
#define print print_64
#define prefix prefix_64
#endif

void add_05(unsigned char cmd) {
	p1.reg1 = 0;
	p1.reg2 = -1;
	p1.base = 0;
	p1.scale = -1;
	eval(&p1);
	v.base += int_s();
	if (is_dynamic) {
		fprintf(stderr, "add05\n");
	}
	assign(&p1);
}

void add_81(unsigned char cmd) {
	parce_reg_mem();
	eval(&p2);
	v.base += int_s();
	if (is_dynamic) {
		fprintf(stderr, "add81\n");
	}
	assign(&p2);
}

void add_83(unsigned char cmd) {
	parce_reg_mem();
	eval(&p2);
	v.base += int_8S();
	if (is_dynamic) {
		fprintf(stderr, "add83\n");
	}
	assign(&p2);
}

void add_01(unsigned char cmd) {
	parce_reg_mem();
	eval(&p2);
	value cur;
	cur.base = v.base;
	cur.mem = v.mem;
	if (is_dynamic) {
		prefix(&p1);
		fprintf(stderr, "add01 ");
		print(&p2);
		fprintf(stderr, " ");
		print(&p1);
		fprintf(stderr, "\n");
	}
	eval(&p1);
	v.base += cur.base;
	if (v.mem == -1) {
		v.mem = cur.mem;
	}
	else if (cur.mem != -1) {
		fprintf(stderr, "ERROR 1236\n");
		return;
	}
	if (is_dynamic) {
		prefix(&p1);
		fprintf(stderr, "add01 ");
		print(&p2);
		fprintf(stderr, " ");
		print(&p1);
		fprintf(stderr, "\n");
	}
	assign(&p2);
}

void add_03(unsigned char cmd) {
	parce_reg_mem();
	eval(&p2);
	value cur;
	cur.base = v.base;
	cur.mem = v.mem;
	if (is_dynamic) {
		prefix(&p1);
		fprintf(stderr, "add03 ");
		print(&p2);
		fprintf(stderr, " ");
		print(&p1);
		fprintf(stderr, "\n");
	}
	eval(&p1);
	v.base += cur.base;
	if (v.mem == -1) {
		v.mem = cur.mem;
	}
	else if (cur.mem != -1) {
		fprintf(stderr, "ERROR 11\n");
		return;
	}
	if (is_dynamic) {
		prefix(&p1);
		fprintf(stderr, "add03 ");
		print(&p2);
		fprintf(stderr, " ");
		print(&p1);
		fprintf(stderr, "\n");
	}
	assign(&p1);
}