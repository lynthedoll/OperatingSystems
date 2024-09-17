// list/list.c
// 
// Implementation for linked list.
//
// <Author>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

list_t *list_alloc() { 
  // Allocate memory for a new list structure
  list_t* mylist = (list_t *) malloc(sizeof(list_t)); 
  // If memory allocation was successful, initialize the list's head to NULL (empty list)
  if (mylist != NULL) {
    mylist->head = NULL;
  }
  // Return the newly created list
  return mylist;
}

// Function to free all memory used by the list
void list_free(list_t *l) {
  if (l == NULL) return;
  node_t *current = l->head;
  while (current != NULL) {
    node_t *temp = current;
    current = current->next;
    free(temp);
  }
  free(l);
}

char* listToString(list_t *l) {
    if (l == NULL) return NULL;
    char* buf = (char*)malloc(sizeof(char) * 10024);
    // Return NULL if memory allocation failed
    if (buf == NULL) return NULL;

    // Temporary buffer for each node's string representation
    char tbuf[20];
    // Initialize the main buffer as an empty string
    buf[0] = '\0';

    node_t* curr = l->head;
    while (curr != NULL) {
        // Convert the current node's value to a string with an arrow
        sprintf(tbuf, "%d->", curr->value);
        strcat(buf, tbuf);
        curr = curr->next;
    }
    // After all nodes, append "NULL" to represent the end of the list
    strcat(buf, "NULL");
    return buf;
}

// Function to find the index of a given value in the list
int list_get_index_of(list_t *l, elem value) {
    if (l == NULL) return -1;

    node_t *current = l->head;
    int index = 0;
    while (current != NULL) {
        if (current->value == value) {
            return index;
        }
        current = current->next;
        index++;
    }
    // If we've gone through the whole list without finding the value, return -1 (not found)
    return -1;
}

// Function to print the list
void list_print(list_t *l) {
  if (l == NULL) return;
  
  node_t *current = l->head;
  while (current != NULL) {
    printf("%d -> ", current->value);
    current = current->next;
  }
  // After all nodes, print "NULL" to represent the end of the list
  printf("NULL\n");
}

// Function to get the length of the list
int list_length(list_t *l) {
  // If the list is NULL, its length is 0
  if (l == NULL) return 0;
  
  int count = 0;
  // Start at the head of the list
  node_t *current = l->head;
  while (current != NULL) {
    count++;
    current = current->next;
  }
  return count;
}

// Function to add a new element to the front of the list
void list_add_to_front(list_t *l, elem value) {
  // If the list is NULL, we can't add to it, so return
  if (l == NULL) return;
  
  // Create a new node with the given value
  node_t *new_node = getNode(value);
  new_node->next = l->head;
  // Set the list's head to the new node, making it the new front of the list
  l->head = new_node;
}

// Helper function to create a new node with a given value
node_t * getNode(elem value) {
  node_t *mynode = (node_t *) malloc(sizeof(node_t));
  mynode->value = value;
  mynode->next = NULL;
  return mynode;
}

// Function to add a new element to the back of the list
void list_add_to_back(list_t *l, elem value) {
  if (l == NULL) return;
  
  // Create a new node with the given value
  node_t *new_node = getNode(value);
  
  // If the list is empty, make the new node the head
  if (l->head == NULL) {
    l->head = new_node;
  } else {
    node_t *current = l->head;
    while (current->next != NULL) {
      current = current->next;
    }
    current->next = new_node;
  }
}

// Function to remove and return the element at the front of the list
elem list_remove_from_front(list_t *l) {
    if (l == NULL || l->head == NULL) {
        return -1;  // Return -1 to indicate an error
    }
    
    node_t *to_remove = l->head;
    elem value = to_remove->value;
    l->head = to_remove->next;
    free(to_remove);
    
    return value;
}

// Function to remove and return the element at the back of the list
elem list_remove_from_back(list_t *l) {
  // If the list is NULL or empty, return -1
  if (l == NULL || l->head == NULL) return -1;
  
  // If there's only one node in the list
  if (l->head->next == NULL) {
    elem value = l->head->value;
    free(l->head);
    l->head = NULL;
    return value;
  }
  
  // Otherwise, find the second-to-last node
  node_t *current = l->head;
  while (current->next->next != NULL) {
    current = current->next;
  }
  
  // Get the value from the last node
  elem value = current->next->value;
  free(current->next);
  // Set the second-to-last node's next pointer to NULL, making it the new last node
  current->next = NULL;
  return value;
}

// Function to add a new element at a specific index in the list
void list_add_at_index(list_t *l, elem value, int index) {
  // If the list is NULL or the index is negative, we can't add, so return
  if (l == NULL || index < 0) return;
  
  // If the index is 0, add to the front of the list
  if (index == 0) {
    list_add_to_front(l, value);
    return;
  }
  
  // Create a new node with the given value, starting at the head of the list
  node_t *new_node = getNode(value);
  node_t *current = l->head;
  // Move to the node just before the desired index
  int i;
  for (i = 0; i < index - 1 && current != NULL; i++) {
    current = current->next;
  }
  
  // Return if we've reached the end of the list before the desired index
  if (current == NULL) return;
  
  // Insert the new node into the list
  new_node->next = current->next;
  current->next = new_node;
}

elem list_remove_at_index(list_t *l, int index) {
    printf("Removing at index: %d\n", index);  

    if (l == NULL) {
        printf("List is NULL\n");  
        return -1;
    }

    if (l->head == NULL) {
        printf("List is empty\n");  
        return -1;
    }

    if (index < 0) {
        printf("Invalid index: %d\n", index);  
        return -1;
    }

    if (index == 0) {
        elem value = l->head->value;
        node_t *temp = l->head;
        l->head = l->head->next;
        free(temp);
        printf("Removed %d from front\n", value);  
        return value;
    }

    node_t *current = l->head;
    node_t *previous = NULL;
    int current_index = 0;

    while (current != NULL && current_index < index) {
        previous = current;
        current = current->next;
        current_index++;
    }

    if (current == NULL) {
        printf("Index out of bounds\n");
        return -1;
    }

    elem value = current->value;
    previous->next = current->next;
    free(current);

    printf("Removed %d at index %d\n", value, index);  
    return value;
}

// Function to get the element at a specific index in the list
elem list_get_elem_at(list_t *l, int index) {
  // If the list is NULL or the index is negative, return -1
  if (l == NULL || index < 0) return -1;
  
  node_t *current = l->head;
  // Move to the node at the desired index
  int i;
  for (i = 0; i < index && current != NULL; i++) {
    current = current->next;
  }
  return (current != NULL) ? current->value : -1;
}