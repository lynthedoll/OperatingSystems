#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

// Allocates and initializes an empty linked list
list_t *list_alloc() { 
    list_t* list = (list_t*)malloc(sizeof(list_t));
    list->head = NULL;
    return list; 
}

// Allocates and initializes a new node with the given block
node_t *node_alloc(block_t *blk) {   
    node_t* node = malloc(sizeof(node_t));
    node->next = NULL;
    node->blk = blk;
    return node; 
}

// Frees the memory allocated for a linked list
void list_free(list_t *l) {
    free(l);
    return;
}

// Frees the memory allocated for a node
void node_free(node_t *node) {
    free(node);
    return;
}

// Prints the contents of the list
void list_print(list_t *l) {
    node_t *current = l->head;
    block_t *b;
  
    if (current == NULL) {
        printf("list is empty\n");
    }
    while (current != NULL) {
        b = current->blk;
        printf("PID=%d START:%d END:%d", b->pid, b->start, b->end);
        current = current->next;
    }
}

// Returns the number of nodes in the list
int list_length(list_t *l) { 
    node_t *current = l->head;
    int i = 0;
    while (current != NULL) {
        i++;
        current = current->next;
    }
    return i; 
}

// Adds a block to the end of the list
void list_add_to_back(list_t *l, block_t *blk) {  
    node_t* newNode = node_alloc(blk);
    newNode->next = NULL;
    if (l->head == NULL) {
        l->head = newNode;
    } else {
        node_t *current = l->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
}

// Adds a block to the front of the list
void list_add_to_front(list_t *l, block_t *blk) {  
    node_t* newNode = node_alloc(blk);
    newNode->next = l->head;
    l->head = newNode;
}

// Inserts a block at a specific index in the list
void list_add_at_index(list_t *l, block_t *blk, int index) {
    int i = 0;
    node_t *newNode = node_alloc(blk);
    node_t *current = l->head;

    if (index == 0) {
        newNode->next = l->head->next;
        l->head = newNode;
    } else if (index > 0) {
        while (i < index && current->next != NULL) {
            current = current->next;
            i++;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

// Adds a block to the list in ascending order of start address
void list_add_ascending_by_address(list_t *l, block_t *newblk) {
    node_t *current;
    node_t *prev;
    node_t *newNode = node_alloc(newblk);

    if (l->head == NULL) {
        l->head = newNode;
    } else {
        prev = current = l->head;

        if (newblk->start < current->blk->start) { // Add at the front
            newNode->next = l->head;
            l->head = newNode;
            return;
        }

        while (current != NULL && newblk->start > current->blk->start) {
            prev = current;
            current = current->next;
        }

        prev->next = newNode;
        newNode->next = current;
    }
}

// Adds a block to the list in ascending order of block size
void list_add_ascending_by_blocksize(list_t *l, block_t *newblk) {
    node_t *current;
    node_t *prev;
    node_t *newNode = node_alloc(newblk);
    int newblk_size = newblk->end - newblk->start + 1;
    int curblk_size;

    if (l->head == NULL) {
        l->head = newNode;
    } else {
        prev = current = l->head;
        curblk_size = current->blk->end - current->blk->start + 1;

        if (newblk_size < curblk_size) { // Add at the front
            newNode->next = l->head;
            l->head = newNode;
            return;
        }

        while (current != NULL && newblk_size >= (current->blk->end - current->blk->start + 1)) {
            prev = current;
            current = current->next;
        }

        prev->next = newNode;
        newNode->next = current;
    }
}

// Adds a block to the list in descending order of block size
void list_add_descending_by_blocksize(list_t *l, block_t *blk) {
    node_t *current;
    node_t *prev;
    node_t *newNode = node_alloc(blk);
    int newblk_size = blk->end - blk->start;
    int curblk_size;

    if (l->head == NULL) {
        l->head = newNode;
    } else {
        prev = current = l->head;
        curblk_size = current->blk->end - current->blk->start + 1;

        if (current->next == NULL) { // Only one node
            if (newblk_size >= curblk_size) { // Add in front
                newNode->next = l->head;
                l->head = newNode;
            } else { // Add behind
                current->next = newNode;
                newNode->next = NULL;
            }
        } else { // Two or more nodes
            if (newblk_size >= curblk_size) { // Add in front
                newNode->next = l->head;
                l->head = newNode;
            } else {
                while (current != NULL && newblk_size <= curblk_size) {
                    prev = current;
                    current = current->next;

                    if (current != NULL) { // Update size for next node
                        curblk_size = current->blk->end - current->blk->start;
                    }
                }
                prev->next = newNode;
                newNode->next = current;
            }
        }
    }
}

// Combines adjacent memory blocks in the list
void list_coalese_nodes(list_t *l) {
    node_t *prev;
    node_t *current;

    if (l->head == NULL || l->head->next == NULL) {
        return; // Nothing to coalesce
    }

    prev = l->head;
    current = l->head->next;

    while (current != NULL) {
        if (prev->blk->end + 1 == current->blk->start) { // Adjacent blocks
            prev->blk->end = current->blk->end; // Merge blocks
            prev->next = current->next; // Remove current node
            node_free(current);
            current = prev->next; // Move to next
        } else {
            prev = current;
            current = current->next; // Continue traversal
        }
    }
}

block_t* list_remove_from_back(list_t *l){
  block_t *value = NULL;
  node_t *current = l->head;

  if(l->head != NULL){
    
    if(current->next == NULL) { // one node
         l->head->next = NULL;
         value = current->blk;
         node_free(current);
    }
    else {
         while (current->next->next != NULL){
            current = current->next;
         }
         value = current->blk;
         node_free(current->next);
         current->next = NULL;
    }
  }
  return value;
}

block_t* list_get_from_front(list_t *l) {
  block_t *value = NULL;
  if(l->head == NULL){
    return value;
  }
  else{
    node_t *current = l->head;
    value = current->blk;
  }
  return value; 
}


block_t* list_remove_from_front(list_t *l) { 
  block_t *value = NULL;
  if(l->head == NULL){
    return value;
  }
  else{
    node_t *current = l->head;
    value = current->blk;
    l->head = l->head->next;
    node_free(current);
  }
  return value; 
}

block_t* list_remove_at_index(list_t *l, int index) { 
  int i;
  block_t* value = NULL;
  
  bool found = false;
  
  if(l->head == NULL){
    return value;
  }
  else if (index == 0){
    return list_remove_from_front(l);
  }
  else if (index > 0){
    node_t *current = l->head;
    node_t *prev = current;
    
    i = 0;
    while(current != NULL && !found){
      if(i == index)
          found = true;
      else {
         prev = current;
         current = current->next;
         i++;
      }
    }
    
    if(found) {
      value = current->blk; 
      prev->next = current->next;
      node_free(current);
    }
  }
  return value; 
}

bool compareBlks(block_t* a, block_t *b) {
  
  if(a->pid == b->pid && a->start == b->start && a->end == b->end)
     return true;
  
  return false;
}

bool compareSize(int a, block_t *b) {  // greater or equal
  
  if(a <= (b->end - b->start) + 1)
     return true;
  
  return false;
}

bool comparePid(int a, block_t *b) {
  
  if(a == b->pid)
     return true;
  
  return false;
}


bool list_is_in(list_t *l, block_t* value) { 
  node_t *current = l->head;
  while(current != NULL){
    if(compareBlks(value, current->blk)){
      return true;
    }
    current = current->next;
  }
return false; 
}

block_t* list_get_elem_at(list_t *l, int index) { 
 int i;
  block_t* value = NULL;
  if(l->head == NULL){
    return value;
  }
  else if (index == 0){
    return list_get_from_front(l);
  }
  else if (index > 0){
    node_t *current = l->head;
    
    i = 0;
    while(current != NULL){
      if(i == index)
          return(current->blk);
      else {
         current = current->next;
         i++;
      }
    }
  }
  return value; 
}

int list_get_index_of(list_t *l, block_t* value){
 int i = 0;
 node_t *current = l->head;
 if(l->head == NULL){
    return -1;
  }
  
  while (current != NULL){
   if (compareBlks(value,current->blk)){
     return i;
    }
    current = current->next;
    i++;
  }
  return -1; 
}

/* Checks to see if block of Size or greater exists in the list. */
bool list_is_in_by_size(list_t *l, int Size){ 
  node_t *current = l->head;
  while(current != NULL){
    if(compareSize(Size, current->blk)){
      return true;
    }
    current = current->next;
  }
return false; 
}

/* Checks to see if pid of block exists in the list. */
bool list_is_in_by_pid(list_t *l, int pid){ 
    node_t *current = l->head;
    while(current != NULL) {
        if(comparePid(pid, current->blk)) {
            return true;
        }
        current = current->next;
    }
    return false;
}

/* Returns the index at which the given block of Size or greater appears. */
int list_get_index_of_by_Size(list_t *l, int Size){
 int i = 0;
 node_t *current = l->head;
 if(l->head == NULL){
    return -1;
  }
  
  while (current != NULL){
   if (compareSize(Size,current->blk)){
     return i;
    }
    current = current->next;
    i++;
  }

  return -1; 
}
                   
/* Returns the index at which the pid appears. */
int list_get_index_of_by_Pid(list_t *l, int pid){
 int i = 0;
 node_t *current = l->head;
 if(l->head == NULL){
    return -1;
  }
  
  while (current != NULL){
   if (comparePid(pid,current->blk)){
     return i;
    }
    current = current->next;
    i++;
  }

  return -1; 
}
