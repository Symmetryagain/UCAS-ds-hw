#ifndef _HEAP_H
#define _HEAP_H

typedef struct {
    int p_time;
    int p_cost;
} ValueType;

ValueType add(ValueType a, ValueType b);

typedef struct {
    ValueType value;
    int id;
} DataType;

typedef int (*CompareFunc)(ValueType *, ValueType *);

typedef struct {
    int h_size;
    DataType *heap;
    CompareFunc cf;
} Heap;

int cmp_time_cost(ValueType *, ValueType *);
int cmp_cost_time(ValueType *, ValueType *);

/**
 * @param h pointer of heap to init
 * @param heap_size the number of nodes in heap, >0
 * @param heap_cf compare function, either cmp12 or cmp21
 * @retval >0 success, capacity of heap
 * @retval 0 fail
 */
int init_heap(Heap *h, int heap_size, CompareFunc heap_cf);

/**
 * @param h pointer of heap to compare two data
 * @param a pointer of data 1
 * @param b pointer of data 2
 * @retval 1 a less than b
 * @retval 0 a not less than b
 */
int heap_cmpLT(Heap *h, DataType a, DataType b);

/**
 * @param h pointer of heap to insert value
 * @param k the value to insert into heap
 * @retval >0 success, size after insert
 * @retval 0 fail
 */
int insert(Heap *h, DataType k);

/**
 * @param h pointer of heap to get size
 * @retval size of the heap
 */
int getsize(Heap *h);

/**
 * @param h pointer of heap to get top
 * @retval top of the heap
 * @note make sure h is not empty
 */
DataType top(Heap *h);

/**
 * @param h pointer of heap to pop
 * @retval >=0 success, size after pop
 * @retval -1 fail
 */
int pop(Heap *h);

/**
 * @param h pointer of heap to free
 * @retval 1 success
 * @retval 0 fail
 */
int free_heap(Heap *h);

#endif
