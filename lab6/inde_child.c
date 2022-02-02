#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/mman.h>

int main(){
    int *p = (int *)mmap(NULL, 4, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);

    if(fork() == 0){
        printf("child here\n");
        *p = 6;
    }
    else{
        printf("parent here \n");
        *p = 3;
        wait(0);
    }

    printf("%d \n", *p);
    munmap(p, 4);
    return 0;
}