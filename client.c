#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

//
#include <pthread.h> 
#include <string.h>
#include "timer.h"
#define NUM_STR 1024
#define STR_LEN 1000

#define WRITE 1
#define READ 0
//
char theArray[NUM_STR][STR_LEN];
int* seed;
sem_t* semaphores;
void *Operate(void* rank);  /* Thread function */


int main()
{
	//
	int thread_count = STR_LEN;
	long       thread;  /* Use long in case of a 64-bit system */
	pthread_t* thread_handles; 
	int i;

	if(connect(clientFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
	{	
		/* Initializes random number generators */
		seed = malloc(thread_count*sizeof(int));	
		/* Intializes semaphores*/
	//	semaphores = malloc(thread_count*sizeof(sem_t));
		for (thread = 0, thread < thread_count ; thread++)
		{
	//		sem_int(&semaphores[thread],0,1);
			seed[thread] = thread;
		}	
		
		/* Fill in the initial values for theArray */
		for (i = 0; i < NUM_STR; i ++)
		{
			sprintf(theArray[i], "String %d has been modified by a write request", i);
			printf("client write %s to server\n\n", theArray[i]);
		}
				
		thread_handles = malloc (thread_count*sizeof(pthread_t)); 
		
		for (thread = 0; thread < thread_count; thread++)
			pthread_creat(&thread_handles[thread],NULL,Operate,(void*)thread);
			
		for (thread = 0; thread < thread_count; thread++)
			pthread_join(thread_handles[thread],NULL);
	
		free(thread_handles);
	}
	else{
		printf("socket creation failed");
	}
	close(clientFileDescriptor);
	return 0;
}

void *Operate(void* rand){
	long my_rand = (long) rand;
	// Find a random position in theArray for read or write
	int pos = rand_r(&[my_rand]) % NUM_STR；  // shall we use thread instead of my_rank
	int randNum = rand_r(&seed[my_rand]) % 100;
	/*connet to server*/	
	struct sockaddr_in sock_var;
	int clientFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
	char read_back[STR_LEN];
	
	sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
	sock_var.sin_port=3000;
	sock_var.sin_family=AF_INET;
	printf("Connected to server %dn",clientFileDescriptor);
	printf("thread %d :randNum = %d ,pos = %d\n",my_rand,randNum,pos);
	
	if (randNum >= 95) //95% are write operations, others are read
	{	
		write(clientFileDescriptor,WRITE,1);
		/*send random */
		write(clientFileDescriptor,theArray[pos],STR_LEN);
	}
	else{
		write(clientFileDescriptor,READ,1);
		read(clientFileDescriptor,read_back,STR_LEN);
		printf("%s",read_back);
	}
	
	return NULL
}
