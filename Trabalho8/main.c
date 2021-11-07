#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <wait.h>

#define true 1


#define TAM_MAX_PIPE 65536
const int N = (int) TAM_MAX_PIPE/20;

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

void init_shmbuf(shmbuf_t *ptr){
    sem_init(&(ptr->lock), 1, 1);
    sem_init(&(ptr->cald_cheio), 1, 0);
    sem_init(&(ptr->cald_vazio), 1, 1);
}

int fd_pipe[2];

int main(){
    int fd_shm;
    int pid[2];
    char* nome = "gabriel";

    fd_shm = shm_open("/sharedmem", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);

    ftruncate(fd_shm, sizeof(shmbuf_t));

    ptr = mmap(NULL, sizeof(shmbuf_t), PROT_READ|PROT_WRITE, MAP_SHARED, fd_shm, 0);
    
    init_shmbuf(ptr);
    
    pipebuf_t buf;
    p = &buf;
    pipe(fd_pipe);

    char str1[20], str2[20];
    sprintf(str1,"%d",fd_pipe[0]);
    sprintf(str2,"%d",fd_pipe[1]);
    char *argv[] = {"producer",
                          str1,
                          str2,
                          NULL};

    for(int i=0;i<2;i++)
        pid[i]=0;
    
    pid[0] = fork();
    if(pid[0]==0)
        execv("producer", argv);

    argv[0] = "consumers";
    
    pid[1] = fork();
    if(pid[1]==0)
        execv("consumers", argv);

    for(int i=0;i<2;i++)
        waitpid(pid[i],NULL,0);

    sem_destroy(&ptr->lock);
    sem_destroy(&ptr->cald_cheio);
    sem_destroy(&ptr->cald_vazio);
    
    return 0;
}