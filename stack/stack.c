#include<stdlib.h>
#include "stack.h"


int push(node_t **head, fractal_t *value){
	if(head == NULL) return 1;
	node_t *newhead = (struct node *) malloc(sizeof(struct node));
	if (newhead == NULL) return 1;
	newhead->fractal = value;
	newhead->next = (*head);
	*head = newhead;
	return 0;
}



fractal_t* pop(struct node **head){
	if(head == NULL) return 1;
	node_t * oldhead = *head;
	*head = oldhead->next;
	fractal_t *popped_fractal  =  oldhead->fractal;
	free (oldhead);
	return popped_fractal;
}
