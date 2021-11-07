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
#define TAM_MAX_PIPE 65536
const int N = (int) TAM_MAX_PIPE/20;
int fd_pipe[2];

typedef struct pipebuf{
    char id[(int) (TAM_MAX_PIPE/20)];
}pipebuf_t;

typedef struct shmbuf{
    sem_t lock;
    sem_t cald_vazio;
    sem_t cald_cheio;
}shmbuf_t;

pipebuf_t *p;
shmbuf_t *ptr;

void ColocaJavalis(){
    sem_wait(&(ptr->cald_vazio));
    int i=1;
    while(write(fd_pipe[1], p, sizeof(pipebuf_t))!=-1){
    
    }
    sem_post(&(ptr->cald_cheio));
}

void Cozinheiro(){
    while(true){
        ColocaJavalis();
    }
}

int fd_nonblock(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return 0;
    flags |= O_NONBLOCK;
    return fcntl(fd, F_SETFL, flags) != -1;
}


int main(int argc, char **argv){
    int fd_shm;
    fd_pipe[0] = strtol(argv[1],NULL,10);
    fd_pipe[1] = strtol(argv[2],NULL,10);
    fd_shm = shm_open("/sharedmem", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);

    ptr = mmap(NULL, sizeof(shmbuf_t), PROT_READ|PROT_WRITE, MAP_SHARED, fd_shm, 0);

    pipebuf_t buf;
    p = &buf;
    fd_nonblock(fd_pipe[1]);

    sem_wait(&(ptr->cald_vazio));
    Cozinheiro();
    
    return 0;
}