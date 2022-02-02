#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/mman.h>

int main(){
    int *p = (int *)mmap(NULL, 4, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    int pid = getpid();
    printf("pid before fork: \t%d \n", pid);

    if(fork() == 0){
        printf("child here\n");
        pid = getpid();
        printf("child pid: \t\t%d \n", pid);
        *p = 6;
    }
    else{
        printf("parent here \n");
        pid = getpid();
        printf("parent pid: \t\t%d \n", pid);
        *p = 3;
        wait(0);
    }

    printf("%d \n", *p);
    munmap(p, 4);
    return 0;
}