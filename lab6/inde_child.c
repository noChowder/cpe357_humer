#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/mman.h>
#include <dirent.h>

void signalhandler1(int sig){
    printf("Not possible!! Good luck \n");
}

int main(){
    signal(SIGTSTP, signalhandler1); // CTRL + Z
    signal(SIGINT, signalhandler1); // CTRL + C
    signal(SIGQUIT, signalhandler1); // CTRL + backslash
    signal(SIGUSR1, signalhandler1);

    int parent_pid = getpid();
    int *childp = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    printf("Parent pid: \t\t%d \n", parent_pid);

    time_t T = time(NULL);
    struct tm *tm = (struct tm *)mmap(NULL, sizeof(struct tm), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);

    DIR *dir;
    
    if(fork() == 0){
        *childp = getpid();

        while(1){
            dir = opendir(".");
            if(!dir){
                return -1;
            }

            printf("\n-----------------------------------------------------------------------------\n");
            printf("Child pid: \t\t%d \n", *childp);

            *tm = *localtime(&T);
            int old_sec = tm->tm_sec;
            printf("Local time is: \t\t%d:%d:%d \n", tm->tm_hour, tm->tm_min, tm->tm_sec);

            sleep(10);

            T = time(NULL);
            *tm = *localtime(&T);
            int timediff = abs(tm->tm_sec - old_sec);
            if(old_sec >= 50){
                int adjusted_sec = tm->tm_sec + 60;
                timediff = abs(adjusted_sec - old_sec);
            }
            printf("Elapsed time: \t\t%d sec\n", timediff);
            printf("Local time is: \t\t%d:%d:%d \n", tm->tm_hour, tm->tm_min, tm->tm_sec);

            struct dirent *entry;
            printf("\ncwd files: \n");
            while((entry = readdir(dir)) != NULL){
                printf("\t\t\t%s \n", entry->d_name);
            }
            closedir(dir);
            printf("-----------------------------------------------------------------------------\n");
        }
    }
    else{
        kill(*childp, SIGTSTP);
        kill(*childp, SIGINT);
        kill(*childp, SIGQUIT);
        kill(*childp, SIGUSR1);

        wait(0);
    }

    munmap(childp, sizeof(int));
    munmap(tm, sizeof(struct tm));
    return 0;
}