#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h> 
#include<string.h>
#include"timer.h"

#define STR_LEN 1000
#define THREAD_COUNT 1000

int thread_count;
int port_num;
int* seed;
int array_size;
void *Operate(void* rank);  /* Thread function */

int main( int argc, char* argv[] )
{     
     	long       thread;  /* Use long in case of a 64-bit system */
     	pthread_t* thread_handles; 
     	int i;

     	/* Set thread count to a fix value (1000) and accept the given array size and port number */
     	thread_count = THREAD_COUNT;
     	array_size = atoi(argv[2]);
     	port_num = atoi(argv[1]);

     	/* Initializes random number generators */
     	seed = malloc(array_size*sizeof(int));	

     	for (i = 0; i < array_size ; i++)
     	{
     	    seed[i] = i;
     	}	
     				
     	thread_handles = malloc (thread_count*sizeof(pthread_t)); 
     	
	/* Create thread_count number of threads, each thread will be either a send or a write request */
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

void *Operate(void* rand)
{	
	long my_rand = (long) rand;
     	/* randomize a position for reading or writing string in the server */
     	int pos = rand_r(&seed[my_rand]) % array_size;
     	int randNum = rand_r(&seed[my_rand]) % 100;
     	/*connet to server*/
     	struct sockaddr_in sock_var;
     	int clientFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
	/* str_clnt includes the read/write command and randomized position which
	   will be sent to server */
     	char str_clnt[20];
	/* str_ser is used to receive the string coming back from the server side */
     	char str_ser[50];
	/* read_or_write = 0 indicates a read request; read_or_write = 1 indicates a write request */
     	int read_or_write;
     	/* Configure the socket */
     	sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
     	sock_var.sin_port=port_num;
     	sock_var.sin_family=AF_INET;

     	if(connect(clientFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
     	{	
     		
     	    if (randNum >= 95) //5% are write operations, others are write
     	    {
                read_or_write = 1;				
     		sprintf(str_clnt, "%d %d", read_or_write, pos );
     	    }

     	    else
     	    {
     		read_or_write = 0;
     	        sprintf(str_clnt, "%d %d", read_or_write, pos );
     	    }
	    /* Send read or write request to server */
     	    write(clientFileDescriptor,str_clnt,20);
	    /* Receive the response which will be stored in str_ser */
     	    read(clientFileDescriptor,str_ser,50);
            //printf("The received string from client is %s \n", str_ser );
     	}
     	else
	{
     	    printf("socket creation failed\n");
     	}
     	close(clientFileDescriptor);
	return NULL;
}
