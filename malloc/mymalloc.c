#include "mymalloc.h"
#include <unistd.h>
#include <stdio.h>


// Struct which will serve as the "nodes" in the metadata linked list regarding memory
struct node{
	struct node *next;
	struct node *prev;
	int size;
	int allocated;
};


// Node pointers which will be used to keep track of the head and tail nodes
struct node *head = NULL;
struct node *tail = NULL;

// The following method will iterate through the metadata nodes and coalesce neighboring unallocated nodes into a single unallocated node
void condense(){
	struct node *current = head;
	while(current->next != NULL){
		if(current->allocated == 0 && current->next->allocated == 0){
			if(current->next == tail){
				current->size = current->size + current->next->size;
				tail->prev = NULL;
				current->next = NULL;
				tail = current;
				break;
			}
			else{
				current->size = current->size + current->next->size;
				current->next = current->next->next;
				if(current->next != NULL){
					current->next->prev = current;
				}
			}
		}
		current = current->next;
	}
}

// Returns a pointer to an unallocated "section" of memory which is closest is size to the size requested by the user
struct node *find_best_fit(int size_to_fit){
	struct node *current = head;
	struct node *smallest = NULL;
	int smallest_size = 1000000000;
	while(current != NULL){
		if(current->size < smallest_size && current->size >= size_to_fit && current->allocated == 0){
			smallest_size = current->size;
			smallest = current;
		}
		current = current->next;
	}
	return smallest;
}

// implementation of malloc
void *my_bestfit_malloc(int requested_size){
	
	// If the user has requested a size less than or equal to zero, just return null
	if(requested_size <= 0){
		return NULL;
	}
	
	// pointers used in various checks and assignments
	struct node *new_node;
	void *addr_to_return;
	
	// If there are no nodes allocated, don't need to check anything rather just allocate the required space and set the head/tail references
	if(head == NULL){
		new_node = sbrk(requested_size + sizeof(struct node));
		head = new_node;
		tail = new_node;
		new_node->size = requested_size;
		new_node->next = NULL;
		new_node->prev = NULL;
		new_node->allocated = 1;
		addr_to_return = (new_node+1);
		
		// check that sbrk didn't fail, if it did, return null
		if(addr_to_return == (void*) -1){
				return NULL;
			}
	}
	
	// If there are already allocated nodes, first check to see if there are any unallocated nodes at which we could store our data, if so store the data in an unallocated node, splitting 
	// that node if its size is greater than the space required. If there are no unallocated nodes, we have to request more space using sbrk, then update the tail reference
	else{
		new_node = find_best_fit(requested_size);
		if(new_node == NULL){
			new_node = sbrk(requested_size + sizeof(struct node));
			new_node->size = requested_size;
			new_node->allocated = 1;
			new_node->next = NULL;
			new_node->prev = tail;
			tail->next = new_node;
			tail = new_node;
			addr_to_return = (new_node+1);
			if(addr_to_return == (void*) -1){
				return NULL;
			}
		}
		else{
			if(new_node->size > requested_size && new_node != tail){
				int left_over = new_node->size - requested_size;
				struct node *extra_space = ((new_node->next)-(left_over + sizeof(struct node)));
				extra_space->size = left_over;
				extra_space->next = new_node->next;
				extra_space->prev = new_node;
				extra_space->next->prev = extra_space;
				extra_space->allocated = 0;
				new_node->next = extra_space;
				new_node->size = requested_size;
				new_node->allocated = 1;
				addr_to_return = (new_node+1);
			}
			if(new_node->size > requested_size && new_node == tail){
				int left_over = new_node->size - requested_size;
				struct node *extra_space = sbrk(left_over + sizeof(struct node));
				extra_space->size = left_over;
				extra_space->next = NULL;
				extra_space->prev = new_node;
				new_node->next = extra_space;
				tail = extra_space;
				extra_space->allocated = 0;
				new_node->size = requested_size;
				new_node->allocated = 1;
				addr_to_return = (new_node+1);
			}
			else{
				new_node->allocated = 1;
				addr_to_return = (new_node+1);
			}
		}
	}
		
	return addr_to_return;
}

// implementation of free
void my_free(void *ptr){
	struct node *to_free;
	// Subtract the size of a node struct from the ptr argument above to get the node associated with that ptr
	to_free = (ptr - (sizeof(struct node)));
	
	// if the node being freed is the only node just reduce brk back to the initial position
	if(to_free == head && head == tail){
		head = NULL;
		tail = NULL;
		sbrk(0 - (to_free->size + sizeof(struct node)));
		to_free = NULL;
	}
	
	// if the node being freed is the tail node, and the node previous is the unallocated head node, we can simply reduce brk back to the starting position rather
	// than trying to coalesce neighboring unallocated nodes
	else if(to_free == tail && tail->prev == head && head->allocated == 0){
		head = NULL;
		tail = NULL;
		sbrk(0 - (to_free->size + to_free->prev->size + 2*sizeof(struct node)));
		to_free = NULL;
	}
	
	// Opposite case of the else if above, if the node being freed is the head node and the next node is an unallocated tail node, then we just reduce brk back to
	// the starting position
	else if(to_free == head && head->next == tail && tail->allocated == 0){
		head = NULL;
		tail = NULL;
		sbrk(0 - (to_free->size + to_free->next->size + 2*sizeof(struct node)));
		to_free = NULL;
	}
	// If the node to be freed is a tail node, we just reduce brk by the size of that node, plus the size of a node struct
	// we also change the tail reference to reflect the change
	else if(to_free == tail){
		tail = to_free->prev;
		tail->next = NULL;
		to_free->prev = NULL;
		sbrk(0 - (to_free->size + sizeof(struct node)));
		to_free = NULL;
		condense();
	}
	
	// If none of the cases above apply, we simply set the allocated "boolean" to false
	else{
		to_free->allocated = 0;
		condense();
	}
}

// Function which prints out all the nodes in the list, used for testing
void printlist(){
	struct node *current;
	current = head;
	if(current == NULL){
		printf("Nothing to print\n");
	}
	printf("Head: %d\nTail: %d\n\n", head, tail);
	while(current != NULL){
		printf("Addr: %d\nsize: %d\nnext: %d\nprev: %d\nalloc: %d\n*********************\n", current, current->size, current->next, current->prev, current->allocated);
		current = current->next;
	}
	
	printf("\n\n");
}

