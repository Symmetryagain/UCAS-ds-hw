#ifndef MULTI_KEY_H

#define MULTI_KEY_H
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAX_RECORD 10000
#define MAX_KEY 5
#define MAX  101   /**< The size of each record's key does not exceed 100 */
#define FALSE 0
#define TRUE 1
#define RADIX 101
/**
 * @brief Use LOOP to calculate the average result multiple times
 */
#define LOOP 100  /**< Set average time, used to obtain statistical data from multiple experiments */


/**
 * @brief Static linked list node
 * Each node contains multiple keys
 * The next of each node points to the next node
 * @note The next of a node being 0 means the end of the list
 * @note The keys array stores multiple keys
 */
typedef struct{
    int keys[MAX_KEY];
    int next;
}SLCell;


/**
 * @brief Static linked list
 * Contains an array to store nodes
 * keynum indicates the number of keys per node
 * length indicates the length of the list
 * @note The index of nodes starts from 1, node 0 is the head node
 * @note The next of the head node points to the first node of the list
 * @note The next of a node being 0 means the end of the list
 * @note The keys array stores multiple keys
 */
typedef struct{
    SLCell array[MAX_RECORD + 1];
    int keynum;
    int length;
}SLList;

/**
 * @brief LSD sort for multi-key record sequences
 * @param arr_record Multi-key record sequence
 * @param r Number of records
 * @param k Number of keys per record
 * @return void
 * @note This function is used to perform LSD sort on multi-key record sequences
 * @note The function sorts according to each key of the record
 * @note The larger the key index, the lower the priority
 * @note This function uses bubble sort to implement LSD sort
 * @note Each sort starts from the last key and sorts forward one by one
 * @note This function will modify the contents of the arr_record array
 */
void lsd_sort(int ** arr_record,int r,int k);

/**
 * @brief Print multi-key record sequence
 * @param arr_record Multi-key record sequence
 * @param r Number of records
 * @param k Number of keys per record
 * @return void
 * @note This function is used to print multi-key record sequences
 * @note Each line prints all the keys of a record
 * @note The index of records starts from 0
 */
void print_arr(int**arr_record, int r, int k);

/**
 * @brief LSD counting sort based on distribute and collect for static linked list
 * @param array Static linked list
 * @param i Current key index for sorting
 * @param f Distribution array
 * @param e End array
 * @return void
 * @note The f array is used to record the head node index of each sublist
 * @note The e array is used to record the tail node index of each sublist
 * @note This function distributes the nodes in the static linked list according to the i-th key
 * @note This function collects the nodes in the static linked list according to the i-th key
 * @note Then sorts in order of increasing priority
 * @note This function will modify the next pointer of the static linked list so that the nodes are sorted according to the i-th key
 */
void Distribute(SLCell* array, int i, int* f, int* e);

/**
 * @brief Collect for static linked list
 * @param array Static linked list
 * @param i Current key index for sorting
 * @param f Distribution array
 * @param e End array    
 * @return void
 * @note The f array is used to record the head node index of each sublist
 * @note The e array is used to record the tail node index of each sublist
 * @note This function collects the nodes in the static linked list according to the i-th key
 * @note This function will modify the next pointer of the static linked list so that the nodes are sorted according to the i-th key
 * @note This function merges all sublists into a complete static linked list
 */
void Collect(SLCell* array, int i, int* f, int* e);

/**
 * @brief Radix sort for static linked list
 * @param L Static linked list
 * @return void
 * @note This function performs radix sort on the static linked list
 * @note The function sorts according to each key of the node
 * @note This function will modify the next pointer of the static linked list so that the nodes are sorted according to the keys
 * @note This function starts from the last key and sorts forward one by one
 */
void RadixSort(SLList *L);

/**
 * @brief Print static linked list
 * @param L Static linked list
 * @return void
 * @note This function is used to print the static linked list
 * @note Each line prints all the keys of a node
 * @note The index of nodes starts from 1, node 0 is the head node
 * @note The next of the head node points to the first node of the list
 */
void print_SLlist(SLList* L);

/**
 * @brief Bubble sort for multi-key record sequences
 * @param array_record Multi-key record sequence
 * @param left Start index for sorting
 * @param right End index for sorting
 * @param key_num Number of keys
 * @return void
 * @note This function is used to perform bubble sort on multi-key record sequences
 * @note The function sorts according to each key of the record
 * @note The larger the key index, the lower the priority
 * @note This function uses bubble sort to implement
 * @note Each sort compares adjacent records from left to right. If the key of the previous record is greater than that of the next, swap them
 * @note This function will modify the contents of the array_record array
 */
void bubblesort(int** array_record,int left, int right, int key_num);

/**
 * @brief MSD sort for multi-key record sequences
 * @param array_record Multi-key record sequence
 * @param left Start index for sorting
 * @param right End index for sorting
 * @param k Number of keys
 * @param key_num Current key index for sorting
 * @return void
 * @note This function is used to perform MSD sort on multi-key record sequences
 * @note The function sorts according to each key of the record
 * @note The larger the key index, the lower the priority
 * @note This function uses recursion to implement MSD sort
 * @note Each sort starts from the current key, recursively sorts each subsequence, and the recursive sort uses bubble sort
 * @note This function will modify the contents of the array_record array
 * @note When left equals right, it means there is only one record and no need to sort
 * @note When key_num equals k, it means all keys have been sorted
 */
void msd_sort(int** array_record, int left, int right, int k, int key_num);
#endif

