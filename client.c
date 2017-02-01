#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

#define NUM_STR 1024

int* seed;
int thread_count;
int clientFileDescriptor

void Usage( char* prog_name )
{
	fprintf(stderr, "usage: %s <number of threads> \n", prog_name );
	exit(0);
}

void* sending_request(void * rank)
{
	struct sockaddr_in sock_var;
	clientFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
	
	char str_clnt[50],str_ser[50];
	int row_num;
	int read_write_probability;
	long my_rank = (long) rank;
	
	if(connect(clientFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
	{
		printf("Connected to server %dn",clientFileDescriptor);
	}
	else{
		printf("socket creation failed");
	}
	
	sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
	sock_var.sin_port=3000;
	sock_var.sin_family=AF_INET;
	
	row_num = rand_r(&seed[my_rank]) % NUM_STR;
	read_write_probability = rand_r(&seed[my_rank]) % 20;
	
	if (randNum >= 20) // 5% are write operations, others are reads
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
	close(clientFileDescriptor);
}

int main()
{
	long       thread;  /* Use long in case of a 64-bit system */
	pthread_t t[thread_count];
	
	/* Intializes random number generators */
	seed = malloc(thread_count*sizeof(int));
	for (i = 0; i < thread_count; i++)
		seed[i] = i;
	
	if( argc != 2 ) Usage(argv[0]);
	thread_count = (int)strtol(argv[1], NULL, 10 );
	if( thread_count <=0 ) Usage(argv[0]);

	for (thread = 0; thread < thread_count; thread++)  
		pthread_create(&t[thread],NULL,sending_request,(void *)thread);
		

	pthread_exit(NULL);
	return 0;
}