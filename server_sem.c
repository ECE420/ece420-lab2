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
sem_t* semaphores;
int thread_number = 0;

void *ServerEcho(void *args)
{
	int clientFileDescriptor=(int)args;
	char str[20];
	/*generate random number in range of 1024*/
	int pos = rand_r(NUM_STR);
	
	read(clientFileDescriptor,str,20);
	printf("nreading from client:%s",str);
	/*if read*/
	if (str == 0){
		sem_wait(&semaphores[pos]);
		write(clientFileDescriptor,theArray[pos],STR_LEN);
		sem_post(&semaphores[pos]);
	}
	/*if write*/
	else{
		sem_wait(&semaphores[pos]);
		read(clientFileDescriptor,theArray[pos],STR_LEN);
		sem_post(&semaphores[pos]);
	}
	
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
	pthread_t* thread_handles;
	sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
	sock_var.sin_port=3000;
	sock_var.sin_family=AF_INET;

	
	/* allocate memory for semaphores*/
	semaphores = malloc((thread_count + 1)*sizeof(sem_t));
	/* semaphore for the server --- one server cannot support more than 20 clients at a time*/
	sem_init(&semaphores[thread_count + 1],0,20);
	/* semaphores initialization*/
	for (i=0; i<thread_count; i++){
		sem_init(&semaphores[i],0,1);
	}
	
	/* theArray initial*/
	for (i = 0; i < NUM_STR; i ++)
	{
		sprintf(theArray[i], "“String %i: the initial value", i);
		printf("Initial string in theArray[%i] is %s \n\n",i,theArray[i]);
	}		

	/*reserve memory for thread handlers*/
	thread_handles = malloc (thread_count*sizeof(pthread_t)); 

	if(bind(serverFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
	{
	
		printf("n server socket has been created");
		listen(serverFileDescriptor,2000); 
		/*initialize the server theArray*/
		
		while(1)        //loop infinity
		{
			sem_wait(&semaphores[thread_count + 1]);
			clientFileDescriptor=accept(serverFileDescriptor,NULL,NULL);
			printf("nConnected to client %dn",clientFileDescriptor);
			pthread_create(&thread_handles[thread_number],NULL,ServerEcho,(void *)clientFileDescriptor);		
			pthread_join(thread_handles[thread_number],NULL);
			thread_number++;
			sem_post(&semaphores[thread_count + 1]);
		}
		close(serverFileDescriptor);
	}
	else{
		printf("n server socket creation failed");
	}
	return 0;
}
