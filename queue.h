#ifndef HEADER_FILE
#define HEADER_FILE


struct element {
  int type; //Machine type
  int time; //Using time
};

typedef struct queue {
	int size;		//current number of elements inside the queue
	int head,tail;		//indices that represente the head and the tail of the queue
	unsigned capacity;	//user-defined parameter: max capacity of the queue
	struct element *array; //the container of the queue: an array to store the elements
}queue;

queue* queue_init (int size);
int queue_destroy (queue *q);
int queue_put (queue *q, struct element* elem);
struct element * queue_get(queue *q);
int queue_empty (queue *q);
int queue_full(queue *q);

#endif
