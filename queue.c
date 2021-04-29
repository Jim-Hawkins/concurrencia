


#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "queue.h"



//To create a queue
queue* queue_init(int capacity){
    
    queue * q = (queue *)malloc(sizeof(queue));//queue es un tipoo de dato (como si fuera un int)
    q->size = 0; //tamano inicial de la cola es cero (no hay datos)
    q->head = 0; //inicio la cabeza no existe -> no hay datos (como es un entero ponemos cero)
    q->capacity = capacity;
    q->tail = capacity - 1;//tail => a capacidad menos 1
    q->array = (struct element*)malloc(q->capacity * sizeof(int));//el array sera de la capacidad por el tamano del dato que manejamos
    
    return q;
}


// To Enqueue an element
int queue_put(queue *q, struct element* x) {
    //primero comprobar si esta lleno
    if(queue_full(q)==1){
	printf("el queue esta lleno");
	return -1;
	}
    q->tail = (q->tail + 1) % q->capacity; //esto nos da en que posicion insertar el elemento en el array
    q->array[q->tail] = *x;
    q->size = q->size + 1;
    return 0;
}


// To Dequeue an element.
struct element* queue_get(queue *q) {
    struct element* element;
    //comprobar si esta vacio
    *element = q->array[q->head];//cogemos el elemento cero de la cola
    q->head = (q->head + 1) % q->capacity;
    q->size = q->size -1;
    
    return element;
}


//To check queue state
int queue_empty(queue *q){
    if(q->size == 0){//si el tamano de size es cero es que no hay elementos
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
    return 0;
}
