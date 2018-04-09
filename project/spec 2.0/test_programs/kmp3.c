#include <stdio.h>

int member3(char c, char* s, char* f) {
    while (s != f) {
        if (c == s[0]) {
            return 1;
        }
        s++;
    }
    return 0;
}

int call_kmp3(char* p, char* d, char* free1, char* free2) {
    char* pp = p;
    char* f = free1;
    char* ff = free1;
    char* neg = free2;
    char* f0 = free1;
    char* neg0 = free2;
    while (1) {
        if (p[0] == 0) {
            return 1;
        }
        else if (f == f0) {
            if (member3(p[0], neg, neg0)) {
                if (ff == f0) {
                    p = pp;
                    d++;
                    ff = f0;
                    neg = neg0;
                    continue;
                }
                else {
                    p = pp;
                    ff++;
                    f = ff;
                    continue;
                }
            }
            else if (neg == neg0 && d[0] == 0) {
                return 0;
            }
            else if (p[0] == d[0]) {
                char* ptr = ff;
                while (ptr != f0) {
                    ptr[-1] = ptr[0];
                    ptr++;
                }
                ptr[-1] = p[0];
                ff--;
                p++;
                d++;
                neg = neg0;
                continue;
            }
            else if (ff == f0) {
                p = pp;
                d++;
                f = f0;
                ff = f0;
                neg = neg0;
                continue;
            }
            else {
                neg--;
                neg[0] = p[0];
                p = pp;
                ff++;
                f = ff;
                continue;
            }
        }
        else if (p[0] == f[0]) {
            p++;
            f++;
            continue;
        }
        else {
            p = pp;
            ff++;
            f = ff;
            continue;
        }
    }
}
