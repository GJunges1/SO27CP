#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <mqueue.h>
#include <errno.h>

#define true 1
#define NUM_THREADS 7

typedef struct messbuf{
    int id;
}messbuf_t;

typedef struct shmbuf{
    sem_t lock;
    sem_t cald_vazio;
    sem_t cald_cheio;
}shmbuf_t;

shmbuf_t *ptr;
char nome[8] = "gabriel";

mqd_t fd[2];
messbuf_t data;
messbuf_t *ptr_m;

void RetiraJavali(int i){
    sem_wait(&ptr->lock);

    int r = mq_receive(fd[0], (void*)ptr_m, sizeof(messbuf_t), 0);
    if(r < 0)
        r = mq_receive(fd[1], (void*)ptr_m, sizeof(messbuf_t), 0);
    if(r < 0){
        sem_post(&ptr->cald_vazio);
        printf("Gaules %c(%d) acordou cozinheiro\n", nome[i], i);
        sem_wait(&ptr->cald_cheio);

        r = mq_receive(fd[0], (void*)ptr_m, sizeof(messbuf_t), 0);
        if(r < 0)
            perror("Erro no receive 2");
    }
    sem_post(&ptr->lock);
}

void ComeJavali(int i){

    unsigned int tempo = rand() % 3 + 1;

    printf("Gaules %c(%d) comendo\n", nome[i], i);
    sleep(tempo);
}

void* Gaules(void* idx){
    int i = (int)(long)idx;
    while(true){
        RetiraJavali(i);
        ComeJavali(i);
    }
}

int main(void){
    pthread_t thread[NUM_THREADS];
    int i;
    int fd_shm;

    srand(time(NULL));

    fd_shm = shm_open("/sharedmem", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);

    ftruncate(fd_shm, sizeof(shmbuf_t));

    ptr = mmap(NULL, sizeof(shmbuf_t), PROT_READ|PROT_WRITE, MAP_SHARED, fd_shm, 0);
    
    ptr_m = &data;

    fd[0] = mq_open("/myqueue1", O_RDONLY|O_NONBLOCK);
    if(fd[0] < 0)
        perror("Erro ao abrir arquivo 1 em C");
    fd[1] = mq_open("/myqueue2", O_RDONLY|O_NONBLOCK);
    if(fd[1] < 0)
        perror("Erro ao abrir arquivo 2 em C");

    for(i=0; i<NUM_THREADS; i++){
        pthread_create(&thread[i], NULL, &Gaules, (void*)(intptr_t) i);
    }

    for(i=0; i<NUM_THREADS; i++)
        pthread_join(thread[i], NULL);
    
    return 0;
}