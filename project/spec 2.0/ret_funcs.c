#ifdef BIT8
#define ret_c3 ret_c3_8
#define parce_reg_mem parce_reg_mem_8
#define assign assign_8
#define pop pop_8
#endif
#ifdef BIT32
#define ret_c3 ret_c3_32
#define parce_reg_mem parce_reg_mem_32
#define assign assign_32
#define pop pop_32
#endif
#ifdef BIT64
#define ret_c3 ret_c3_64
#define parce_reg_mem parce_reg_mem_64
#define assign assign_64
#define pop pop_64
#endif

void ret_c3(unsigned char cmd) {
	pop();
	if (is_end) {
		p1.reg1 = 0;//rax
		p1.reg2 = -1;
		p1.scale = -1;
		p1.base = 0;
		prefix(&p1);
		fprintf(stderr, "END\n");
		return;
	}
	p1.reg1 = 16; //rip
	p1.reg2 = -1;
	p1.scale = -1;
	p1.base = 0;
	if (is_dynamic) {
		fprintf(stderr, "WTF RET%d\n", cmd);
	}
	assign(&p1);
}
