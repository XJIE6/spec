#pragma once

#include <stdlib.h>
#include <stdio.h>

void* my_malloc(long long n) {
    return malloc(n);
}