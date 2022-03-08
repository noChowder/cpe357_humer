#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

int main(int argc, char *argv[]){
    char *args[4];
    args[0] = malloc(100);
    args[1] = malloc(100);
    args[2] = malloc(100);
    args[3] = malloc(100);
    char *par_id = malloc(100);
    char *par_count = malloc(100);
    sprintf(par_count, "%d", atoi(argv[2]));

    time_t T = time(NULL);
    struct tm *tm = (struct tm *)mmap(NULL, sizeof(struct tm), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    *tm = *localtime(&T);

    {
        /* data */
    };
    

    for(int i = 0; i < atoi(par_count); i++){
        sprintf(par_id, "%d", i);
        //printf("%s \n", par_id);
        //printf("%d \n", atoi(par_count));
        strcpy(args[0], argv[1]);
        strcpy(args[1], par_id);
        strcpy(args[2], par_count);
        args[3] = NULL;
        //printf("%s \n", args[0]);
        //printf("%s \n", args[1]);
        //printf("%s \n", args[2]);
        //printf("%s \n", args[3]);
        if(fork() == 0){
            //printf("here \n");
            execv("./par", args);
            //printf("here 2\n");
        }
        else{
            //printf("here3 \n");
        }
    }
    for(int i = 0; i < atoi(par_count); i++){
        wait(0);
    }

    // for(int i = 0; i < atoi(par_count); i++){
    //     //printf("free here %d\n", i);
    //     free(args[i]);
    // }

    int min_diff = 0;
    int old_min = tm->tm_min;
    int sec_diff = 0;
    int old_sec = tm->tm_sec;
    //printf("Time elapsed: %d:%d \n", old_min, old_sec);
    T = time(NULL);
    *tm = *localtime(&T);
    min_diff = abs(tm->tm_min - old_min);
    sec_diff = abs(tm->tm_sec - old_sec);
    printf("Time elapsed: %d min & %d sec \n", min_diff, sec_diff);

    free(args[0]);
    free(args[1]);
    free(args[2]);
    free(args[3]);
    free(par_id);
    free(par_count);
    munmap(tm, sizeof(struct tm));
    return 0;
}