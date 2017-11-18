int test(int len, int* a) {
	for (int i = 0; i < len; ++i) {
		for (int j = i + 1; j < len; ++j) {
			if (a[i] > a[j]) {
				int k = a[i];
				a[i] = a[j];
				a[j] = k;
			}
		}
	}
}