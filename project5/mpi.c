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

int main(int argc, char *argv[]){
    char *args[4];
    int par_id = 0;
    int par_count = atoi(argv[2]);
    for(int i = 0; i < par_count; i++){
        args[i] = malloc(100);
        par_id = i + '0';
        strcpy(args[0], argv[1]);
        strcpy(args[1], (char *)par_id);
        strcpy(args[2], par_count + '0');
        strcpy(args[3], "NULL");
        printf("%s \n", args[i]);
        if(fork() == 0){
            execv("./par", args);
        }
    }


    for(int i = 0; i < par_count; i++){
        free(args[i]);
    }
    return 0;
}