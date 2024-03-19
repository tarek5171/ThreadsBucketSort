#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define NUM_THREADS 10
#define ARRAY_SIZE 99999
#define GIVEN_NUMBER 100

int global_array[ARRAY_SIZE];
int local_array[NUM_THREADS][ARRAY_SIZE]; // Sub-array for each thread
int local_counts[NUM_THREADS]; // To store the count of elements for each thread
int sorted_array[ARRAY_SIZE]; // Global array to store sorted elements

void insertionSort(int arr[], int n) {
    int i, key, j;
    for (i = 1; i < n; i++) {
        key = arr[i];
        j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}

int main() {
    // Generate array of random numbers
    srand(time(NULL));
    for (int i = 0; i < ARRAY_SIZE; i++) {
        global_array[i] = rand() % GIVEN_NUMBER + 1; // Random number between 1 and GIVEN_NUMBER
    }
    // Print the sorted global array
    printf("Sorted array: ");
    for (int i = 0; i < ARRAY_SIZE; i++) {
        printf("%d ", global_array[i]);
    }
    printf("\n");

    clock_t start, end;
    double cpu_time_used;

    // Record the start time
    start = clock();

    // Specify the number of threads to use
    omp_set_num_threads(NUM_THREADS);

    // Parallel region starts
    #pragma omp parallel
    {
        // Get the thread number
        int thread_num = omp_get_thread_num();
        
        // Define the interval for this thread
        int min = thread_num * GIVEN_NUMBER / NUM_THREADS + 1;
        int max = (thread_num + 1) * GIVEN_NUMBER / NUM_THREADS;

        // Initialize local count for this thread
        int local_count = 0;

        // Traverse global array and select elements falling in the interval
        for (int i = 0; i < ARRAY_SIZE; i++) {
            if (global_array[i] >= min && global_array[i] <= max) {
                local_array[thread_num][local_count++] = global_array[i];
            }
        }

        // Store local count for this thread
        local_counts[thread_num] = local_count;

        // Sort the local array using insertion sort
        insertionSort(local_array[thread_num], local_count);
    } // Parallel region ends

    // Parallel loop to gather sorted sub-arrays into the global sorted array
    #pragma omp parallel for
    for (int i = 0; i < NUM_THREADS; i++) {
        int index = 0;
        for (int j = 0; j < i; j++) {
            index += local_counts[j];
        }
        for (int j = 0; j < local_counts[i]; j++) {
            sorted_array[index + j] = local_array[i][j];
        }
    }

    // Stop measuring time
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    

    // Print the sorted global array
    printf("Sorted array: ");
    for (int i = 0; i < ARRAY_SIZE; i++) {
        printf("%d ", sorted_array[i]);
    }
    printf("\n");

    // Print the runtime
    printf("Total execution time: %f seconds\n", cpu_time_used);

    return 0;
}
