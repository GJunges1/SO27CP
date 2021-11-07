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
#include <mqueue.h>
#include <errno.h>

#define true 1

typedef struct messbuf{
    int id;
}messbuf_t;

typedef struct shmbuf{
    sem_t lock;
    sem_t cald_vazio;
    sem_t cald_cheio;
}shmbuf_t;

shmbuf_t *ptr;

mqd_t fd[2];
messbuf_t *ptr_m;

void ColocaJavalis(){
    sem_wait(&(ptr->cald_vazio));
    int i=0,r; // para printar
    r = mq_send(fd[0], (char*) ptr_m, sizeof(messbuf_t), 0);
    if(r < 0){
        perror("Erro no mq_send buffer 1"); return;
    }
    else{
        while(r == 0){
            r = mq_send(fd[0], (void*)ptr_m, sizeof(messbuf_t), 0);
        }
    }

    r = mq_send(fd[1], (char*) ptr_m, sizeof(messbuf_t), 0);
    if(r < 0){
        perror("Erro no mq_send  buffer 2"); return;
    }
    else{
        while(r == 0){
            r = mq_send(fd[1], (void*)ptr_m, sizeof(messbuf_t), 0);
        }
    }

    sem_post(&(ptr->cald_cheio));
}

void Cozinheiro(){
    while(true){
        ColocaJavalis();
    }
}

int main(void){
    int fd_shm;
    fd_shm = shm_open("/sharedmem", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);

    ptr = mmap(NULL, sizeof(shmbuf_t), PROT_READ|PROT_WRITE, MAP_SHARED, fd_shm, 0);

    sem_wait(&(ptr->cald_vazio));

    messbuf_t data;
    ptr_m = &data;

    fd[0] = mq_open("/myqueue1", O_WRONLY|O_NONBLOCK);
    if(fd[0] < 0)
        perror("Erro ao abrir arquivo no produtor");
    fd[1] = mq_open("/myqueue2", O_WRONLY|O_NONBLOCK);
    if(fd[1] < 0)
        perror("Erro ao abrir arquivo no produtor");

    Cozinheiro();
    
    return 0;
}