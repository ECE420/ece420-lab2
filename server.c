#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>

//
#include <string.h>
#include "timer.h"
#define NUM_STR 1024
#define STR_LEN 1000

char theArray[NUM_STR][STR_LEN];
sem_t* semaphores;


void *ServerEcho(void *args)
{
	int clientFileDescriptor=(int)args;
	
	
	
	
	read(clientFileDescriptor,str,20);
	printf("nreading from client:%s",str);
	write(clientFileDescriptor,str,20);
	printf("nechoing back to client");
	close(clientFileDescriptor);
}


int main()
{
	struct sockaddr_in sock_var;
	int serverFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
	int clientFileDescriptor;
	int i;
	//pthread_t t[20];
	int thread_count = STR_LEN;
	long thread;
	pthread* thread_handles;
	
	sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
	sock_var.sin_port=3000;
	sock_var.sin_family=AF_INET;
	
	/* allocate memory for semaphores*/
	semaphores = malloc((thread_count + 1)*sizeof(sem_t));
	/* semaphore for the server --- one server cannot support more than 20 clients at a time*/
	sem_int(&semaphores[thread_count + 1],0,20);
		
	
	if(bind(serverFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
	{
	
		printf("nsocket has been created");
		listen(serverFileDescriptor,2000); 
		/*initialize the server theArray*/
		for (i = 0; i < NUM_STR; i ++)
		{
			sprintf(theArray[i], "“String %i: the initial value", i);
			printf("Initial string in theArray[%i] is %s \n\n",i,theArray[i]);
		}		
		/*reserve memory for thread handlers*/
		thread_handles = malloc (thread_count*sizeof(pthread_t)); 
		
		while(1)        //loop infinity
		{
			// we may need to put semaphore_init= 20 here in order to prevent supporting more than 20 clients at one time
			sem_wait(&semaphores[thread_count + 1]);
			clientFileDescriptor=accept(serverFileDescriptor,NULL,NULL);
			printf("nConnected to client %dn",clientFileDescriptor);
			pthread_create(&t,NULL,ServerEcho,(void *)clientFileDescriptor);		
			sem_post(&semaphores[thread_count + 1]);
		}
		close(serverFileDescriptor);
	}
	else{
		printf("nsocket creation failed");
	}
	return 0;
}
