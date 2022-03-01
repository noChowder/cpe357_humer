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
    int fd[2];
    pipe(fd);

    int save_stdin = dup(STDIN_FILENO);
    dup2(fd[0], STDIN_FILENO);

    char text[100];
    if(fork() == 0){
        close(fd[0]);
        write(fd[1], "findstuff$ \n", 20);
        close(fd[1]);
        return 0;
    }
    else{
        close(fd[1]);
        read(STDIN_FILENO, text, 20);
        if((strlen(text) > 0) && (text[strlen(text) - 1] == '\n')){
            text[strlen(text) - 1] = '\0';
        }
        printf("\033[0;34m");
        printf("%s", text);
        printf("\033[0;m");

        dup2(save_stdin, STDIN_FILENO);
        read(STDIN_FILENO, text, 20);
        //printf("%s \n", text);
        close(fd[0]);
        wait(0);
    } text

    return 0;
}