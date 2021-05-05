
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


    int total = 0; 
    
    FILE *file;
    file = fopen(argv[1],"r"); //abrimos fichero

    int NUM_PRODUC = atoi(argv[2]); //num de productores que habra

    queue * q = queue_init(atoi(argv[3])); //declaramos la variable cola

    int num_op; //num de operaciones a realizar

    fscanf(file,"%d\n",&num_op);

    printf("%d\n",num_op);

    struct element * ops = (struct element *) malloc(sizeof(struct element)*num_op);

    for(int i = 0;i<num_op;i++){

	int id;
	
	fscanf(file,"%d %d %d\n",&id,&ops[i].type,&ops[i].time);
	
	printf("%d %d %d\n",id, ops[i].type,ops[i].time);


	//printf("%d %d %d\n",id,elem.type,elem.time);	

	}
    
    


    printf("Total: %i €.\n", total);

    return 0;
}
