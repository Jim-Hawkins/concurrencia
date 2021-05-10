#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "queue.h"


//To create a queue
queue* queue_init(int capacity){
    queue * q = (queue *) malloc(sizeof(queue)); //create a queue-type variable dinamically allocated
    q->size = 0; 				   //the queue is empty at the beginning
    q->capacity = capacity;			   //set the queue's capacity
    q->head = 0; 				   //we will extract elements from the leftmost side of the queue
    q->tail = capacity - 1;			   //we will insert elements in the rightmost side of the queue
    							//(refer to the memory of the project for more clearness)
    q->array = (struct element*) malloc(q->capacity * sizeof(struct element));
    						   //dinamically create an array of 'capacity' elements
    return q;
}


// To Enqueue an element
int queue_put(queue *q, struct element* x) {
						/*VA A CAMBIAR CUANDO METAMOS MUTEX Y CONDITIONS*/
    //check if the queue is full
    if(queue_full(q)){
	printf("Queue is full");
	return -1;
	}
    q->tail = (q->tail + 1) % q->capacity; //when inserting, tail increases (moves to the right)
    q->array[q->tail] = *x;
    q->size = q->size + 1;
    return 0;
}


// To Dequeue an element.
struct element* queue_get(queue *q) {
							/*VA A CAMBIAR CUANDO METAMOS MUTEX Y CONDITIONS*/
    struct element * candidate;
    printf("long de cola: %d\n",q->size);
    //check if the queue is empty
    if(queue_empty(q)==1){
    	printf("estoy vacia");	
	return candidate;
    }
    else {
	candidate = &(q->array[q->head]);
	printf("ver que saca la cola(tipo): %d\n",candidate->type);	    //extractions are done by the head of the queue
	printf("ver que saca la cola(tiempo): %d\n",candidate->time);
    	q->head = (q->head + 1) % q->capacity;    //index 'head' increases in order to point to the next element
    	q->size = q->size - 1;
	return candidate;	
	}
    
}


//To check queue state
int queue_empty(queue *q){
    if(q->size <= 0){
	return 1;
    }
    return 0;
}

int queue_full(queue *q){
    if (q->size == q->capacity){
	return 1;
    }
    return 0;
}

//To destroy the queue and free the resources
int queue_destroy(queue *q){
    //free the array and the queue itself (which were creacted with a malloc)
    free(q->array);
    free(q);
    return 0;
}
