
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



/**
 * Entry point
 * @param argc
 * @param argv
 * @return
 */
int main (int argc, const char * argv[] ) {
    int total = 0; 					//final result
    int id;						//dummy variable 
    
    FILE * file = fopen(argv[1],"r"); 		//open the file
    int NUM_PRODUC = atoi(argv[2]); 			//number of producers
    queue * q = queue_init(atoi(argv[3])); 		//create a queue using a function defined in queue.h

    int num_op; 					//number of operations to process
    fscanf(file, "%d\n", &num_op);

    //printf("%d\n",num_op);
    
    //dinamically create an array of element structures
    struct element * ops = (struct element *) malloc(sizeof(struct element) * num_op);


    for(int i = 0; i < num_op; i++){
    	//store the read information in the variable id and in the structure
	fscanf(file, "%d %d %d\n",&id, &ops[i].type, &ops[i].time);
	
	//printf("%d %d %d\n",id, ops[i].type,ops[i].time);


	//printf("%d %d %d\n",id,elem.type,elem.time);	

	}
    
    


    printf("Total: %i €.\n", total);

    return 0;
}
