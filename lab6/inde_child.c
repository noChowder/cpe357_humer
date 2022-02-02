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
    time_t T = time(NULL);

    struct tm tm = *localtime(&T);
    printf("Time is: \t\t%d:%d:%d \n", tm.tm_hour, tm.tm_min, tm.tm_sec);

    if(fork() == 0){
        printf("child here\n");
        pid = getpid();
        printf("child pid: \t\t%d \n", pid);
        int old_sec = tm.tm_sec;
        sleep(5);
        tm = *localtime(&T);
        printf("Time is: \t\t%d:%d:%d \n", tm.tm_hour, tm.tm_min, tm.tm_sec);
        printf("Elapsed time: \t%d sec\n", tm.tm_sec-old_sec);
    }
    else{
        printf("parent here \n");
        pid = getpid();
        printf("parent pid: \t\t%d \n", pid);
        wait(0);
    }

    munmap(p, 4);
    return 0;
}