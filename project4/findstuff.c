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

int sub_find_file(char *file, struct dirent *entry, char *path, int match){
    char tempPath[1000];
    DIR *dir;
    dir = opendir(path);
    if(!dir){
        //printf("%s \n", path);
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
            match = sub_find_file(file, sentry, tempPath, match);
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

int find_file(char *file, char *flag){
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
                match = sub_find_file(file, entry, path, match);
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
            if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
                continue;
            }
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

int sub_find_string(char *text, struct dirent *entry, char *path, int match){
    char tempPath[1000];
    DIR *dir;
    //printf("here: %s \n", path);
    dir = opendir(path);
    if(!dir){
        //printf("%s \n", path);
        perror("opendir() failed");
        return -1;
    }
    struct dirent *sentry;
    //int match = 0;
    //printf("%s \n", path);
    while((sentry = readdir(dir)) != NULL){
        //printf("%s \n", sentry->d_name);
        if(sentry->d_type == 4 && strcmp(sentry->d_name, ".") != 0 && strcmp(sentry->d_name, "..") != 0){
            //printf("%s \n", file);
            strcpy(tempPath, path);
            strcat(tempPath, "/");
            strcat(tempPath, sentry->d_name);
            //printf("here: %s \n", tempPath);
            match = sub_find_string(text, sentry, tempPath, match);
        }
        FILE *fp;
        //printf("%s \n", path);
        strcpy(tempPath, path);
        strcat(tempPath, "/");
        strcat(tempPath, sentry->d_name);
        fp = fopen(tempPath, "rb");
        if(!fp){
            printf("%s \n", sentry->d_name);
            perror("fopen() failed");
            return -1;
        }
        char *buff;
        buff = malloc(255*sizeof(char));
        //fscanf(fp, "%s", buff);
        //printf("%s \n", buff);
        while(fscanf(fp, "%s", buff) != EOF){
            //printf("%s \n", buff);
            //sleep(2);
            if(strcmp(text, buff) == 0){
                printf("%s/%s \n", path, sentry->d_name);
                match = 1;
                fclose(fp);
                //return match;
                break;
            }
            //fscanf(fp, "%s", buff);
        }
        //fclose(fp);        
        free(buff);
    }
    closedir(dir);
    return match;
}

int find_string(char *text, char *flag1, char *flag2){
    //int match = 0;
    // remove quotes from text
    int len = strlen(text);
    for(int i = 0; i < len; i++){
        text[i] = text[i+1];
    }
    text[len-2] = '\0';
    //printf("%s \n", text);
    char path[1000];
    getcwd(path, 1000);
    //printf("%s \n", path);
    DIR *dir;
    int match = 0;
    struct dirent *entry;

    printf("Found text in following paths: \n");
    if(*flag1 == '-' && *(flag1+1) == 'f'){ // check flags, go through subdirectories, specific file type
        printf("flag1 is -f \n");
    }
    else if(strcmp(flag1, "-s") == 0){
        //printf("sub flag \n");
        dir = opendir(".");
        if(!dir){
            perror("opendir() failed");
            return -1;
        }
        while((entry = readdir(dir)) != NULL){
            if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
                continue;
            }
            if(entry->d_type == 4){
                //printf("%s \n", entry->d_name);
                match = sub_find_string(text, entry, path, match);
            }
        }
        closedir(dir);
    }
    else{ // no flags
        //printf("here \n");
        dir = opendir(".");
        if(!dir){
            perror("opendir() failed");
            return -1;
        }
        while((entry = readdir(dir)) != NULL){
            if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
                continue;
            }
            FILE *fp;
            //printf("%s \n", entry->d_name);
            fp = fopen(entry->d_name, "rb");
            if(!fp){
                perror("fopen() failed");
                return -1;
            }
            char *buff;
            buff = malloc(255*sizeof(char));
            //fscanf(fp, "%s", buff);
            //printf("%s \n", buff);
            while(fscanf(fp, "%s", buff) != EOF){
                //printf("%s \n", buff);
                //sleep(2);
                if(strcmp(text, buff) == 0){
                    printf("%s/%s \n", path, entry->d_name);
                    match = 1;
                    fclose(fp);
                    break;
                }
                //fscanf(fp, "%s", buff);
            }            
            free(buff);
            //fclose(fp);
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
    //char *usrinput = mmap(NULL, 100, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
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
        char *usrinput;
        usrinput = malloc(100*sizeof(char));
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
            //printf("%d \n", p->numProcesses);
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
                    //printf("string \n");
                    // if arg3 is -f
                    // if arg3 is -s
                    // if arg3 is -f and arg4 is -s
                    //printf("%c \n", *(args[2]+1));
                    if(*args[2] == '-' && *(args[2]+1) == 'f' && strcmp(args[3], "-s") == 0){
                        if(find_string(args[1], args[2], args[3]) != 0){
                            fprintf(stderr, ">nothing found< \n\n");
                            return -1;
                        }
                    }
                    else if(strcmp(args[2], "-s") == 0){
                        if(find_string(args[1], args[2], args[3]) != 0){
                            fprintf(stderr, ">nothing found< \n\n");
                            return -1;
                        }
                    }
                    else{ // no flags
                        if(find_string(args[1], args[2], args[3]) != 0){
                            fprintf(stderr, ">nothing found< \n");
                            return -1;
                        }
                    }
                    return 0;
                }
                else{
                    //printf("%s \n", args[3]);
                    if(strcmp(args[2], "-s") == 0){
                        if(find_file(args[1], args[2]) != 0){
                            fprintf(stderr, ">nothing found< \n\n");
                            return -1;
                        }
                    }
                    else if(strcmp(args[2], "-s") != 0){
                        //printf("this \n");
                        if(find_file(args[1], args[2]) != 0){
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
                        p->numProcesses-=1;
                        p->pids[pidSerial-1];
                        p->tasks[pidSerial-1] = NULL;
                        break;
                    }
                }
                kill(child_pid, SIGKILL);
                //return 0;
            }
        }
        else if(strcmp(args[0], "q") == 0 || strcmp(args[0], "quit") == 0){
            kill(prompt, SIGKILL);
            for(int i = 0; i < 4; i++){
                //printf("%s \n", args[i]);
                free(args[i]);
            }
            break;
        }
        else if(strcmp(args[0], "list") == 0){
            printf("Number of active processes: %d \n", p->numProcesses);
            for(int i = 0; i < 10; i++){
                if(p->tasks[i] != NULL){
                    printf("Process %d is finding %s \n", i+1, p->tasks[i]);
                }
            }
        }
        else if(strcmp(args[0], "kill") == 0){
            int pidSerial = atoi(args[1]);
            if(p->tasks[pidSerial-1] != NULL){
                kill(p->pids[pidSerial-1], SIGKILL);
                p->numProcesses-=1;
                p->pids[pidSerial-1] = 0;
                p->tasks[pidSerial-1] = NULL;
            }
        }
        
        for(int i = 0; i < 4; i++){
            //printf("%s \n", args[i]);
            free(args[i]);
            //printf("%s \n", args[i]);
        }
        free(usrinput);
        
        close(fd[0]);
        waitpid(prompt, &status, 0);
        kill(prompt, SIGKILL);
        prompt = fork();
    }
    }
    
    
    munmap(p, sizeof(int));
    return 0;
}