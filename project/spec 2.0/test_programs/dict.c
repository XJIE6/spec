int dict(int len, int* a, int* b, int c) {
    for (int i = 0; i < len; ++i) {
        if (a[i] == c) {
            if (b[i] == 0) {
                return 1;
            }
            else {
                return 2;
            }
        }
    }
    return 3;
}