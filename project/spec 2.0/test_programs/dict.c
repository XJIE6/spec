int dict(int len, int* a, int* b, int c) {
    for (int i = 0; i < len; ++i) {
        if (a[i] == c) {
            return b[i];
        }
    }
    return -1;
}