#include <stdio.h>

int member(char c, char* s) {
	while (s[0] != 0) {
		if (c == s[0]) {
			return 1;
		}
		s++;
	}
	return 0;
}

int call_kmp(char* p, char* d, char* free1, char* free2) {
	return kmp2(p, d, p, free1, free1, free2, free1, free2);
}

int kmp2(char* p, char* d, char* pp, char* f, char* ff, char* neg, char* f0, char* neg0) {
	//fprintf(stderr, "%s %s %s %s %s %s\n", p, d, pp, f, ff, neg);
	if (p[0] == 0) {
		//fprintf(stderr, "1\n");
		return 1;
	}
	else if (f[0] == 0) {
		//fprintf(stderr, "2\n");
		if (neg[0] != 0 && member(p[0], neg)) {
			//fprintf(stderr, "3\n");
			if (ff[0] == 0) {
				//fprintf(stderr, "4\n");
				return kmp2(pp, d + 1, pp, f0, f0, neg0, f0, neg0);
			}
			else {
				//fprintf(stderr, "5\n");
				return kmp2(pp, d, pp, ff + 1, ff + 1, neg, f0, neg0);
			}
		}
		else if (neg[0] == 0 && d[0] == 0) {
			//fprintf(stderr, "6\n");
			return 0;
		}
		else if (p[0] == d[0]) {
			//fprintf(stderr, "7\n");
			f0[0] = p[0];
			f0 += 1;
			f0[0] = 0;
			return kmp2(p + 1, d + 1, pp, f0, ff, neg0, f0, neg0);
		}
		else if (ff[0] == 0) {
			//fprintf(stderr, "8\n");
			return kmp2(pp, d + 1, pp, f0, f0, neg0, f0, neg0);
		}
		else {
			//fprintf(stderr, "9\n");
			neg[-1] = p[0];
			return kmp2(pp, d, pp, ff + 1, ff + 1, neg - 1, f0, neg0);
		}
	}
	else if (p[0] == f[0]) {
		//fprintf(stderr, "10\n");
		return kmp2(p + 1, d, pp, f + 1, ff, neg, f0, neg0);
	}
	else {
		//fprintf(stderr, "11\n");
		return kmp2(pp, d, pp, ff + 1, ff + 1, neg, f0, neg0);
	}
}