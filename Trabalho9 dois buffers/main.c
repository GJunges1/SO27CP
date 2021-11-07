#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <wait.h>
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

mqd_t fd[2];
messbuf_t data;
messbuf_t *ptr_m = &data;
shmbuf_t *ptr;

void init_shmbuf(shmbuf_t *ptr){
    sem_init(&(ptr->lock), 1, 1);
    sem_init(&(ptr->cald_cheio), 1, 0);
    sem_init(&(ptr->cald_vazio), 1, 1);
}

int main(){
    int pid[2];
    int fd_shm;

    fd_shm = shm_open("/sharedmem", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);

    ftruncate(fd_shm, sizeof(shmbuf_t));

    ptr = mmap(NULL, sizeof(shmbuf_t), PROT_READ|PROT_WRITE, MAP_SHARED, fd_shm, 0);
    
    init_shmbuf(ptr);

    struct mq_attr attr;

    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(messbuf_t); // = 4 (pois é int)

    int r = mq_unlink("/myqueue1");
    if(r < 0)
        perror("Unlink 1");

    fd[0] = mq_open("/myqueue1", O_CREAT, 0666, &attr);
    if(fd[0] < 0){
        perror("Erro ao abrir/criar arquivo 1 na main"); return 0;
    }
    r = mq_unlink("/myqueue2");
    if(r < 0)
        perror("Unlink 2");

    fd[1] = mq_open("/myqueue2", O_CREAT, 0666, &attr);
    if(fd[1] < 0){
        perror("Erro ao abrir/criar arquivo 2 na main"); return 0;
    }
    
    for(int i=0;i<2;i++)
        pid[i]=0;
    
    pid[0] = fork();
    if(pid[0]==0)
        execl("producer", "producer", NULL);

    pid[1] = fork();
    if(pid[1]==0)
        execl("consumers", "consumers", NULL);

    for(int i=0;i<2;i++)
        waitpid(pid[i],NULL,0);

    sem_destroy(&ptr->lock);
    sem_destroy(&ptr->cald_cheio);
    sem_destroy(&ptr->cald_vazio);
    
    return 0;
}