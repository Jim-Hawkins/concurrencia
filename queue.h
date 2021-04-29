#ifndef HEADER_FILE
#define HEADER_FILE


struct element {
  int type; //Machine type
  int time; //Using time
};

typedef struct queue {
	// Define the struct yourself
	int size;//el tamano de la cola
	int head,tail; //son los indices de por donde va la cola :o
	unsigned capacity; //capacidad que tendra el array (definida por el user(nosotros))
	struct element *array;
}queue;

queue* queue_init (int size);
int queue_destroy (queue *q);
int queue_put (queue *q, struct element* elem);
struct element * queue_get(queue *q);
int queue_empty (queue *q);
int queue_full(queue *q);

#endif
