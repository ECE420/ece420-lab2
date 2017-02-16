#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>
#include <string.h>
#include "timer.h"

#define STR_LEN 1000
#define THREAD_COUNT 1000

typedef struct{
    int clientFileDescriptor;
    int thread_number;
}Passin_value;

char** theArray;
sem_t* semaphores;
int thread_number = -1;
int port_num;
int array_size;
double start[THREAD_COUNT],finish[THREAD_COUNT], elapsed[THREAD_COUNT];	
double sum = 0;
Passin_value passin[THREAD_COUNT];
FILE *f;
FILE *f2;

void *ServerEcho(void *args)
{
	int clientFileDescriptor;
	char str[20];
	int thread_id;
	int read_or_write, row_num;
	Passin_value* my_passin;
	char* read_str;
	read_str = malloc( STR_LEN * sizeof(char));
        /* Get the clientFileDescriptor and thread_id from the thread arguments */
	my_passin = (Passin_value*) args;
	clientFileDescriptor = my_passin->clientFileDescriptor;
	thread_id = my_passin->thread_number;
	//printf("The pass in thread id is %d \n", thread_id);	

	read(clientFileDescriptor,str,20);
	//printf("reading from client:%s",str);
        /* Start to measure time here */
	GET_TIME(start[thread_id]);
	/* Parse the input string from client side */
	sscanf(str, "%d%5d", &read_or_write, &row_num );
	read_str = theArray[row_num];
	//printf("The received command in server side is %d, row: %d\n", read_or_write, row_num );
	sem_wait( &semaphores[row_num] );

	if( read_or_write == 1 )  //WRITE
	{
	    sprintf( theArray[row_num], "String %d has been modified by a write request", row_num );
	}
	sem_post( &semaphores[row_num] );
	/* Time measurement for each thread ends here */
	GET_TIME(finish[thread_id]);
	/* Calculate the time difference */
	elapsed[thread_id] = finish[thread_id] - start[thread_id];

	/* Send results back to client */
	if( read_or_write == 0 )  //Read
	{
	    write(clientFileDescriptor,theArray[row_num],50);
	}
	else // Write
	{
	    write(clientFileDescriptor,theArray[row_num],50);
	}

	close(clientFileDescriptor);	
	pthread_exit(NULL);
}


int main(int argc, char* argv[])
{
	struct sockaddr_in sock_var;
	int serverFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
	int clientFileDescriptor;
	int i;

	/* Set thread count to a fix value (1000) and accept the given array size and port number */
	int thread_count = THREAD_COUNT;
     	array_size = atoi(argv[2]);
     	port_num = atoi(argv[1]);	

	pthread_t* thread_handles;
	sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
	sock_var.sin_port=port_num;
	sock_var.sin_family=AF_INET;

	theArray = malloc( array_size * sizeof(char*) );

	/* Initialize the start, finish, elapsed array for timer measurement */
	for(i=0;i<THREAD_COUNT;i++){
	     start[i] = 0;
	     finish[i] = 0;
	     elapsed[i] = 0;
	}	

	/* theArray initialization*/
	for (i = 0; i < array_size; i ++)
	{
	    theArray[i] = malloc( STR_LEN * sizeof(char) );
	    sprintf(theArray[i], "String %i: the initial value", i);
	    //printf("Initial string in theArray[%i] is %s \n\n",i,theArray[i]);
	}		
	
	semaphores = malloc((array_size)*sizeof(sem_t));
	/* Initialize all the semaphores to 1 */
	for (i=0; i<array_size; i++){
	    sem_init(&semaphores[i],0,1);
	}

	/*reserve memory for thread handlers*/
	thread_handles = malloc (thread_count*sizeof(pthread_t)); 

	if(bind(serverFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
	{
	
	    //printf("n server socket has been created");
	    listen(serverFileDescriptor,2000); 
	    /*initialize the server theArray*/

	while(1){
	    
	    while(thread_number < (THREAD_COUNT-1))    //loop THREAD_COUNT number of times (since thread_number starts with -1)
	    {
		/* When the server accepts the connection, add the thread_number and launch a thread */
		clientFileDescriptor=accept(serverFileDescriptor,NULL,NULL);                                                              		        //printf("nConnected to client %dn",clientFileDescriptor);
	 	thread_number++;
		passin[thread_number].clientFileDescriptor = clientFileDescriptor;
		passin[thread_number].thread_number = thread_number;
		//printf("The thread number is %d\n", thread_number);
		pthread_create(&thread_handles[thread_number],NULL,ServerEcho,(void *)&passin[thread_number]);
	    }

	    /* Join all the threads and then calculate the totoal processing time and log necessary data */
	    for (i = 0 ; i < THREAD_COUNT ; i++){
		pthread_join(thread_handles[i],NULL);		
	    }

	    f = fopen("sem_10000.txt","a+");

	    for (i = 0; i < THREAD_COUNT; i++){		
		sum += elapsed[i];		
	     }
	    /* Log the processing time results */
      	    fprintf(f, "%f \n", sum );
	    printf("The server_sem takes %f \n", sum);
	    fclose(f);		
	
	   /* Log the theArray and compare the results for each run */
            f2 = fopen("theArray_output_sem2.txt", "a+");
            for(i=0; i< array_size; i++){
            fprintf(f2, "%s \n", theArray[i]);
            }
            fclose(f2);


     	    /* Reset the start, finish, elapsed array for timer measurement */
	    for (i = 0 ; i < THREAD_COUNT ; i++){
		start[i] = 0;
		finish[i] = 0;
		elapsed[i] = 0;	
	    }
	    /* Reset sum and thread_number */
	    sum = 0;
	    thread_number = -1;
		
	}
	/* Destroy all the semaphores */
	for (i = 0; i<array_size;i++){
		sem_destroy(&semaphores[i]);
	
	}	
    	close(serverFileDescriptor);
	}

	else
	{
	    printf("n server socket creation failed\n");
	}
	return 0;
}
