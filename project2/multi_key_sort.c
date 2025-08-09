#define _POSIX_C_SOURCE 199309L
#include "multi_key.h"

// Main function for statistics
int main(){
    struct timespec ts1, ts2;
    double time_lsd = 0.0;
    double time_msd = 0.0;
    double time_cnd = 0.0;
    int l = 0;
    // Perform multiple calculations in the program to take the average value
    srand((unsigned)time(0));// Add this line to randomize the seed
    while(l < LOOP){
        int r = (rand() % MAX_RECORD) + 1;
        int k = (rand() % MAX_KEY) + 1;
        printf("Woring on sort [%d]\trecord number[%d]\tkeys per record[%d]\n",l+1,r,k);
        //printf("r=%d\nk=%d\n",r,k);
        int **array_record = (int **)malloc(r * sizeof(int*));// Allocate space for multi-key record sequence and nodes
        for(int i = 0; i < r; i++){
            array_record[i] = (int *)malloc(k * sizeof(int));
            for(int j = 0; j < k ; j++){
                array_record[i][j] = (rand() % MAX);
            }
        }

        SLList list;
        for(int i = 0; i < r; i++){
            // Initialize the linked list
            for(int j = 0; j < k; j++){
                list.array[i + 1].keys[j] =  array_record[i][j];
            }
            list.array[i].next = i + 1;
        }
        list.array[r].next = 0; // Construct this static linked list
        list.keynum = k;
        list.length = r;

        int** array_record_msd = (int **)malloc(r * sizeof(int*));// Copy the same data for msd
        for(int i = 0; i < r; i++){
            array_record_msd[i] = (int *)malloc(k * sizeof(int));
            for(int j = 0; j < k ; j++){
                array_record_msd[i][j] = array_record[i][j];
            }
        }
        

        clock_gettime(CLOCK_MONOTONIC, &ts1);
        // Perform lsd sort
        lsd_sort(array_record,r,k);
        clock_gettime(CLOCK_MONOTONIC, &ts2);
        time_lsd += (ts2.tv_sec - ts1.tv_sec) + (ts2.tv_nsec - ts1.tv_nsec) / 1e9;


        clock_gettime(CLOCK_MONOTONIC, &ts1);
        // Use recursion for msd sort
        msd_sort(array_record_msd,0,r-1,k,0);
        clock_gettime(CLOCK_MONOTONIC, &ts2);
        time_msd += (ts2.tv_sec - ts1.tv_sec) + (ts2.tv_nsec - ts1.tv_nsec) / 1e9;

        SLList *p = &list;

        clock_gettime(CLOCK_MONOTONIC, &ts1);
        // Perform distribute and collect sort
        RadixSort(p);
        clock_gettime(CLOCK_MONOTONIC, &ts2);
        time_cnd += (ts2.tv_sec - ts1.tv_sec) + (ts2.tv_nsec - ts1.tv_nsec) / 1e9;


        // Free space
        for(int i = 0; i < r; i++){
            free(array_record[i]);
        }
        free(array_record);

        l++;
    }
    
    printf("=========================================\n");
    printf("LSD_BUBBLE_AVRAGE_TIME = %.7f\n", time_lsd/LOOP);
    printf("=========================================\n");

    printf("=========================================\n");
    printf("MSD_BUBBLE_AVRAGE_TIME = %.7f\n", time_msd/LOOP);
    printf("=========================================\n");
    
    printf("=========================================\n");
    printf("LSD_DISTRIBUTE&COLLET = %.7f\n", time_cnd/LOOP);
    printf("=========================================\n");

    return 0;
}

/**
 * @brief LSD bubble sort
 * @note The larger the key index, the lower the priority
 */
