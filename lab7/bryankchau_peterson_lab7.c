#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
    char einstein1[] = "Two things are infinite: the universe and human stupidity; and I'm not sure about the universe.";
    char einstein2[] = "If you can't explain it to a six year old, you don't understand it yourself.";

    char *text = (char *)mmap(NULL, 1000, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    
    /* peterson's alg */
    int *flag0 = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);    
    int *flag1 = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);    
    int *turn = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    *flag0 = 0;
    *flag1 = 0;
    *turn = 0;

    if(fork() == 0){
        for(int i = 0; ; i++){
            *flag0 = 1;
            *turn = 1;
            if(i%2 == 0){
                while(*flag1 == 1 && *turn == 1);
                strcpy(text, einstein1);
                *flag0 = 0;
            }
            else{
                while(*flag1 == 1 && *turn == 1);
                strcpy(text, einstein2);
                *flag0 = 0;
            }
        }
    }
    else{
        while(1){
            *flag1 = 1;
            *turn = 0;
            while(*flag0 == 1 && *turn == 0);
            char outtext[100];
            strcpy(outtext, text);
            printf("%s \n", outtext);
            *flag1 = 0;
        }
    }

    munmap(text, 1000);
    munmap(turn, sizeof(int));
    munmap(flag0, sizeof(int));
    munmap(flag1, sizeof(int));
}