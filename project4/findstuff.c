#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/times.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>

int main(){
    char usrinput[100];
    //printf("\033[0;34m");
    //printf("findstuff$");
    //printf("\033[0;m");
    int fd[2];
    pipe(fd);
    int save_usrinput = dup(STDIN_FILENO);
    dup2(fd[0], STDIN_FILENO);

    if(fork() == 0){
        printf("\033[0;34m");
        printf("findstuff$ ");
        printf("\033[0;m");
        return 0;
    }
    else{
        close(fd[1]);
        dup2(save_usrinput, STDIN_FILENO);
        read(STDIN_FILENO, usrinput, 100);
        //printf("%s \n", usrinput);
        char *args[4];
        for(int i = 0; i < 4; i++)
            args[i] = (char *)malloc(20);
        char *cmds = strtok(usrinput, " \n");
        int i;
        while(cmds != NULL){
            args[i] = cmds;
            i++;
            cmds = strtok(NULL, " \n");
        }
        if(i > 5){
            fprintf(stderr, "Too many args, stopping \n");
            return -1;
        }
        printf("%s", args[1]);
        close(fd[0]);
        wait(0);
    }

    return 0;
}