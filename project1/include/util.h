#ifndef _UTIL_H
#define _UTIL_H

#include <stddef.h>

#define swap(x,y) s_swap(&(x), &(y), sizeof(x), sizeof(y))

inline void s_swap(void *a, void *b, size_t a_size, size_t b_size) {
    if (a_size == b_size) {
        char *pa = a, *pb = b, tmp;
        for (size_t i = 0; i < a_size; ++i) {
            tmp = pa[i];
            pa[i] = pb[i];
            pb[i] = tmp;
        }
    }
}

inline int max(int a, int b) {
    return a < b ? b : a;
}

#endif
