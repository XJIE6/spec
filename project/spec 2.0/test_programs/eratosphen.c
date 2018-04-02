char* eratosphen(int n) {
    char* a = my_malloc(n);
    for (int i = 0; i < n; ++i) {
        a[i] = 1;
    }
    a[0] = 0;
    a[1] = 0;
    for (int i = 2; i < n; ++i) {
        if (a[i]) {
            for (int j = i * i; j < n; j += i) {
                a[j] = 0;
            }
        }
    }
    return a;
}