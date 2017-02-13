all:
	gcc -g -Wall client.c -o client -lpthread -lm
	gcc -g -Wall server_sem.c -o server_sem -lpthread -lm
	gcc -g -Wall server_mutex.c -o server_mutex -lpthread -lm
