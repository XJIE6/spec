#include <stdio.h>
#include <stdlib.h>
#include "test.h"
#include "spec.h"
#include "state.h"

#include "run_config.c"

int main() {
	// int n = 30000;
	// int res[n];
	// srand(12345);
	// for (int i = 0; i < n; ++i) {
	// 	res[i] = rand() % 100000;
	// }

	// for (int i = 0; i < n; ++i) {
	// 	fprintf(stderr, "%d ", res[i]);
	// }

	// sort(n, res);

	// for (int i = 0; i < n; ++i) {
	// 	fprintf(stderr, "%d ", res[i]);
	// }

	//run_pow(3, 5);
	//run_sort(300);
	//run_dict();
	//run_prime(1000);
	//run_kmp();
	run_kmp2();
	// const char* s = "abaabab";
	// const char* S = "aababaabaaaa";
	// int n = 100;
	// int m = n / 2;
	// char a[n], b[n];
	// a[m] = 0;
	// b[m] = 0;
	// fprintf(stderr, "%d\n", kmp2(s, S, s, a + m, a + m, b + m, a + m, b + m));
}