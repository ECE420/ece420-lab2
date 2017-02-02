#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>

//
#include <string.h>
#include "timer.h"
#define NUM_STR 1024
#define STR_LEN 1000

typedef struct{
    int clientFileDescriptor;
    int thread_number;
}Passin_value;

char theArray[NUM_STR][STR_LEN];
pthread_mutex_t mutex;
int thread_number = -1;
double start[NUM_STR],finish[NUM_STR], elapsed[NUM_STR];	
double sum = 0;

Passin_value passin[NUM_STR];

void *ServerEcho(void *args)
{
	int clientFileDescriptor;
	char str[20];
	int thread_id;
	int read_or_write, row_num;
	Passin_value* my_passin;

	my_passin = (Passin_value*) args;
	clientFileDescriptor = my_passin->clientFileDescriptor;
	thread_id = my_passin->thread_number;
	//printf("The pass in thread id is %d \n", thread_id);
	GET_TIME(start[thread_id]);
	pthread_mutex_lock(&mutex);
	read(clientFileDescriptor,str,20);
	//printf("reading from client:%s",str);
	
	/* Parse the input string from client side */
	sscanf(str, "%d%5d", &read_or_write, &row_num );
	//printf("The received command in server side is %d, row: %d\n", read_or_write, row_num );
	
	if( read_or_write == 0 )  //Read
	{
	    write(clientFileDescriptor,theArray[row_num],50);
	}
	else // Write
	{
	    sprintf( theArray[row_num], "String %d has been modified by a write request", thread_number );
	    write(clientFileDescriptor,theArray[row_num],50);
	}
	pthread_mutex_unlock(&mutex);
	GET_TIME(finish[thread_id]);	
	elapsed[thread_id] = finish[thread_id] - start[thread_id];

	//printf("The start is %f, the finish is %f,elapesed time is %f \n",start[thread_id],finish[thread_id],finish[thread_id] - start[thread_id]);
	close(clientFileDescriptor);	
	pthread_exit(NULL);
}


int main()
{
	struct sockaddr_in sock_var;
	int serverFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
	int clientFileDescriptor;
	int i;
	int thread_count = NUM_STR;
	

	pthread_t* thread_handles;
	sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
	sock_var.sin_port=3000;
	sock_var.sin_family=AF_INET;

	for(i=0;i<NUM_STR;i++){
	     start[i] = 0;
	     finish[i] = 0;
	     elapsed[i] = 0;
	}	

	/* theArray initial*/
	for (i = 0; i < NUM_STR; i ++)
	{
	    sprintf(theArray[i], "String %i: the initial value", i);
	    //printf("Initial string in theArray[%i] is %s \n\n",i,theArray[i]);
	}		
	
	pthread_mutex_init(&mutex, NULL);

	/*reserve memory for thread handlers*/
	thread_handles = malloc (thread_count*sizeof(pthread_t)); 

	if(bind(serverFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
	{
	
	    //printf("n server socket has been created");
	    listen(serverFileDescriptor,2000); 
	    /*initialize the server theArray*/
	
	    
	    while(thread_number < (NUM_STR-1))        //loop infinity
	    {
		
		clientFileDescriptor=accept(serverFileDescriptor,NULL,NULL);                                                              		        //printf("nConnected to client %dn",clientFileDescriptor);
	 	thread_number++;
		passin[thread_number].clientFileDescriptor = clientFileDescriptor;
		passin[thread_number].thread_number = thread_number;
		//printf("The thread number is %d\n", thread_number);
		pthread_create(&thread_handles[thread_number],NULL,ServerEcho,(void *)&passin[thread_number]);		
		

	    }
		
		//pthread_join(thread_handles[thread_number],NULL);
		pthread_mutex_destroy(&mutex);
		close(serverFileDescriptor);
	}
	else{
		printf("n server socket creation failed\n");
	}
	for (i = 0; i < 5; i++){
		printf("Original elapse is %f\n",elapsed[i]);		
	}
	for (i = 0; i < NUM_STR; i++){		
		sum += elapsed[i];
	}
	printf("The server_mutex takes %f \n", sum);
	return 0;
}
