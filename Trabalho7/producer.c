#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <signal.h>

#define true 1


typedef struct shmbuf{
    int javalis;
    sem_t lock;
    sem_t cald_vazio;
    sem_t cald_cheio;
}shmbuf_t;

int M = 20;
shmbuf_t *ptr;

void ColocaJavalis(int m){
    sem_wait(&(ptr->cald_vazio));
    ptr->javalis+=m;
    sem_post(&(ptr->cald_cheio));
}

void Cozinheiro(){
    while(true){
        ColocaJavalis(M);
    }
}

int main(){
    int fd;

    fd = shm_open("/sharedmem", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);

    ptr = mmap(NULL, sizeof(shmbuf_t), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    sem_wait(&(ptr->cald_vazio));
    Cozinheiro();
    
    return 0;
}