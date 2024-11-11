#include <stdbool.h>

/* Type for list elements (modifiable) */
typedef int elem;

/* Node structure: stores a value and points to the next node */
struct node {
	elem value;
	struct node *next;
};
typedef struct node node_t;

/* List structure with a pointer to the first node */
struct list {
	node_t *head;
};
typedef struct list list_t;

/* Allocates and frees list memory */
list_t *list_alloc();
void list_free(list_t *l);

/* Prints the list */
void list_print(list_t *l);

/* Returns a string version of the list */
char* listToString(list_t *l);

/* Allocates a node with the given value */
node_t * getNode(elem value);

/* Returns the list length */
int list_length(list_t *l);

/* Adds elements to the list */
void list_add_to_back(list_t *l, elem value);
void list_add_to_front(list_t *l, elem value);
void list_add_at_index(list_t *l, elem value, int index);

/* Removes elements from the list and returns the value */
elem list_remove_from_back(list_t *l);
elem list_remove_from_front(list_t *l);
elem list_remove_at_index(list_t *l, int index);

/* Checks if an element is in the list */
bool list_is_in(list_t *l, elem value);

/* Gets the element at a specific index (1-based, -1 if out of range) */
elem list_get_elem_at(list_t *l, int index);

/* Finds the index of an element (-1 if not found) */
int list_get_index_of(list_t *l, elem value);
