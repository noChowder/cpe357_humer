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
        close(fd[0]);
        printf("\033[0;34m");
        //write(fd[1], "findstuff$ ", 12);
        printf("findstuff$ ");
        printf("\033[0;m");
        close(fd[1]);
        return 0;
    }
    else{
        close(fd[1]);
        dup2(save_usrinput, STDIN_FILENO);
        read(STDIN_FILENO, usrinput, 100);
        //printf("%s", usrinput);
        char *args[5];
        //for(int i = 0; i < 4; i++)
        //    args[i] = (char *)malloc(20);
        char *cmds = strtok(usrinput, " ");
        int i = 0;
        while(cmds != NULL){
            if(i == 4){
                fprintf(stderr, "Too many args, stopping \n");
                return -1;
            }
            args[i++] = cmds;
            cmds = strtok(NULL, " ");
        }
        for(i = 0; i < 4; i++){
            //printf("%s \n", args[i]);
        }
        printf("%s \n", args[1]);
        close(fd[0]);
        wait(0);
    }

    return 0;
}