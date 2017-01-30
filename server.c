#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>

#define NUM_STR 1024
#define STR_LEN 1000

char theArray[NUM_STR][STR_LEN];
static int thread_id = -1;
static int thread_count;
sem_t* semaphores;

void Usage( char* prog_name )
{
	fprintf(stderr, "usage: %s <number of threads> \n", prog_name );
	exit(0);
}

void *ServerEcho(void *args)
{
	int clientFileDescriptor = (int)args;
	char str[50];
	int read_or_write, row_num;
	
	sem_wait( &semaphores[thread_id] );
	read(clientFileDescriptor,str,50);
	printf("nreading from client:%s",str);
	
	/* Parse the input string from client side */
	sscanf(str, "%d%4d", &read_or_write, &row_num );
	printf("The received command in server side is %d, row: %d", read_or_write, row_num );
	
	if( read_or_write == 0 )  //Read
	{
		write(clientFileDescriptor,theArray[row_num],50);
	}
	else // Write
	{
		sprintf( theArray[row_num], "String %d has been modified by a write request", row_num );
		write(clientFileDescriptor,theArray[row_num],50);
	}
	close(clientFileDescriptor);
	sem_post( &semaphores[thread_id] );
}


int main()
{
	struct sockaddr_in sock_var;
	int serverFileDescriptor=socket(AF_INET,SOCK_STREAM,0);
    int clientFileDescriptor;
    int count = 0;
	int i;
	pthread_t t[thread_count];
	
	if( argc != 2 ) Usage(argv[0]);
	thread_count = (int)strtol(argv[1], NULL, 10 );
	if( thread_count <=0 ) Usage(argv[0]);
		
		/* Fill in the initial values for theArray */
	for (i = 0; i < NUM_STR; i ++)
	{
		sprintf(theArray[i], "String %d: the initial value", i);
		printf("%s\n\n", theArray[i]);
	}
	
	semaphores = malloc( thread_count * sizeof(sem_t) );
	
    for( i=0; i<thread_count; i++ )
	{
		sem_init( &semaphores[i], 0, 1 );
	}
	
	sock_var.sin_addr.s_addr=inet_addr("127.0.0.1");
	sock_var.sin_port=3000;
	sock_var.sin_family=AF_INET;
	if(bind(serverFileDescriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
	{
		printf("nsocket has been created");
		listen(serverFileDescriptor,2000); 
		while(1)        //loop infinity
		{
			clientFileDescriptor=accept(serverFileDescriptor,NULL,NULL);
			
			thread_id++;
			
			printf("nConnected to client %d \n",clientFileDescriptor);
			pthread_create(&t[thread_id],NULL,ServerEcho,(void *)clientFileDescriptor);
		}
		
		for( i=0; i<thread_count; i++ )
		{
			sem_destory( &semaphores[i] );
		}
		close(serverFileDescriptor);
		free(semaphores);
	}
	else{
		printf("nsocket creation failed");
	}
	pthread_exit(NULL);
	return 0;
}