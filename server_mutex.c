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

char theArray[NUM_STR][STR_LEN];
pthread_mutex_t mutex;
int thread_number = -1;

void *ServerEcho(void *args)
{
	int clientFileDescriptor = (int)args;
	char str[20];
	int read_or_write, row_num;
	
	pthread_mutex_lock(&mutex);
	read(clientFileDescriptor,str,20);
	printf("reading from client:%s",str);
	
	/* Parse the input string from client side */
	sscanf(str, "%d%5d", &read_or_write, &row_num );
	printf("The received command in server side is %d, row: %d\n", read_or_write, row_num );
	
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
	close(clientFileDescriptor);	
	pthread_exit(NULL);
}


int main()
{
	struct sockaddr_in sock_var;
	int serverFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
	int clientFileDescriptor;
	int i;
	int thread_count = STR_LEN;
	pthread_t* thread_handles;
	sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
	sock_var.sin_port=3000;
	sock_var.sin_family=AF_INET;

	
	
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
		
	    while(1)        //loop infinity
	    {

		clientFileDescriptor=accept(serverFileDescriptor,NULL,NULL);                                                              		        //printf("nConnected to client %dn",clientFileDescriptor);
	 	thread_number++;
		printf("The thread number is %d\n", thread_number);
		pthread_create(&thread_handles[thread_number],NULL,ServerEcho,(void *)clientFileDescriptor);		
		

	    }
		
		//pthread_join(thread_handles[thread_number],NULL);
		pthread_mutex_destroy(&mutex);
		close(serverFileDescriptor);
	}
	else{
		printf("n server socket creation failed\n");
	}
	return 0;
}
