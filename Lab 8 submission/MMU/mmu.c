#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "list.h"
#include "util.h"

// Converts a string to uppercase for case-insensitive comparison
void TOUPPER(char *arr) {
    for (int i = 0; i < strlen(arr); i++) {
        arr[i] = toupper(arr[i]);
    }
}

// Reads input data from the file and parses the allocation policy
void get_input(char *args[], int input[][2], int *n, int *size, int *policy) {
    FILE *input_file = fopen(args[1], "r");
    if (!input_file) {
        fprintf(stderr, "Error: Invalid filepath\n");
        fflush(stdout);
        exit(0);
    }

    parse_file(input_file, input, n, size); // Parse input file
    fclose(input_file);

    TOUPPER(args[2]); // Convert policy argument to uppercase

    // Determine the memory allocation policy based on the input flag
    if ((strcmp(args[2], "-F") == 0) || (strcmp(args[2], "-FIFO") == 0))
        *policy = 1; // First Fit
    else if ((strcmp(args[2], "-B") == 0) || (strcmp(args[2], "-BESTFIT") == 0))
        *policy = 2; // Best Fit
    else if ((strcmp(args[2], "-W") == 0) || (strcmp(args[2], "-WORSTFIT") == 0))
        *policy = 3; // Worst Fit
    else {
        printf("usage: ./mmu <input file> -{F | B | W }  \n(F=FIFO | B=BESTFIT | W=WORSTFIT)\n");
        exit(1);
    }
}

// Allocates memory to a process based on the specified policy
void allocate_memory(list_t *freelist, list_t *alloclist, int pid, int blocksize, int policy) {
    /* Steps:
     * 1. Find a suitable block in the free list based on the allocation policy.
     * 2. If found:
     *      - Remove it from the free list.
     *      - Assign it to the process with the given PID.
     *      - Adjust the block size and handle fragmentation (if any).
     *      - Add the allocated block to the allocated list.
     * 3. If no suitable block is found, print an error.
     */

    node_t *current = freelist->head;
    node_t *best = NULL;
    block_t *blk;
    int best_size = -1;
    int current_size;

    // Traverse the free list to find the most suitable block based on the policy
    while (current != NULL) {
        current_size = current->blk->end - current->blk->start + 1;

        if (current_size >= blocksize) {
            switch (policy) {
                case 1: // First Fit: Select the first block that fits
                    best = current;
                    goto found;
                case 2: // Best Fit: Select the smallest block that fits
                    if (best == NULL || current_size < best_size) {
                        best = current;
                        best_size = current_size;
                    }
                    break;
                case 3: // Worst Fit: Select the largest block that fits
                    if (best == NULL || current_size > best_size) {
                        best = current;
                        best_size = current_size;
                    }
                    break;
            }
        }
        current = current->next;
    }

found:
    if (best == NULL) {
        // No suitable block found
        printf("Error: Memory Allocation %d blocks\n", blocksize);
        return;
    }

    // Remove the selected block from the free list
    blk = best->blk;
    int index = list_get_index_of(freelist, blk);
    blk = list_remove_at_index(freelist, index);

    // Handle leftover memory as a fragment if the block is larger than requested
    if ((blk->end - blk->start + 1) > blocksize) {
        block_t *fragment = malloc(sizeof(block_t));
        fragment->start = blk->start + blocksize;
        fragment->end = blk->end;
        fragment->pid = 0; // Fragment belongs to no process

        // Add the fragment back to the free list based on the policy
        if (policy == 1) {
            list_add_to_back(freelist, fragment);
        } else if (policy == 2) {
            list_add_ascending_by_blocksize(freelist, fragment);
        } else {
            list_add_descending_by_blocksize(freelist, fragment);
        }

        // Adjust the selected block's size to match the request
        blk->end = blk->start + blocksize - 1;
    }

    // Assign the block to the process and add it to the allocated list
    blk->pid = pid;
    list_add_ascending_by_address(alloclist, blk);
}

