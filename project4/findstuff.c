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

int finder(char *file){
    return 0;
}

int main(){
    char *usrinput = mmap(NULL, 100, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    int *child_count = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    *child_count = 0;
    int fd[2];
    pipe(fd);
    int save_usrinput = dup(STDIN_FILENO);
    dup2(fd[0], STDIN_FILENO);
    int status;
    int prompt = fork();

    while(1){
    if(prompt == 0){
        close(fd[0]);
        printf("\033[0;34m");
        printf("findstuff$ ");
        printf("\033[0;m");
        close(fd[1]);
        return 0;
    }
    else{
        close(fd[1]);
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

        if( (strcmp(args[0], "find") == 0) ){
            *child_count = *child_count + 1;
            printf("%d \n", *child_count);
            int status;
            int child_pid = fork();
            if(child_pid == 0){
                if(finder(args[1]) != 0){
                    fprintf(stderr, ">nothing found< \n");
                    return -1;
                }
                if(finder(args[1]) == 0){
                    sleep(2);
                    return 0;
                }
                return 0;
            }
            else{
                while(1){
                    sleep(1);
                    int ret = waitpid(child_pid, &status, WNOHANG);
                    if(ret > 0){
                        printf("child %d returned \n", *child_count);
                        *child_count = *child_count - 1;
                        kill(child_pid, SIGKILL);
                        break;
                    }
                }
            }
        }
        close(fd[0]);
        waitpid(prompt, &status, 0);
        kill(prompt, SIGKILL);
        prompt = fork();
    }
    }
    
    munmap(usrinput, 100);
    munmap(child_count, sizeof(int));
    return 0;
}