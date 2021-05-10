
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
queue * q ;	//create a queue using a function defined in queue.h and implemented in queue.c de forma global,
		//para trabajar con la cola de forma global en todas las funciones.

struct element * operations ; //declaramos de forma global la estrucutra element sobre operations

//mutex y condiciones como variables globales
pthread_mutex_t mutex;
pthread_cond_t buffer_not_full;
pthread_cond_t buffer_not_empty;

//structure to pass parameters to producers:

typedef struct {
	struct element * array; 
	int start, end;
	
}data_producer;

typedef struct {
	int num_ops;
} data_consumer;

void *producer(void *data_producers){
	
	data_producer *dp = (data_producer*) (data_producers) ; //transformamos el tipo de dato del parametro al tipo adecuado para operar con ello

	
	for(int i = dp->start; i <= dp->end; i++){
		
		//mover elemento del array de operaciones a la cola
		struct element elem = dp->array[i];

		if(pthread_mutex_lock(&mutex)!=0){//pide acceso a la seccion critica

			perror("Error mutex_lock: ");
			exit(-1);
			} 

		while(queue_full(q)){ //si la cola esta llena no ejecuta, (hasta buffer no lleno y mutex este libre)
			if(pthread_cond_wait(&buffer_not_full, &mutex)!= 0){
				perror("Error wait: ");
				exit(-1);		
			}
		}
		// inicio seccion critica

		queue_put(q, &elem);

		
		if(pthread_cond_signal(&buffer_not_empty)!=0){ // senal de que no esta vacia el buffer (para el consumidor)
			perror("Error Cond_Signal: ");
			exit(-1);
		}
		if(pthread_mutex_unlock(&mutex)!=0){ //deja la seccion critica
			perror("Eror Mutex_Error: ");
			exit(-1);
		}
	}
	
	pthread_exit(0); //exit of the thread
}

//consumer must return the computed amount trough pthread_exit
void *consumer(void *data_consumers){

	int * result; //declaramos la variable resultado 
	data_consumer *dc = (data_consumer*) (data_consumers) ;//transformamos el tipo de dato del parametro al tipo adecuado para operar con ello

	result = (int *) malloc(sizeof(int));

	struct element  *elem; //variable para traer el dato de la cola

	for(int i = 0; i < dc->num_ops;  i++){
		//entrar a la cola, extraer dato y sumar
		if(pthread_mutex_lock(&mutex)!= 0){ //reclama entrar en seccion critica
			perror("Error Mutex_lock: ");
			exit(-1);
		}

		while(queue_empty(q)){//mientras el buffer esta vacio espera
			if(pthread_cond_wait(&buffer_not_empty, &mutex)!=0){ //espera hasta que haya algo en el buffer y tenga el mutex
				perror("Error Cond_Wait: ");
				exit(-1);		
			}
		}
		//seccion critica
		elem = queue_get(q);
		
		//
		if(pthread_cond_signal(&buffer_not_full)!=0){ //senal de que el buffer no esta lleno para productor
			perror("Error Cond_Signal: ");
			exit(-1);	
		}
		
		if(pthread_mutex_unlock(&mutex)!=0){
			perror("Error mutex_unlock");
			exit(-1);	
		} 
		
		switch(elem->type){ //realizamos las operaciones
			case 1:
				*result = *result + elem->time;
				break;
			case 2:
				*result = *result + elem->time * 3;
				break;
			case 3:
				*result = *result + elem->time * 10;
				break;
			default: 
				break;
		}
	
	}

	
	pthread_exit(result); //cierra hilo y retorna la varible result
}

/**
 * Entry point
 * @param argc
 * @param argv
 * @return
 */
