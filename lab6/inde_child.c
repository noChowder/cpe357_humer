#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/mman.h>

int main(){
    int parent_pid = getpid();
    int *childp = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    printf("pid before fork: \t%d \n", parent_pid);
    time_t T = time(NULL);

    struct tm *tm = (struct tm *)mmap(NULL, sizeof(struct tm), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    *tm = *localtime(&T);
    printf("Time is: \t\t%d:%d:%d \n", tm->tm_hour, tm->tm_min, tm->tm_sec);

    if(fork() == 0){
        printf("child here\n");
        *childp = getpid();
        printf("child pid: \t\t%d \n", *childp);
        int old_sec = tm->tm_sec;
        sleep(5);
        T = time(NULL);
        *tm = *localtime(&T);
        printf("Time is: \t\t%d:%d:%d \n", tm->tm_hour, tm->tm_min, tm->tm_sec);
        printf("Elapsed time: \t\t%d sec\n", tm->tm_sec-old_sec);
    }
    else{
        printf("parent here \n");
        parent_pid = getpid();
        printf("parent pid: \t\t%d \n", parent_pid);
        wait(0);
    }

    munmap(childp, sizeof(int));
    munmap(tm, sizeof(struct tm));
    return 0;
}