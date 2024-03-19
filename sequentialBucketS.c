#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define ARRAY_SIZE 99999
#define GIVEN_NUMBER 100
#define NUM_BUCKETS 4

int global_array[ARRAY_SIZE];
int buckets[NUM_BUCKETS][ARRAY_SIZE];
int bucket_sizes[NUM_BUCKETS] = {0};

void insertionSort(int arr[], int n) {
    int i, key, j;
    for (i = 1; i < n; i++) {
        key = arr[i];
        j = i - 1;

        /* Move elements of arr[0..i-1], that are greater than key, to one position ahead of their current position */
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}

void distributeToBuckets() {
    int i;
    for (i = 0; i < ARRAY_SIZE; i++) {
        int bucket_index = global_array[i] / (ARRAY_SIZE / NUM_BUCKETS);
        buckets[bucket_index][bucket_sizes[bucket_index]++] = global_array[i];
    }
}

void mergeBuckets() {
    int i, j, index = 0;
    for (i = 0; i < NUM_BUCKETS; i++) {
        for (j = 0; j < bucket_sizes[i]; j++) {
            global_array[index++] = buckets[i][j];
        }
    }
}

int main() {
    int i;
    clock_t start, end;
    double cpu_time_used;

    // Seed the random number generator
    srand(time(NULL));

    // Fill the global array with random and non-sorted numbers between 1 and 20
    for (i = 0; i < ARRAY_SIZE; i++) {
        global_array[i] = rand() % GIVEN_NUMBER + 1; // Random number between 1 and 20
    }

    /*printf("Array before sorting:\n");
    for (i = 0; i < ARRAY_SIZE; i++) {
        printf("%d ", global_array[i]);
    }
    printf("\n");*/

    start = clock(); // Start time

    // Distribute elements into buckets
    distributeToBuckets();

    // Sort each bucket
    for (i = 0; i < NUM_BUCKETS; i++) {
        insertionSort(buckets[i], bucket_sizes[i]);
    }

    // Merge sorted buckets
    mergeBuckets();

    end = clock(); // End time

    /*printf("Array after sorting:\n");
    for (i = 0; i < ARRAY_SIZE; i++) {
        printf("%d ", global_array[i]);
    }
    printf("\n");*/

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Total execution time: %f seconds\n", cpu_time_used);

    return 0;
}