void lsd_sort(int ** arr_record,int r,int k){
    for(int i = k - 1; i >= 0; i--){  // Sort by each key, starting from the lowest priority, using bubble sort
        int* temp;
        int j = r-1;
        int sorted = FALSE;
        while(!sorted){
            sorted = TRUE;
            for(int m = 0; m < j; m++){
                if(arr_record[m+1][i] < arr_record[m][i]){  
                    temp = arr_record[m+1];
                    arr_record[m+1] = arr_record[m];
                    arr_record[m] = temp;
                    sorted = FALSE;    // Indicates an inversion was found, set sorted to 0
                }
            }
            j--;        
        }
    }
}

/**
 * @brief Print multi-key record sequence
 */
void print_arr(int**arr_record, int r, int k){
    for(int i = 0; i < r; i++){
        printf("array[%d]:",i);
        for(int j = 0; j < k; j++){
            printf("\t[%d]",arr_record[i][j]);
        }
        printf("\n");
    }
}

/**
 * @brief Distribute for static linked list
 */
void Distribute(SLCell* array, int i, int* f, int* e){
    for(int j = 0; j < RADIX; j++) f[j] = 0;    // Initialize f array
    for(int p = array[0].next; p; p = array[p].next){
        // Insert the node pointed by p into the corresponding sublist
        int m = array[p].keys[i];
        if(!f[m]) {
            f[m] = p; // If f[j] is empty, link p
        }
        else{
            array[e[m]].next = p;
        }
        e[m] = p;
    }
}

/**
 * @brief Collect for static linked list
 */
void Collect(SLCell* array, int i, int* f, int* e){
    int j;
    for(j = 0; !f[j]; j++);// Find the first non-empty sublist
    array[0].next = f[j];
    int t = e[j];
    while(j < RADIX){
        for(j++; j < RADIX; j++){// Find the next non-empty sublist
            if(f[j]){
                array[t].next = f[j];   // Merge two lists
                t = e[j];
            }
        }
    }
    array[t].next = 0;
    // The last node points to the head
}

/**
 * @brief Radix sort for static linked list
 */
void RadixSort(SLList *L){
    int* f;
    int* e;
    // Allocate space for f and e
    f = (int *)malloc(RADIX * sizeof(int));
    e = (int *)malloc(RADIX * sizeof(int));
    for(int i = L->keynum - 1; i >= 0; i--){// Sort by key, the rightmost has the lowest priority
        Distribute(L->array,i,f,e);
        Collect(L->array,i,f,e);
    }
    free(f); free(e);
}

/**
 * @brief Print static linked list
 */
void print_SLlist(SLList* L){
    int i = L->array[0].next;
    int j = 0;
    while(i){
        printf("array[%d]:",j);
        for(int k = 0; k < L->keynum; k++){
            printf("\t[%d]",L->array[i].keys[k]);
        }
        printf("\n");
        j++;
        i = L->array[i].next;
    }
}

/**
 * @brief Bubble sort for the key_num-th key between left and right in the sequence, also using bubble sort
 */
void bubblesort(int** array_record,int left, int right, int key_num){
    int j = right;
    int* temp;
    int sorted = FALSE;
    while(!sorted){
        sorted = TRUE;
        for(int m = left; m < j; m++){
            if(array_record[m+1][key_num] < array_record[m][key_num]){  
                temp = array_record[m+1];
                array_record[m+1] = array_record[m];
                array_record[m] = temp;
                sorted = FALSE;    // Indicates an inversion was found, set sorted to 0
            }
        }
        j--;        
    }
}

/**
 * @brief Implement msd algorithm using recursion
 */
void msd_sort(int** array_record, int left, int right, int k, int key_num){
    if(key_num >= k || right <= left + 1) return ;// If the current key has been sorted, or there is only one element with the same key
    bubblesort(array_record,left,right,key_num);// Sort by key_num

    int start = left;// If there are multiple records with the same key, recursively call msd to sort the next key
    while(start < right){
        int end = start + 1;
        while(end < right && array_record[start][key_num] == array_record[end][key_num]){
            end++;
        }
        msd_sort(array_record, start, end, k ,key_num + 1);// Sort by lower priority key
        start = end;    // Continue sorting
    }
}
