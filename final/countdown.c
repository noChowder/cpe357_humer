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
#include <sys/time.h>

#define INTERVAL 500

float time2launch = 0;

void alarm_handler(int alarmsig){
    for(int i = 0; i < time2launch; i++){
        printf(".");
    }
    printf("\n");
    fflush(stdout);
    // printf("alarm sent \n");
}

int main(int argc, char *argv[]){
    signal(SIGALRM, alarm_handler);
    time2launch = atof(argv[1]);
    time2launch *= 2;
    
    struct itimerval alarm;
    alarm.it_value.tv_sec = 0;
    alarm.it_value.tv_usec = INTERVAL * 1000;
    alarm.it_interval = alarm.it_value;
    while(time2launch > 0){
        if(setitimer(ITIMER_REAL, &alarm, NULL) != 0){
            fprintf(stderr, "Error: setitimer() \n");
            return -1;
        }
        pause();
        time2launch -= 1;
    }
    //usleep(INTERVAL*1000);

    if(argc > 2){
    int cmdlines = argc - 1;
    char *args[cmdlines];
    for(int i = 0; i < cmdlines-1; i++){
        args[i] = malloc(100);
        strcpy(args[i], argv[i+2]);
    }
    args[cmdlines] = NULL;
    // printf("here\n");
    if(cmdlines > 0){
        if(fork() == 0){
            execv(args[0], args);
        }
    }
    // else{
    //     wait(0);
    // }
    wait(0);

    for(int i = 0; i < cmdlines; i++){
        // printf("%d", i);
        free(args[i]);
    }
    }
    return 0;
}