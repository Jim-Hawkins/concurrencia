
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
	//printf("inicia productor\n");	//to debug
	
	for(int i = dp->start; i <= dp->end; i++){
		
		//mover elemento del array de operaciones a la cola
		struct element elem = dp->array[i];
		//printf("product: %d %d \n",elem.type, elem.time); //to debug
		pthread_mutex_lock(&mutex); //pide acceso a la seccion critica

		while(queue_full(q)){ //si la cola esta llena no ejecuta, (hasta buffer no lleno y mutex este libre)
			pthread_cond_wait(&buffer_not_full, &mutex);
		}
		// inicio seccion critica
		//printf("anadir elem\n"); //to debug
		queue_put(q, &elem);

		for(int a=0;a<q->size;a++){
			//printf("en la cola esta: (type)%d (time)%d \n", q->array[a].type,q->array[a].time);
}
		//
		pthread_cond_signal(&buffer_not_empty);// senal de que no esta vacia el buffer (para el consumidor)
		pthread_mutex_unlock(&mutex); //deja la seccion critica
	}
	//to debug
	//printf("%d %d\n",dp->start,dp->end); 
	//printf("hola soy el thread %ld\n", pthread_self());
	//printf("termina productor\n");
	//end debug
	pthread_exit(0); //exit of the thread
}

//consumer must return the computed amount trough pthread_exit
void *consumer(void *data_consumers){

	int * result; //declaramos la variable resultado 
	data_consumer *dc = (data_consumer*) (data_consumers) ;//transformamos el tipo de dato del parametro al tipo adecuado para operar con ello
	//printf("inicia consumidor\n");//to debug
	result = (int *) malloc(sizeof(int));

	struct element  *elem; //variable para traer el dato de la cola

	for(int i = 0; i < dc->num_ops;  i++){
		//entrar a la cola, extraer dato y sumar
		pthread_mutex_lock(&mutex); //reclama entrar en seccion critica

		while(queue_empty(q)){//mientras el buffer esta vacio espera
			pthread_cond_wait(&buffer_not_empty, &mutex); //espera hasta que haya algo en el buffer y tenga el mutex
		}
		//seccion critica
		elem = queue_get(q);
		//printf("consumidor: %d %d \n",elem->type, elem->time); //to debug
		//
		pthread_cond_signal(&buffer_not_full); //senal de que el buffer no esta lleno para productor
		pthread_mutex_unlock(&mutex); 
		
		switch(elem->type){ //realizamos las operaciones
			case 1:
				*result = *result + elem->time;
				//printf("a\n");
				break;
			case 2:
				*result = *result + elem->time * 3;
				//printf("b\n");
				break;
			case 3:
				*result = *result + elem->time * 10;
				//printf("c\n");
				break;
			default: //perror("holiii");//to debug no debe entrar aqui
				break;
		}
	//printf("resultado: %d\n", *result); //to debug
	}

	//printf("sera esto :D\n"); //to debug
	//printf("termina consumidor\n"); //to debug
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
    pthread_cond_init(&buffer_not_full,NULL);
    pthread_cond_init(&buffer_not_empty,NULL);
    pthread_mutex_init(&mutex, NULL);

    int *total; 				//final result
    int id;					//dummy variable 
    int slice;					//amount of tasks that each producer will be given
    int start, end;				//indexes to define where a producer starts and ends in the operations array
    
    FILE * file = fopen(argv[1], "r");	//open the file
    int num_producers = atoi(argv[2]);	//get number of producers
    

    int num_op; 				//number of operations to process
    fscanf(file, "%d\n", &num_op);
    
    int consumer_return;			//return value of the consumer thread
    pthread_t consumer_t;			//consumer thread
    int producer_return[num_producers];	//array of return values of the producers
    pthread_t producers_t[num_producers];	//array of producer threads

    //dinamically create an array of element structures
    operations= (struct element *) malloc(sizeof(struct element) * num_op);
    int count = 0;
    //store the file information in the variable id (number of process) and in the structure (type and time)
    for(int i = 0; i < num_op; i++){
	if (fscanf(file, "%d %d %d\n", &id, &operations[i].type, &operations[i].time)!=3){
		return -4;
		//caso num_ops > ops
		
		}
	}



    ////only for debug
    //printf("type and time\n");
   // for(int i = 0; i < num_op; i++){
    //	printf("%d %d\n", operations[i].type, operations[i].time);
   // }
    ////

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
    	pthread_join(producers_t[p], NULL);
    }
	
    pthread_join(consumer_t, (void**)&total);

    
    printf("Total: %i €.\n", *total);
    pthread_cond_destroy(&buffer_not_full);
    pthread_cond_destroy(&buffer_not_empty);
    pthread_mutex_destroy(&mutex);

    return 0;
}
