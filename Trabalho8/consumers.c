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
char nome[8] = "gabriel";

void RetiraJavali(int i){
    sem_wait(&ptr->lock);
    int ret = read(fd_pipe[0],p,sizeof(pipebuf_t));
    if(ret == -1){
        sem_post(&ptr->cald_vazio);
        printf("Gaules %c(%d) acordou cozinheiro\n", nome[i], i);
        sem_wait(&ptr->cald_cheio);
        ret = read(fd_pipe[0],p,sizeof(pipebuf_t));
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

int fd_nonblock(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return 0;
    flags |= O_NONBLOCK;
    return fcntl(fd, F_SETFL, flags) != -1;
}

int main(int argc, char **argv){
    pthread_t thread[NUM_THREADS];
    int i;
    int fd_shm;
    fd_pipe[0] = strtol(argv[1],NULL,10);
    fd_pipe[1] = strtol(argv[2],NULL,10);

    srand(time(NULL));

    fd_shm = shm_open("/sharedmem", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);

    ftruncate(fd_shm, sizeof(shmbuf_t));

    ptr = mmap(NULL, sizeof(shmbuf_t), PROT_READ|PROT_WRITE, MAP_SHARED, fd_shm, 0);

    pipebuf_t buf;
    p = &buf;
    fd_nonblock(fd_pipe[0]);

    for(i=0; i<NUM_THREADS; i++){
        pthread_create(&thread[i], NULL, &Gaules, (void*)(intptr_t) i);
    }

    for(i=0; i<NUM_THREADS; i++)
        pthread_join(thread[i], NULL);
    
    return 0;
}