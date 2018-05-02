#include <stdio.h>
#include <stdlib.h>
#include "../libfractal/fractal.h"

/*
 * Source (doc + some code) : Inginious exercises available at the following url
 * https://inginious.info.ucl.ac.be/course/LSINF1252/simple_stack
 */

/*
* noeud du stack
*/
typedef struct node {
  struct node *next;
  fractal_t *fractal;
} node_t;



/**
* Add @value at the "top" of the stack.
*
* @head : pointer to the top of the stack
* @value : the fractal to be placed in the element at the top of the stack
*
* @return 0 if no error, 1 otherwise
*/
int pushInBuffer(node_t **head, fractal_t *value);

/**
 * Empties the buffer (with frees)
 *
 * @head : pointer to the top of the stack
 */
void flushBuffer(node_t *head);


/**
* Remove the top element of the stack and return its content.
*
* @head : pointer to the top of the stack
* @result : pointer to store the popped element
*
* @return 0 if no error, 1 otherwise
*
*/
fractal_t* popFromBuffer(struct node **head);
