#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
    int i;
    if(fork() == 0){
        printf("c");
        return 0;
    }
    else{
        wait(&i);
        if(fork() == 0){
            printf("d");
            return 0;
        }
        else{
            wait(&i);
            printf("a");
        }
        printf("b");
    }
    return 0;
}