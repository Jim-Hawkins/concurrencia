
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


//structure to pass parameters to producers
struct data_producer{
	struct element * array; 
	int start, end;
	queue * q;
};

void *producer(void *data_producer){
	//mover elemento del array de operaciones a la cola
	printf("hola soy el thread %ld\n", pthread_self());
	pthread_exit(NULL);
}

//consumer must return the computed amount trough pthread_exit
void *consumer(void *q){
	int * result;
	result = (int *) malloc(sizeof(int));
	//entrar a la cola, extraer dato y sumar
	*result = 1 + 1;
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
    
    //create consumer thread
    if( (pthread_create(&consumer_t, NULL, consumer, (void*) q)) < 0){
    	perror("Consumer thread error");
    }
    //create producers
    for(int p = 0; p < num_producers; p++){
    	if( (pthread_create(&producers_t[p], NULL, producer, &tasks[p])) < 0){
    		perror("Producer thread error");
        }
    }

    //wait for the ending of the threads
    pthread_join(consumer_t, (void**)&total);
    for(int p = 0; p < num_producers; p++){
    	pthread_join(producers_t[p], NULL);
    }
    printf("Total: %i €.\n", *total);

    return 0;
}
