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

mqd_t fd;
messbuf_t *ptr_m;

void ColocaJavalis(){
    sem_wait(&(ptr->cald_vazio));
    int i=0,r; // para printar
    r = mq_send(fd, (char*) ptr_m, sizeof(messbuf_t), 0);
    if(r < 0)
        perror("Erro no mq_send");
    else{
        while(r == 0){
            r = mq_send(fd, (void*)ptr_m, sizeof(messbuf_t), 0);
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

    fd = mq_open("/myqueue", O_WRONLY|O_NONBLOCK);
    if(fd < 0)
        perror("Erro ao abrir arquivo no produtor");

    Cozinheiro();
    
    return 0;
}