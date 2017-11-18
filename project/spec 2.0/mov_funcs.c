#ifdef BIT8
#define mov_89 mov_89_8
#define mov_8b mov_8b_8
#define mov_b8 mov_b8_8
#define mov_c7 mov_c7_8
#define parce_reg_mem parce_reg_mem_8
#define assign assign_8
#define eval eval_8
#define mov mov_8
#define int_cur int_8
#define int_c7 int_8
#define print print_8
#define prefix prefix_8
#endif
#ifdef BIT32
#define mov_89 mov_89_32
#define mov_8b mov_8b_32
#define mov_b8 mov_b8_32
#define mov_c7 mov_c7_32
#define parce_reg_mem parce_reg_mem_32
#define assign assign_32
#define eval eval_32
#define mov mov_32
#define int_cur int_32
#define int_c7 int_32
#define print print_32
#define prefix prefix_32
#endif
#ifdef BIT64
#define mov_89 mov_89_64
#define mov_8b mov_8b_64
#define mov_b8 mov_b8_64
#define mov_c7 mov_c7_64
#define parce_reg_mem parce_reg_mem_64
#define assign assign_64
#define eval eval_64
#define mov mov_64
#define int_cur int_64
#define int_c7 int_32S
#define print print_64
#define prefix prefix_64
#endif

void mov_89(unsigned char cmd) {
	parce_reg_mem();
	print_params();
	eval(&p1);
	print_value();
	if (is_dynamic) {
		prefix(&p1);
		fprintf(stderr, "mov89 ");
		print(&p1, 1);
		fprintf(stderr, " ");
		print(&p2, 0);
		fprintf(stderr, "\n");
	}
	assign(&p2);
}

void mov_8b(unsigned char cmd) {
	parce_reg_mem();
	print_params();
	eval(&p2);
	print_value();
	if (is_dynamic) {
		prefix(&p2);
		fprintf(stderr, "mov8b ");
		print(&p2, 1);
		fprintf(stderr, " ");
		print(&p1, 0);
		fprintf(stderr, "\n");
	}
	assign(&p1);
}

void mov_b8(unsigned char cmd) {
	v.base = int_cur();
	v.mem = -1;
	p1.reg1 = cmd - 0xb8 + (REXR() << 3);
	p1.reg2 = -1;
	p1.base = 0;
	p1.scale = -1;
	assign(&p1);
}

void mov_c7(unsigned char cmd) {
	parce_reg_mem();
	v.base = int_c7();
	v.mem = -1;
	is_dynamic = 0;
	assign(&p2);
}