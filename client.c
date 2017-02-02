#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#include "timer.h"
#include<string.h>

#define NUM_STR 1024
#define STR_LEN 1000

int* seed;
int thread_count;

void* sending_request(void * rank)
{
	struct sockaddr_in sock_var;
	int clientFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
	
	char str_clnt[50],str_ser[50];
	int row_num;
	int rand_num;
	long my_rank = (long) rank;
	
	sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
	sock_var.sin_port=3000;
	sock_var.sin_family=AF_INET;

	if(connect(clientFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
	{
		printf("Connected to server %dn",clientFileDescriptor);
		row_num = rand_r(&seed[my_rank]) % NUM_STR;
		rand_num = rand_r(&seed[my_rank]) % 20;
		
		if (rand_num >= 20) // 5% are write operations, others are reads
		{
			sprintf(str_clnt, "1%4d", row_num );
		}
		else
		{
			sprintf(str_clnt, "0%4d", row_num );
		}	
		write(clientFileDescriptor,str_clnt,50);
		read(clientFileDescriptor,str_ser,50);
		printf("String from Server: %s",str_ser);
	}
	else{
		printf("socket creation failed");
	}	
	close(clientFileDescriptor);
}

int main()
{
	long       thread;  /* Use long in case of a 64-bit system */

	int i;
	pthread_t* t;

	t = malloc( STR_LEN * sizeof(pthread_t));

	/* Intializes random number generators */
	seed = malloc(STR_LEN*sizeof(int));
	for (i = 0; i < STR_LEN; i++)
		seed[i] = i;
	
	for (thread = 0; thread < thread_count; thread++)  
		pthread_create(&t[thread],NULL,sending_request,(void *)thread);
		
    for( thread=0; thread< thread_count; thread++)
		pthread_join(t[thread], NULL);

	free(t);
	pthread_exit(NULL);
	return 0;
}
