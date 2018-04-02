int kmp(int len1, char* s1, int len2, char* s2) {
    for (int i = 0; i <= len2 - len1; ++i) {
        int f = 1;
        for (int j = 0; j < len1; ++j) {
            if (s1[j] != s2[i + j]) {
                f = 0;
            }
        }
        if (f) {
            return i;
        }
    }
    return -1;
}
