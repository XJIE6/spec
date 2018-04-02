#include <stdio.h>

int member3(char c, char* s) {
    while (s[0] != 0) {
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
        else if (f[0] == 0) {
            if (neg[0] != 0 && member3(p[0], neg)) {
                if (ff[0] == 0) {
                    p = pp;
                    d = d + 1;
                    f = f0;
                    ff = f0;
                    neg = neg0;
                    continue;
                }
                else {
                    p = pp;
                    f = ff + 1;
                    ff = ff + 1;
                    continue;
                }
            }
            else if (neg[0] == 0 && d[0] == 0) {
                return 0;
            }
            else if (p[0] == d[0]) {
                ff--;
                char* ptr = ff;
                while (ptr[1] != 0) {
                    ptr[0] = ptr[1];
                    ptr++;
                }
                ptr[0] = p[0];
                p = p + 1;
                d = d + 1;
                f = f0;
                neg = neg0;
                continue;
            }
            else if (ff[0] == 0) {
                p = pp;
                d = d + 1;
                f = f0;
                ff = f0;
                neg = neg0;
                continue;
            }
            else {
                neg--;
                neg[0] = p[0];
                p = pp;
                f = ff + 1;
                ff = ff + 1;
                continue;
            }
        }
        else if (p[0] == f[0]) {
            p = p + 1;
            f = f + 1;
            continue;
        }
        else {
            p = pp;
            f = ff + 1;
            ff = ff + 1;
            continue;
        }
    }
}
