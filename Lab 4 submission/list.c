#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

list_t *list_alloc() { 
  list_t* mylist = (list_t *)malloc(sizeof(list_t)); 
  mylist->head = NULL;
  return mylist;  // returns the newly created list
}

void list_free(list_t *l) {
  if (l == NULL) {
    return; 
  }

  node_t *current = l->head;
  node_t *next_node;

  // Iterates through the list and deallocates each node
  while (current != NULL) {
    next_node = current->next;  
    free(current);              
    current = next_node;        
  }

  // Releases the memory for the list structure
  free(l);
}

char * listToString(list_t *l) {
  if (l == NULL || l->head == NULL) {
    char* empty = (char *)malloc(sizeof(char) * 10);
    strcpy(empty, "NULL");
    return empty;  
  }

  char* buf = (char *)malloc(sizeof(char) * 1024); 
  char tbuf[20];

  node_t* curr = l->head;
  while (curr != NULL) {
    sprintf(tbuf, "%d->", curr->value);
    strcat(buf, tbuf);
    curr = curr->next;
  }
  strcat(buf, "NULL");  

  return buf;  
}


void list_print(list_t *l) {
  if (l == NULL || l->head == NULL) {
    printf("List is empty.\n");
    return;  // handles the case of an empty list
  }

  node_t *current = l->head;

  // Loops through and prints each item in the list
  while (current != NULL) {
    printf("%d ", current->value);  // output the value of the current node
    current = current->next;        // moves to the following node
  }
  printf("\n");  // Newline after the list output
}

int list_length(list_t *l) {
  if (l == NULL || l->head == NULL) {
    return 0;  // Returns 0 if the list is null or empty
  }

  int length = 0;
  node_t *current = l->head;

  // Goes through each node, counting them
  while (current != NULL) {
    length++;  // Adds to length for every node
    current = current->next;
  }

  return length;
}

void list_add_to_back(list_t *l, elem value) {
  if (l == NULL) {
    return;  // Manages the null list scenario
  }

  // Creates a new node for the list
  node_t *new_node = (node_t *)malloc(sizeof(node_t));
  if (new_node == NULL) {
    return;  // Handles memory allocation issues
  }

  new_node->value = value;
  new_node->next = NULL;  // New node points to null, as it's the end

  // If the list is empty, assign new node as the head
  if (l->head == NULL) {
    l->head = new_node;
    return;
  }

  // Finds the last node in the list
  node_t *current = l->head;
  while (current->next != NULL) {
    current = current->next;
  }

  // Attaches the new node at the end
  current->next = new_node;
}

void list_add_to_front(list_t *l, elem value) {
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    if (new_node == NULL) {
        return;  // Manages memory allocation failures
    }

    new_node->value = value;
    new_node->next = l->head;
    l->head = new_node;
}

void list_add_at_index(list_t *l, elem value, int index) {
  if (l == NULL || index < 1) {
    return;  // Handles cases of null list or invalid index
  }

  node_t *new_node = (node_t *)malloc(sizeof(node_t));
  if (new_node == NULL) {
    return;  // Deals with memory allocation issues
  }
  new_node->value = value;
  new_node->next = NULL;

  // Special case for adding at the start (index 1)
  if (index == 1) {
    new_node->next = l->head;
    l->head = new_node;
    return;
  }

  // Navigate to the (index - 1) node
  node_t *current = l->head;
  int current_index = 1;

  while (current != NULL && current_index < index - 1) {
    current = current->next;
    current_index++;
  }

  new_node->next = current->next;
  current->next = new_node;
}

elem list_remove_from_back(list_t *l) {
  if (l == NULL || l->head == NULL) {
    return -1;  // Handles null or empty lists
  }

  node_t *current = l->head;
  elem removed_value;

  // Case of a single element in the list
  if (current->next == NULL) {
    removed_value = current->value;
    free(current);
    l->head = NULL;
    return removed_value;
  }

  // Moves to the second-to-last node
  node_t *prev = NULL;
  while (current->next != NULL) {
    prev = current;
    current = current->next;
  }

  // Deletes the last node
  removed_value = current->value;
  free(current);
  prev->next = NULL;  // Update the second-to-last node's next to null

  return removed_value;
}

elem list_remove_from_front(list_t *l) {
  if (l == NULL || l->head == NULL) {
    return -1;  // Manages null or empty list cases
  }

  node_t *old_head = l->head;
  elem removed_value = old_head->value;

  // Updates the head to the next node
  l->head = old_head->next;

  // Releases the memory of the previous head
  free(old_head);

  return removed_value;  // Returns the removed node's value
}

elem list_remove_at_index(list_t *l, int index) {
  if (l == NULL || l->head == NULL || index < 1) {
    return -1;  // Handles null, empty lists, or invalid index cases
  }

  node_t *current = l->head;
  elem removed_value;

  // Case for removing the first node (index 1)
  if (index == 1) {
    removed_value = current->value;
    l->head = current->next;  // Updates head to the next node
    free(current);             // Deallocates the old head
    return removed_value;
  }

  // Traverse to the (index - 1) node
  node_t *prev = NULL;
  int current_index = 1;

  while (current != NULL && current_index < index) {
    prev = current;
    current = current->next;
    current_index++;
  }

  if (current == NULL) {
    return -1;  // Index beyond list length
  }

  removed_value = current->value;
  prev->next = current->next;  // Links previous node to the following node
  free(current);                // Deallocates the targeted node

  return removed_value;
}

bool list_is_in(list_t *l, elem value) {
  if (l == NULL || l->head == NULL) {
    return false;  // Manages null or empty list scenarios
  }

  node_t *current = l->head;

  // Loops through the list to find the value
  while (current != NULL) {
    if (current->value == value) {
      return true;  // Value exists in the list
    }
    current = current->next;
  }

  return false;  // Value doesn't exist in the list
}

elem list_get_elem_at(list_t *l, int index) {
  if (l == NULL || l->head == NULL || index < 1) {
    return -1;  // Handles null, empty lists, or invalid index
  }

  node_t *current = l->head;
  int current_index = 1;

  // Proceeds until reaching the specified index
  while (current != NULL) {
    if (current_index == index) {
      return current->value;  // Returns value at the index
    }
    current_index++;
    current = current->next;
  }

  return -1;  // Index is outside the list boundaries
}

int list_get_index_of(list_t *l, elem value) {
  if (l == NULL || l->head == NULL) {
    return -1;  // Manages null or empty list cases
  }

  node_t *current = l->head;
  int index = 1;

  // Searches for the value within the list
  while (current != NULL) {
    if (current->value == value) {
      return index;  // Returns the index if value is found
    }
    index++;
    current = current->next;
  }

  return -1;  // Value is not in the list
}