#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <wait.h>

#define true 1


typedef struct shmbuf{
    int javalis;
    sem_t lock;
    sem_t cald_vazio;
    sem_t cald_cheio;
}shmbuf_t;

shmbuf_t *ptr;

void init_shmbuf(shmbuf_t *ptr){
    ptr->javalis = 0;
    sem_init(&(ptr->lock), 1, 1);
    sem_init(&(ptr->cald_cheio), 1, 0);
    sem_init(&(ptr->cald_vazio), 1, 1);
}

int main(){
    int fd;
    int pid[2];
    char* nome = "gabriel";

    fd = shm_open("/sharedmem", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);

    ftruncate(fd, sizeof(shmbuf_t));

    ptr = mmap(NULL, sizeof(shmbuf_t), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    
    init_shmbuf(ptr);
    
    for(int i=0;i<2;i++)
        pid[i]=0;
    
    pid[0] = fork();
    if(pid[0]==0)
        execl("producer","producer",NULL);
    
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