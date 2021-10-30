#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
/*
	Trabalho 4 - Sistemas Operacionais
	Gabriel Junges Baratto
	2021/2
*/

#define NUM_THREADS 5

sem_t sA, sB, sC;

void* tA(void* v){

	int tempo = rand() % 5 + 1; //gerando tempo aleatório entre 1 e 5 segundos

	printf("Iniciando A\n");
	sleep(tempo);

	printf("Processo Finalizado A\n");
	sem_post(&sA); // Up no semáforo A

}
void* tB(void* v){

	int tempo = rand() % 5 + 1; //gerando tempo aleatório entre 1 e 5 segundos

	printf("Iniciando B\n");
	sleep(tempo);

	printf("Processo Finalizado B\n");

	sem_post(&sB); // Up no semáforo B

}
void* tC(void* v){

	int tempo = rand() % 5 + 1; //gerando tempo aleatório entre 1 e 5 segundos

	sem_wait(&sA); // Down no semáforo A (libera a primeira tarefa esperando esse sem)
	sem_wait(&sB); // Down no semáforo B (libera a primeira tarefa esperando esse sem)

	printf("Iniciando C\n");
	sleep(tempo);

	printf("Processo Finalizado C\n");

	sem_post(&sC); // Up no semáforo C
	sem_post(&sC); // Up no semáforo C

}
void* tD(void* v){

	int tempo = rand() % 5 + 1; //gerando tempo aleatório entre 1 e 5 segundos

	sem_wait(&sC); // Down no semáforo C (libera a primeira tarefa esperando esse sem)

	printf("Iniciando D\n");
	sleep(tempo);

	printf("Processo Finalizado D\n");

}
void* tE(void* v){

	int tempo = rand() % 5 + 1; //gerando tempo aleatório entre 1 e 5 segundos

	sem_wait(&sC); // Down no semáforo C (libera a primeira tarefa esperando esse sem)

	printf("Iniciando E\n");
	sleep(tempo);

	printf("Processo Finalizado E\n");

}

int main(void){
	int i=0;
	pthread_t thread[NUM_THREADS];

	//Semente para geração de números aleatórios:
	srand(time(NULL));

	//Criando semáforos:
	sem_init(&sA, 0, 0);
	sem_init(&sB, 0, 0);
	sem_init(&sC, 0, 0);

	//Criando threads:
	pthread_create(&thread[i], NULL, &tA, (void*)(intptr_t) i++);
	pthread_create(&thread[i], NULL, &tB, (void*)(intptr_t) i++);
	pthread_create(&thread[i], NULL, &tC, (void*)(intptr_t) i++);
	pthread_create(&thread[i], NULL, &tD, (void*)(intptr_t) i++);
	pthread_create(&thread[i], NULL, &tE, (void*)(intptr_t) i++);

	//Esperando threads:
	for(i=0;i<NUM_THREADS;i++)
		pthread_join(thread[i],NULL);
	
	//Excluindo semáforos
	sem_destroy(&sA);
	sem_destroy(&sB);
	sem_destroy(&sC);

	exit(0);
}