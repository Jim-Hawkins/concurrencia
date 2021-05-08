
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/stat.h>
#include <pthread.h>
#include "queue.h"
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>


#define NUM_CONSUMERS 1

//mutex y condiciones como variables globales
pthread_mutex_t mutex;
pthread_cond_t buffer_not_full;
pthread_cond_t buffer_not_empty;

//structure to pass parameters to producers
struct data_producer{
	struct element * array; 
	int start, end;
	queue * q;
};

struct data_consumer{
	queue * q;
	int num_ops;
};

void *producer(void *data_producer){
	for(int i = data_producer->start; i < data_producer->end; i++){
		//mover elemento del array de operaciones a la cola
		struct element * elem = data_producer->array[i];
		pthread_mutex_lock(&mutex);
		while(queue_full(data_producer.q)){
			pthread_cond_wait(&buffer_not_full, &mutex);
		}
		//seccion critica
		queue_put(data_producer->q, elem);
		//
		pthread_cond_signal(&buffer_not_empty);
		pthread_mutex_unlock(&mutex);
	}
	printf("hola soy el thread %ld\n", pthread_self());
	pthread_exit(NULL);
}

//consumer must return the computed amount trough pthread_exit
void *consumer(void *data_consumer){
	int * result;
	result = (int *) malloc(sizeof(int));
	struct element * elem;
	for(int i = 0; i < data_consumer->num_ops;  i++){
		//entrar a la cola, extraer dato y sumar
		pthread_mutex_lock(&mutex);
		while(queue_empty(data_producer.q)){
			pthread_cond_wait(&buffer_not_empty, &mutex);
		}
		//seccion critica
		elem = queue_get(q);
		//
		pthread_cond_signal(&buffer_not_full);
		pthread_mutex_unlock(&mutex);
		
		switch(elem->type){
			case 1:
				*result = *result + elem->time;
			case 2:
				*result = *result + elem->time * 3;
			case 3:
				*result = *result + elem->time * 10;
			default: break;
		}
	}
	pthread_exit(result);
}

/**
 * Entry point
 * @param argc
 * @param argv
 * @return
 */
int main (int argc, const char * argv[] ) {

    int *total; 				//final result
    int id;					//dummy variable 
    int slice;					//amount of tasks that each producer will be given
    int start, end;				//indexes to define where a producer starts and ends in the operations array
    
    FILE * file = fopen(argv[1], "r");	//open the file
    int num_producers = atoi(argv[2]);	//get number of producers
    queue * q = queue_init(atoi(argv[3]));	//create a queue using a function defined in queue.h and implemented in queue.c

    int num_op; 				//number of operations to process
    fscanf(file, "%d\n", &num_op);
    
    int consumer_return;			//return value of the consumer thread
    pthread_t consumer_t;			//consumer thread
    int producer_return[num_producers];	//array of return values of the producers
    pthread_t producers_t[num_producers];	//array of producer threads
    
    //dinamically create an array of element structures
    struct element * operations = (struct element *) malloc(sizeof(struct element) * num_op);


    //store the file information in the variable id (number of process) and in the structure (type and time)
    for(int i = 0; i < num_op; i++){
	fscanf(file, "%d %d %d\n", &id, &operations[i].type, &operations[i].time);
	}

    ////only for debug
    printf("type and time\n");
    for(int i = 0; i < num_op; i++){
    	printf("%d %d\n", operations[i].type, operations[i].time);
    }
    ////

    //Assignment of the tasks contained in an array to each producer
    	/* The last thread will carry the remainder of num_op/num_producers, that is, until the end of the array.
    	 * Example for 40 tasks and 7 producers:
    	 *  task array: #####  #####  #####  #####  #####  #####  ##########
    	 *  thread ord: first  second third  fourth fifth  sixth  seventh
    	 */

    //create the array to store the tasks
    struct data_producer tasks[num_producers];
    //define the slice (number of ops) for each thread and initialize the start and end (indexes of the operations array)
    slice = num_op/num_producers;
    start = 0;
    end = slice - 1;
    for(int t = 0; t < num_producers; t++){
        //make sure no task is left behind (see comment block with the example above)
    	if(t == num_producers - 1){
    		end = num_op;
    	}
    	tasks[t].array = operations;
    	tasks[t].start = start;
    	tasks[t].end   = end;
    	tasks[t].q     = q;
    	//update start and end
    	start = start + slice;
    	end   = end   + slice;
    }
    //create producers
    for(int p = 0; p < num_producers; p++){
    	if( (pthread_create(&producers_t[p], NULL, producer, &tasks[p])) < 0){
    		perror("Producer thread error");
        }
    }
    
    //create consumer thread
    struct data_consumer dc;
    dc->q = q;
    dc->num_ops = num_op;
    if( (pthread_create(&consumer_t, NULL, consumer, (void*) dc)) < 0){
    	perror("Consumer thread error");
    }

    //wait for the ending of the threads
    pthread_join(consumer_t, (void**)&total);
    for(int p = 0; p < num_producers; p++){
    	pthread_join(producers_t[p], NULL);
    }
    printf("Total: %i €.\n", *total);

    return 0;
}
