#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#define true 1
#define NUM_THREADS 8

char nome[8] = "gabriel";
int M = 20;
int N = 7;
int javalis = 0;

pthread_mutex_t lock_mcald;
sem_t sem_cald_vazio;
sem_t sem_cald_cheio;

void RetiraJavali(int i){
    pthread_mutex_lock(&lock_mcald);
    if(javalis == 0){
        sem_post(&sem_cald_vazio);
        printf("Gaules %c(%d) acordou cozinheiro\n", nome[i], i);
        sem_wait(&sem_cald_cheio);
    }
    javalis--;
    pthread_mutex_unlock(&lock_mcald);
}

void ColocaJavalis(int m){
    sem_wait(&sem_cald_vazio);
    sleep(3);
    javalis+=m;
    sem_post(&sem_cald_cheio);
}

void ComeJavali(int i){
    printf("Gaules %c(%d) comendo\n", nome[i], i);
    sleep(1);
}

void* Gaules(void* idx){
    int i = (int)(long)idx;
    while(true){
        RetiraJavali(i);
        ComeJavali(i);
    }
}

void* Cozinheiro(void* ignore){
    while(true){
        ColocaJavalis(M);
    }
}

int main(void){
    pthread_mutex_init(&lock_mcald, NULL);
    sem_init(&sem_cald_cheio, 0, 0);
    sem_init(&sem_cald_vazio, 0, 1);
    pthread_t thread[NUM_THREADS];
    int i;
    
    pthread_create(&thread[N], NULL, &Cozinheiro, (void*)(intptr_t) N);
    for(i=0; i<N; i++){
        pthread_create(&thread[i], NULL, &Gaules, (void*)(intptr_t) i);
    }

    for(i=0; i<NUM_THREADS; i++)
        pthread_join(thread[i], NULL);
    
    pthread_mutex_destroy(&lock_mcald);
    sem_destroy(&sem_cald_cheio);
    sem_destroy(&sem_cald_vazio);

    return 0;
}