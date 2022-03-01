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
#include <dirent.h>

typedef struct Processes{
    int pids[10];
    char *tasks[10];
    int numProcesses;
}Processes;

int sub_finder(char *file, struct dirent *entry, char *path, int match){
    char tempPath[1000];
    DIR *dir;
    dir = opendir(path);
    if(!dir){
        printf("%s \n", path);
        perror("opendir() failed");
        return -1;
    }
    struct dirent *sentry;
    //int match = 0;
    while((sentry = readdir(dir)) != NULL){
        //printf("%s \n", sentry->d_name);
        if(sentry->d_type == 4 && strcmp(sentry->d_name, ".") != 0 && strcmp(sentry->d_name, "..") != 0){
            //printf("%s \n", file);
            strcpy(tempPath, path);
            strcat(tempPath, "/");
            strcat(tempPath, sentry->d_name);
            //printf("here: %s \n", tempPath);
            match = sub_finder(file, sentry, tempPath, match);
        }
        if( (strcmp(file, sentry->d_name) == 0) ){
            //printf("here \n");
            printf("%s/%s \n", path, sentry->d_name);
            match = 1;
        }
    }
    closedir(dir);
    return match;
}

int finder(char *file, char *flag){
    char path[1000];
    getcwd(path, 1000);
    DIR *dir;
    int match = 0;
    struct dirent *entry;
    
    /* check subdirectories */
    printf("Matching file paths: \n");
    if( (strcmp(flag, "-s") == 0) ){
        dir = opendir(".");
        if(!dir){
            perror("opendir() failed");
            return -1;
        }
        while((entry = readdir(dir)) != NULL){
            //printf("%d \t", entry->d_type);
            if(entry->d_type == 4 && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
                match = sub_finder(file, entry, path, match);
            }
        }
        closedir(dir);
    }
    else{
        //printf("here \n");
        dir = opendir(".");
        if(!dir){
            perror("opendir() failed");
            return -1;
        }
        while((entry = readdir(dir)) != NULL){
            if( (strcmp(file, entry->d_name) == 0) ){
                printf("%s/%s \n", path, entry->d_name);
                match = 1;
            }
        }
        closedir(dir);
    }

    if(match != 1){
        return -1;
    }
    putchar('\n');

    return 0;
}

int main(){
    char *usrinput = mmap(NULL, 100, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    Processes *p = mmap(NULL, sizeof(Processes), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    p->numProcesses = 0;

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
        for(int i = 0; i < 4; i++){
            args[i] = malloc(20 * sizeof(char));
        }
        char *cmds = strtok(usrinput, " \n");
        int i = 0;
        while(cmds != NULL){
            if(i == 4){
                fprintf(stderr, "Too many args, stopping \n");
                return -1;
            }
            strcpy(args[i++], cmds);
            cmds = strtok(NULL, " \n");
        }

        if( (strcmp(args[0], "find") == 0) ){
            p->numProcesses+=1;
            if(p->numProcesses > 10){
                fprintf(stderr, "Too many processes, please wait \n");
                break;
            }
            int pidSerial = p->numProcesses;
            printf("%d \n", p->numProcesses);
            int status;
            int child_pid = fork();
            p->pids[pidSerial-1] = child_pid;
            p->tasks[pidSerial-1] = args[1];
            //printf("Child %d is finding %s \n", p->pidSerial, p->tasks[p->pidSerial-1]);
            if(child_pid == 0){
                /* check 2nd arg for file or string */
                // if 2nd arg in quotes use diff function
                // if 2nd arg not in quotes use finder
                int len = strlen(args[1]);
                //printf("last char:  %s \n", args[1]+len-1);
                //printf("first char: %c \n", *args[1]);
                if( (strcmp(args[1]+len-1, "\"")) == 0 && (*args[1] == '\"') ){
                    printf("string \n");
                    // if arg3 is -f
                    // if arg4 is -s
                    // if arg3 is -f and arg4 is -s
                    return 0;
                }
                else{
                    //printf("%s \n", args[3]);
                    if(strcmp(args[2], "-s") == 0){
                        if(finder(args[1], args[2]) != 0){
                            fprintf(stderr, ">nothing found< \n\n");
                            return -1;
                        }
                    }
                    else if(strcmp(args[2], "-s") != 0){
                        //printf("this \n");
                        if(finder(args[1], args[2]) != 0){
                            fprintf(stderr, ">nothing found< \n\n");
                            return -1;
                        }
                    }
                    else{
                        fprintf(stderr, "Invalid input \n");
                        return -1;
                    }
                    return 0;
                }
            }
            else{
                while(1){
                    //sleep(1);
                    int ret = waitpid(child_pid, &status, WNOHANG);
                    if(ret > 0){
                        printf("child %d returned \n", p->numProcesses);
                        //p->numProcesses-=1;
                        break;
                    }
                }
                kill(child_pid, SIGKILL);
                //return 0;
            }
        }
        
        for(int i = 0; i < 4; i++){
            //printf("%s \n", args[i]);
            free(args[i]);
        }
        
        close(fd[0]);
        waitpid(prompt, &status, 0);
        kill(prompt, SIGKILL);
        prompt = fork();
    }
    }
    
    munmap(usrinput, 100);
    munmap(p, sizeof(int));
    return 0;
}