#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <semaphore.h>
/*
	Trabalho 5 - Sistemas Operacionais
	Gabriel Junges Baratto
	2021/2
*/

#define NUM_THREADS 3

sem_t sem1, sem2, sem3;
float j=0, h=0, x=0, y=0, z=0, u=0;

//P1: begin A; E; F; end;
void* P1(void* idx){
	printf("begin A\n"); //begin A:
	x=1; y=1; z=2;

	sem_post(&sem1);
	sem_post(&sem1);

	printf("begin E\n"); //begin E:
	y = y + 2;

	sem_post(&sem1);
	sem_post(&sem1);

	sem_wait(&sem2); // F waits D
	sem_wait(&sem3); // F waits H
	printf("begin F\n"); //begin F:
	u = h + j/3;
}
//P2: begin B; C; D; end;
void* P2(void* idx){
	sem_wait(&sem1); // B waits A
	printf("begin B\n"); //begin B:
	x = x * 2;

	printf("begin C\n"); //begin C:
	x = x + 1;

	sem_wait(&sem1); // D waits E
	printf("begin D\n"); //begin D;
	h = y + x;
	sem_post(&sem2);
}
//P3: begin G; H; end;
void* P3(void* idx){
	sem_wait(&sem1); // G waits A
	printf("begin G\n"); //begin G:
	z = z/2;

	sem_wait(&sem1); // H waits E
	printf("begin H\n"); //begin H:
	j = z + y - 4;
	sem_post(&sem3);
}

int main(){
	int i=0;
	pthread_t thread[NUM_THREADS];

	sem_init(&sem1, 0, 0);
	sem_init(&sem2, 0, 0);
	sem_init(&sem3, 0, 0);

	pthread_create(&thread[i], NULL, &P1, (void*)(intptr_t) i++);
	pthread_create(&thread[i], NULL, &P2, (void*)(intptr_t) i++);
	pthread_create(&thread[i], NULL, &P3, (void*)(intptr_t) i++);

	for(i=0;i<NUM_THREADS;i++)
		pthread_join(thread[i], NULL);
	
	printf("u = %f\n",u);
	
	sem_destroy(&sem1);
	sem_destroy(&sem2);
	sem_destroy(&sem3);

	return 0;
}