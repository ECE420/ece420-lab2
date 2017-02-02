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

//
int* seed;
void *Operate(void* rank);  /* Thread function */

int main()
{
	//
	int thread_count = STR_LEN;
	long       thread;  /* Use long in case of a 64-bit system */
	pthread_t* thread_handles; 

	/* Initializes random number generators */
	seed = malloc(thread_count*sizeof(int));	
	for (thread = 0; thread < thread_count ; thread++)
	{
		seed[thread] = thread;
	}	
				
	thread_handles = malloc (thread_count*sizeof(pthread_t)); 
	
	for (thread = 0; thread < thread_count; thread++)
	{
	    pthread_create(&thread_handles[thread],NULL,Operate,(void*)thread);
	}
	
	for (thread = 0; thread < thread_count; thread++)
	{
	    pthread_join(thread_handles[thread],NULL);
	}

	pthread_exit(NULL);
	free(thread_handles);
	return 0;
}

void *Operate(void* rand){
	long my_rand = (long) rand;
	
	int pos = rand_r(&seed[my_rand]) % NUM_STR;  
	int randNum = rand_r(&seed[my_rand]) % 100;
	/*connet to server*/	
	struct sockaddr_in sock_var;
	int clientFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
	char read_back[STR_LEN];
	char str_clnt[20];
	char str_ser[50];
	int read_or_write;
	
	sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
	sock_var.sin_port=3000;
	sock_var.sin_family=AF_INET;
	if(connect(clientFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
	{	
	//	printf("Connected to server %dn",clientFileDescriptor);
		
		if (randNum >= 95) //5% are write operations, others are write
		{
		    read_or_write = 1;				
		    sprintf(str_clnt, "%d%5d", read_or_write, pos );
		    printf("The rand number is %d and the command send is %s\n", randNum, str_clnt);
		}
		else
		{
		    read_or_write = 0;
		    sprintf(str_clnt, "%d%5d", read_or_write, pos );
		    printf("The rand number is %d and the command send is %s\n", randNum, str_clnt);
		}
		write(clientFileDescriptor,str_clnt,20);
		read(clientFileDescriptor,str_ser,50);
		printf("String from Server: %s\n",str_ser);
	}
	else{
		printf("socket creation failed\n");
	}
	close(clientFileDescriptor);
	return NULL;
}