void deallocate_memory(list_t * alloclist, list_t * freelist, int pid, int policy) { 
     /* if policy == 1 -> FIFO
     *              2 -> BESTFIT 
     *              3 -> WORSTFIT
     * 
     * pid - process id of the block to deallocate 
     * alloclist - list of allocated memory blocksize
     * freelist - list of free memory blocks
     * 
     * 
    * 1. Check if a node is in the ALLOC_LIST with a blk.pid = pid
    * 2. if so, remove it and go to #3, if not print "Error: Can't locate Memory Used by PID: <pid>"
    * 3. set the blk.pid back to 0
    * 4. add the blk back to the FREE_LIST based on policy.
    */
    block_t *blk;
    int index;

    // Check if the process exists in the allocated list
    if (!list_is_in_by_pid(alloclist, pid)) {
        printf("Error: Can't locate Memory Used by PID: %d\n", pid);
        return;
    }

    // Remove the block from the allocated list
    index = list_get_index_of_by_Pid(alloclist, pid);
    blk = list_remove_at_index(alloclist, index);

    // Reset the block's PID to 0
    blk->pid = 0;

    // Add the block back to the free list based on the policy
    if (policy == 1) {
        list_add_to_back(freelist, blk);
    } else if (policy == 2) {
        list_add_ascending_by_blocksize(freelist, blk);
    } else {
        list_add_descending_by_blocksize(freelist, blk);
    }
}

// Sorts the free list by address and merges adjacent free blocks
list_t* coalese_memory(list_t *list) {
    list_t *temp_list = list_alloc();
    block_t *blk;

    // Sort the free list by address
    while ((blk = list_remove_from_front(list)) != NULL) {
        list_add_ascending_by_address(temp_list, blk);
    }

    // Merge adjacent blocks
    list_coalese_nodes(temp_list);

    return temp_list;
}

// Prints the contents of a list with a message header
void print_list(list_t *list, char *message) {
    node_t *current = list->head;
    block_t *blk;
    int i = 0;

    printf("%s:\n", message);

    while (current != NULL) {
        blk = current->blk;
        printf("Block %d:\t START: %d\t END: %d", i, blk->start, blk->end);

        if (blk->pid != 0)
            printf("\t PID: %d\n", blk->pid);
        else
            printf("\n");

        current = current->next;
        i += 1;
    }
}

// Main function to simulate memory management
int main(int argc, char *argv[]) {
    int PARTITION_SIZE, inputdata[200][2], N = 0, Memory_Mgt_Policy;

    list_t *FREE_LIST = list_alloc();   // List of free memory blocks
    list_t *ALLOC_LIST = list_alloc();  // List of allocated memory blocks
    int i;

    if (argc != 3) {
        printf("usage: ./mmu <input file> -{F | B | W }  \n(F=FIFO | B=BESTFIT | W=WORSTFIT)\n");
        exit(1);
    }

    get_input(argv, inputdata, &N, &PARTITION_SIZE, &Memory_Mgt_Policy);

    // Initialize the free list with the entire partition size
    block_t *partition = malloc(sizeof(block_t));
    partition->start = 0;
    partition->end = PARTITION_SIZE + partition->start - 1;
    list_add_to_front(FREE_LIST, partition);

    // Process each input operation
    for (i = 0; i < N; i++) {
        printf("************************\n");
        if (inputdata[i][0] != -99999 && inputdata[i][0] > 0) {
            printf("ALLOCATE: %d FROM PID: %d\n", inputdata[i][1], inputdata[i][0]);
            allocate_memory(FREE_LIST, ALLOC_LIST, inputdata[i][0], inputdata[i][1], Memory_Mgt_Policy);
        } else if (inputdata[i][0] != -99999 && inputdata[i][0] < 0) {
            printf("DEALLOCATE MEM: PID %d\n", abs(inputdata[i][0]));
            deallocate_memory(ALLOC_LIST, FREE_LIST, abs(inputdata[i][0]), Memory_Mgt_Policy);
        } else {
            printf("COALESCE/COMPACT\n");
            FREE_LIST = coalese_memory(FREE_LIST);
        }

        printf("************************\n");
        print_list(FREE_LIST, "Free Memory");
        print_list(ALLOC_LIST, "\nAllocated Memory");
        printf("\n\n");
    }

    list_free(FREE_LIST);
    list_free(ALLOC_LIST);

    return 0;
}
