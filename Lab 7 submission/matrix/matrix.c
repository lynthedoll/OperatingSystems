#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX 20

int matA[MAX][MAX]; 
int matB[MAX][MAX]; 

int matSumResult[MAX][MAX];
int matDiffResult[MAX][MAX]; 
int matProductResult[MAX][MAX]; 

// Structure to store arguments for each thread, including thread ID and the range of rows it will process
typedef struct {
    int thread_id;
    int start_row;
    int end_row;
} ThreadArgs;

// Fills a matrix with random integers between 1 and 10
void fillMatrix(int matrix[MAX][MAX]) {
    for(int i = 0; i < MAX; i++) {
        for(int j = 0; j < MAX; j++) {
            matrix[i][j] = rand() % 10 + 1;
        }
    }
}

// Prints the contents of a matrix in a formatted layout
void printMatrix(int matrix[MAX][MAX]) {
    for(int i = 0; i < MAX; i++) {
        for(int j = 0; j < MAX; j++) {
            printf("%5d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Computes the sum of corresponding elements in two matrices for the assigned rows
void* computeSum(void* args) {
    ThreadArgs* thread_args = (ThreadArgs*)args;
    int start_row = thread_args->start_row;
    int end_row = thread_args->end_row;
    
    // Loops through the assigned rows to calculate the sum
    for(int i = start_row; i < end_row; i++) {
        for(int j = 0; j < MAX; j++) {
            matSumResult[i][j] = matA[i][j] + matB[i][j];
        }
    }
    
    free(args);  // Frees the allocated memory for thread arguments
    return NULL;
}

// Computes the difference between corresponding elements in two matrices for the assigned rows
void* computeDiff(void* args) {
    ThreadArgs* thread_args = (ThreadArgs*)args;
    int start_row = thread_args->start_row;
    int end_row = thread_args->end_row;
    
    // Loops through the assigned rows to calculate the difference
    for(int i = start_row; i < end_row; i++) {
        for(int j = 0; j < MAX; j++) {
            matDiffResult[i][j] = matA[i][j] - matB[i][j];
        }
    }
    
    free(args);  // Frees the allocated memory for thread arguments
    return NULL;
}

// Computes the product of two matrices for the assigned rows
void* computeProduct(void* args) {
    ThreadArgs* thread_args = (ThreadArgs*)args;
    int start_row = thread_args->start_row;
    int end_row = thread_args->end_row;
    
    // Loops through the assigned rows to calculate the product
    for(int i = start_row; i < end_row; i++) {
        for(int j = 0; j < MAX; j++) {
            matProductResult[i][j] = 0;  // Initialize result to zero
            for(int k = 0; k < MAX; k++) {
                matProductResult[i][j] += matA[i][k] * matB[k][j];
            }
        }
    }
    
    free(args);  // Frees the allocated memory for thread arguments
    return NULL;
}

int main() {
    srand(time(0));  // Seed the random number generator
    
    // Fill matrices A and B with random values
    fillMatrix(matA);
    fillMatrix(matB);
    
    // Display the original matrices
    printf("Matrix A:\n");
    printMatrix(matA);
    printf("Matrix B:\n");
    printMatrix(matB);
    
    // Create thread objects and determine the number of rows each thread will process
    pthread_t threads[10];
    int num_threads = 10;
    int rows_per_thread = MAX / num_threads;
    
    // Spawn threads to compute matrix addition
    for(int i = 0; i < num_threads; i++) {
        ThreadArgs* args = malloc(sizeof(ThreadArgs));
        args->thread_id = i;
        args->start_row = i * rows_per_thread;
        args->end_row = (i == num_threads - 1) ? MAX : (i + 1) * rows_per_thread;
        pthread_create(&threads[i], NULL, computeSum, (void*)args);
    }
    
    // Wait for all addition threads to complete
    for(int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Repeat the process for matrix subtraction
    for(int i = 0; i < num_threads; i++) {
        ThreadArgs* args = malloc(sizeof(ThreadArgs));
        args->thread_id = i;
        args->start_row = i * rows_per_thread;
        args->end_row = (i == num_threads - 1) ? MAX : (i + 1) * rows_per_thread;
        pthread_create(&threads[i], NULL, computeDiff, (void*)args);
    }
    
    for(int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Repeat the process for matrix multiplication
    for(int i = 0; i < num_threads; i++) {
        ThreadArgs* args = malloc(sizeof(ThreadArgs));
        args->thread_id = i;
        args->start_row = i * rows_per_thread;
        args->end_row = (i == num_threads - 1) ? MAX : (i + 1) * rows_per_thread;
        pthread_create(&threads[i], NULL, computeProduct, (void*)args);
    }
    
    for(int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Display the results of the operations
    printf("Results:\n");
    printf("Sum:\n");
    printMatrix(matSumResult);
    printf("Difference:\n");
    printMatrix(matDiffResult);
    printf("Product:\n");
    printMatrix(matProductResult);
    
    return 0;
}
