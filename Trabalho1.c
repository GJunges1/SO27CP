#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
    Trabalho 1 - Sistemas Operacionais 2021/2
    Gabriel Junges Baratto
            G
           / \
           A  R
           |  |
           B  I
           |  |
           E  L
*/


int main(){
    pid_t pid_A=0, pid_B=0, pid_R=0, pid_I=0, pid_E=0, pid_L=0;

	printf("proc-G, pid %d, ppid %d, acaba de ser criado\n", getpid(), getppid());//pai G diz oi
    
    pid_A = fork();
    if(pid_A==0){ // filho A entra
        printf("proc-A, pid %d, ppid %d, acaba de ser criado\n", getpid(), getppid());//A (filho de G) é criado

        pid_B=fork();
        if(pid_B==0){
            printf("proc-B, pid %d, ppid %d, acaba de ser criado\n", getpid(), getppid());//B (filho de A) é criado

            pid_E=fork();
            if(pid_E==0){
                printf("proc-E, pid %d, ppid %d, acaba de ser criado\n", getpid(), getppid());//E (filho de B) é criado
                printf("proc-E, pid %d, ppid %d, morreu\n", getpid(), getppid());//E morre
                exit(0);
            }

            waitpid(pid_E,NULL,0);// B aguarda E morrer
            printf("proc-B, pid %d, ppid %d, morreu\n", getpid(), getppid());//B morre
            exit(0);

        }

        waitpid(pid_B,NULL,0);// A aguarda B morrer
        sleep(1);
        printf("proc-A, pid %d, ppid %d, morreu\n", getpid(), getppid());//A morre
        exit(0);
    }

    waitpid(pid_A,NULL,0);//G aguarda A para criar R

    pid_R = fork();
    if(pid_R==0){
    	printf("proc-R, pid %d, ppid %d, acaba de ser criado\n", getpid(), getppid());//R (filho de G) é criado
    	
    	pid_I = fork();
    	if(pid_I==0){

            printf("proc-I, pid %d, ppid %d, acaba de ser criado\n", getpid(), getppid());//L (filho de R) é criado

            pid_L = fork();
            if(pid_L==0){
                printf("proc-L, pid %d, ppid %d, acaba de ser criado\n", getpid(), getppid());//L (filho de I) é criado    

                printf("proc-L, pid %d, ppid %d, morreu\n", getpid(), getppid());//L (filho de I) morre
                exit(0);
            }
            
            waitpid(pid_L,NULL,0);//I aguarda L para morrer
            printf("proc-I, pid %d, ppid %d, morreu\n", getpid(), getppid());//I morre
            exit(0);

    	}

        waitpid(pid_I,NULL,0);//R espera I morrer
    	printf("proc-R, pid %d, ppid %d, morreu\n", getpid(), getppid());//R morre
    	exit(0);
    }
    

    waitpid(pid_A,NULL,0);// pai G espera filho A
    waitpid(pid_R,NULL,0);// pai G espera filho R

    printf("proc-G, pid %d, ppid %d, morreu\n", getpid(), getppid());//pai G morre
	return 0;
}

