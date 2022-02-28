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
    char *usrinput = mmap(NULL, 100, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    //printf("\033[0;34m");
    //printf("findstuff$ ");
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
        //read(STDIN_FILENO, usrinput, 12);
        //printf("\033[0;34m");
        //printf("%s ", usrinput);
        //printf("\033[0;m");

        dup2(save_usrinput, STDIN_FILENO);
        read(STDIN_FILENO, usrinput, 100);
        size_t len = strlen(usrinput);
        char *args[4];
        char *cmds = strtok(usrinput, " \n");
        int i = 0;
        while(cmds != NULL){
            if(i == 4){
                fprintf(stderr, "Too many args, stopping \n");
                return -1;
            }
            args[i++] = cmds;
            cmds = strtok(NULL, " \n");
        }
        close(fd[0]);
        wait(0);
    }
    
    munmap(usrinput, 100);
    return 0;
}