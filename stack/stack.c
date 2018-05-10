#include <stdlib.h>
#include "stack.h"




int pushInBuffer(node_t **head, fractal_t *value){
    node_t *newhead = (node_t *) malloc(sizeof(node_t));
    if (newhead == NULL) return EXIT_FAILURE;
  	newhead->fractal = value;
  	newhead->next = (*head);
  	*head = newhead;

	return EXIT_SUCCESS;
}

fractal_t* popFromBuffer(struct node **head){
	if(head == NULL) return NULL;
    node_t * oldhead = *head;
    *head = oldhead->next;
    fractal_t *popped_fractal  =  oldhead->fractal;
    free(oldhead);
    return popped_fractal;
}

void flushBuffer(node_t *head) {
    if (head != NULL) {
        node_t *newHead = head->next;
        //fractal_free(head->fractal);
        //free(head);
        flushBuffer(newHead);
    }
}
