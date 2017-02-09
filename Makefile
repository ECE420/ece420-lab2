all:
	gcc client.c -o client -lpthread
	gcc server_sem.c -o server_sem -lpthread
	gcc server_mutex.c -o server_mutex -lpthread
