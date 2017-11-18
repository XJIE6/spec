int my_pow(int a, int b) {
	if (b == 0) {
		return 1;
	}
	return my_pow(a, b - 1) * a;
}
