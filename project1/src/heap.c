#include "heap.h"
#include "util.h"
#include <stdlib.h>

ValueType add(ValueType a, ValueType b) {
    ValueType res;
    res.p_cost = a.p_cost + b.p_cost;
    res.p_time = a.p_time + b.p_time;
    return res;
}

int cmp_time_cost(ValueType *a, ValueType *b) {
    return a->p_time < b->p_time || (a->p_time == b->p_time && a->p_cost < b->p_cost);
}

int cmp_cost_time(ValueType *a, ValueType *b) {
    return a->p_cost < b->p_cost || (a->p_cost == b->p_cost && a->p_time < b->p_time);
}

int init_heap(Heap *h, int heap_size, CompareFunc heap_cf) {
    if (heap_size <= 0)
        return 0;
    h->heap = (DataType *)malloc((heap_size + 1) * sizeof(DataType));
    if (h->heap == NULL)
        return 0;
    h->h_size = 0;
    h->cf = heap_cf;
    return heap_size;
}

int heap_cmpLT(Heap *h, DataType a, DataType b) {
    return h->cf(&a.value, &b.value);
}

int insert(Heap *h, DataType k) {
    h->heap[++h->h_size] = k;
    int p = h->h_size;
    while (p > 1) {
        if (heap_cmpLT(h, h->heap[p], h->heap[p >> 1])) {
            swap(h->heap[p >> 1], h->heap[p]);
            p >>= 1;
        }
        else
            break;
    }
    return h->h_size;
}

int getsize(Heap *h) {
    return h->h_size;
}

DataType top(Heap *h) {
    return h->heap[1];
}

int pop(Heap *h) {
    swap(h->heap[1], h->heap[h->h_size]);
    h->h_size--;
    int p = 1;
    while (p <= h->h_size) {
        if ((p << 1 | 1) <= h->h_size) {
            if (heap_cmpLT(h, h->heap[p << 1], h->heap[p << 1 | 1])) {
                if (heap_cmpLT(h, h->heap[p << 1], h->heap[p])) {
                    swap(h->heap[p << 1], h->heap[p]);
                    p <<= 1;
                }
                else
                    break;
            }
            else {
                if (heap_cmpLT(h, h->heap[p << 1 | 1], h->heap[p])) {
                    swap(h->heap[p << 1 | 1], h->heap[p]);
                    p = p << 1 | 1;
                }
                else
                    break;
            }
        }
        else {
            if ((p << 1) <= h->h_size) {
                if (heap_cmpLT(h, h->heap[p << 1], h->heap[p])) {
                    swap(h->heap[p << 1], h->heap[p]);
                    p <<= 1;
                }
                else
                    break;
            }
            else
                break;
        }
    }
    return h->h_size;
}

int free_heap(Heap *h) {
    free(h->heap);
    return 1;
}
