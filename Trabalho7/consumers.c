#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

#define true 1
#define NUM_THREADS 7

typedef struct shmbuf{
    int javalis;
    sem_t lock;
    sem_t cald_vazio;
    sem_t cald_cheio;
}shmbuf_t;

shmbuf_t *ptr;
char nome[8] = "gabriel";

void RetiraJavali(int i){
    sem_wait(&ptr->lock);
    if(ptr->javalis == 0){
        sem_post(&ptr->cald_vazio);
        printf("Gaules %c(%d) acordou cozinheiro\n", nome[i], i);
        sem_wait(&ptr->cald_cheio);
    }
    ptr->javalis--;
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

int main(){
    pthread_t thread[NUM_THREADS];
    int i;
    int fd;

    srand(time(NULL));

    fd = shm_open("/sharedmem", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);

    ftruncate(fd, sizeof(shmbuf_t));

    ptr = mmap(NULL, sizeof(shmbuf_t), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    for(i=0; i<NUM_THREADS; i++){
        pthread_create(&thread[i], NULL, &Gaules, (void*)(intptr_t) i);
    }

    for(i=0; i<NUM_THREADS; i++)
        pthread_join(thread[i], NULL);
    
    return 0;
}