int main (int argc, const char * argv[] ) { 
	
    if(argc != 4){
	//mas argumentos entrados
	return -1;
	}

    if(atoi(argv[3])<=0){
	//si longitud de la cola es cero retornamos error
	return -2;	
	}

    if(atoi(argv[2])<=0){
	//caso de que los productores sean cero negativos	
	return -3;
	}

    q= queue_init(atoi(argv[3])); //inicializamos la cola (se generan todas sus caracteristicas)
    
    //Creamos todas las condicionales y el mutex
    if(pthread_cond_init(&buffer_not_full,NULL)!=0){
		perror("Error cond_init: ");
		exit(-1);
	}

    if(pthread_cond_init(&buffer_not_empty,NULL)!=0){
		perror("Error cond_init: ");
		exit(-1);
	}

    if(pthread_mutex_init(&mutex, NULL)!=0){
		perror("Error mutex_init: ");
		exit(-1);
	}

    int *total; 				//final result
    int id;					//dummy variable 
    int slice;					//amount of tasks that each producer will be given
    int start, end;				//indexes to define where a producer starts and ends in the operations array
    
    FILE * file = fopen(argv[1], "r");	//open the file
    int num_producers = atoi(argv[2]);	//get number of producers
    

    int num_op; 				//number of operations to process
    if(fscanf(file, "%d\n", &num_op)!=1){
		//no hay num de ops		
	perror("Error: ");
	exit(-1);	
	}
    

    pthread_t consumer_t;			//consumer thread

    pthread_t producers_t[num_producers];	//array of producer threads

    //dinamically create an array of element structures
    operations= (struct element *) malloc(sizeof(struct element) * num_op);
    int count = 0;

    //store the file information in the variable id (number of process) and in the structure (type and time)
    for(int i = 0; i < num_op; i++){
	if(fscanf(file, "%d %d %d\n", &id, &operations[i].type, &operations[i].time)!=3){
		perror("Error: ");
		exit(-1);
	}
		
		//caso num_ops > ops
		
		
	}


    //Assignment of the tasks contained in an array to each producer
    	/* The last thread will carry the remainder of num_op/num_producers, that is, until the end of the array.
    	 * Example for 40 tasks and 7 producers:
    	 *  task array: #####  #####  #####  #####  #####  #####  ##########
    	 *  thread ord: first  second third  fourth fifth  sixth  seventh
    	 */

    //create the array to store the tasks
    data_producer tasks[num_producers];

    //define the slice (number of ops) for each thread and initialize the start and end (indexes of the operations array)
    slice = num_op/num_producers;
    start = 0;
    end = slice - 1;

    for(int t = 0; t < num_producers; t++){
        //make sure no task is left behind (see comment block with the example above)
    	if(t == num_producers - 1){
    		end = num_op -1;
    	}
    	tasks[t].array = operations;
    	tasks[t].start = start;
    	tasks[t].end   = end;

    	//update start and end
    	start = start + slice;
    	end   = end   + slice;
    }

    //create producers
    for(int p = 0; p < num_producers; p++){
    	if( (pthread_create(&producers_t[p], NULL, (void*)producer, &tasks[p])) < 0){
    		perror("Producer thread error");
		exit(-1);
        }
    }
    
    //create consumer thread
    data_consumer dc;

    dc.num_ops = num_op;
    if( (pthread_create(&consumer_t, NULL, (void*)consumer, (void*) &dc)) < 0){
    	perror("Consumer thread error");
    }

    //wait for the ending of the threads
    for(int p = 0; p < num_producers; p++){
    	if(pthread_join(producers_t[p], NULL)!=0){
		perror("Error Pthread_join: ");
		exit(-1);
	}
    }
	
    if(pthread_join(consumer_t, (void**)&total)!=0){
		perror("Error Pthread_join: ");
		exit(-1);
	}

    if(fclose(file)!=0){
	perror("Error close: ");
	exit(-1);
	}

    printf("Total: %i €.\n", *total);

    if(pthread_cond_destroy(&buffer_not_full)!=0){
	perror("Error Cond_Destroy: ");
	exit(-1);
	}

    if(pthread_cond_destroy(&buffer_not_empty)!=0){
	perror("Error Cond_Destroy: ");
	exit(-1);
	}

    if(pthread_mutex_destroy(&mutex)!=0){
	perror("Error Mutex_Destroy: ");
	exit(-1);
	}

    queue_destroy(q); //destruimos la cola
    free(operations); //liberamos array operations

    return 0;
}
