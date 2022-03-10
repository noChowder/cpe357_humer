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

int main(int argc, char *argv[]){
    float time2launch = atof(argv[1]);
    time2launch *= 2;

    while(time2launch > 0){
        for(int i = 0; i < time2launch; i++){
            printf(".");
        }
        time2launch -= 1;
        printf("\n");
        usleep(500000);
    }

    return 0;
}