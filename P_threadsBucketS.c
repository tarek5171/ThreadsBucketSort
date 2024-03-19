#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define NUM_THREADS 10
#define ARRAY_SIZE 99999
#define GIVEN_NUMBER 100 //max number for random number generator

int global_array[ARRAY_SIZE];
int sorted_global_array[ARRAY_SIZE]; // New global array to store sorted values from each thread
int cumulative_sizes[NUM_THREADS];  // Array to store cumulative sizes of local arrays from all threads
pthread_barrier_t barrier; // Barrier synchronization

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

void *thread_function(void *arg) {
    clock_t start, end;
    double cpu_time_used;

    // Record the start time
    start = clock();
    int thread_num = *((int *) arg);
    int min = thread_num * GIVEN_NUMBER / NUM_THREADS + 1;
    int max = (thread_num + 1) * GIVEN_NUMBER / NUM_THREADS;
    int *local_array = (int *)malloc(sizeof(int) * ARRAY_SIZE); // Dynamically allocate memory
    int local_count = 0; // Counter for elements stored in the local array

    // elements within min and max
    for (int i = 0; i < ARRAY_SIZE; i++) {
        if (global_array[i] >= min && global_array[i] <= max) {
            local_array[local_count++] = global_array[i];
        }
    }
    cumulative_sizes[thread_num] = local_count;

    insertionSort(local_array, local_count);

    // Calculate the starting index in sorted_global_array based on cumulative sizes
    int start_index = 0;
    for (int i = 0; i < thread_num; i++) {
        start_index += cumulative_sizes[i];
    }

    // Copy local array to the sorted_global_array based on cumulative sizes
    for (int i = 0; i < local_count; i++) {
        sorted_global_array[start_index + i] = local_array[i];
    }

    free(local_array);
    end = clock();

    // Calculate the CPU time used
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    //printf("Thread %d execution time: %f seconds %d\n", thread_num, cpu_time_used, local_count);

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_args[NUM_THREADS];
    int i, rc;
    clock_t start, end;
    double cpu_time_used;

    // Seed the random number generator
    srand(time(NULL));

    // Initialize barrier
    pthread_barrier_init(&barrier, NULL, NUM_THREADS);

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

    // Create threads
    for (i = 0; i < NUM_THREADS; i++) {
        thread_args[i] = i; // Set the thread argument to the loop counter
        rc = pthread_create(&threads[i], NULL, thread_function, (void *)&thread_args[i]);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            return -1;
        }
    }

    // Wait for all threads to complete
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    end = clock(); // End time

    // Destroy barrier
    pthread_barrier_destroy(&barrier);

    /*printf("Array after sorting:\n");
    for (i = 0; i < ARRAY_SIZE; i++) {
        printf("%d ", sorted_global_array[i]);
    }
    printf("\n");*/

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Total execution time: %f seconds\n", cpu_time_used);

    return 0;
}
