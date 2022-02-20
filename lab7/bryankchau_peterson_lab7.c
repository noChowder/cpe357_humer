#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

int main(){
    char einstein1[] = "Two things are infinite: the universe and human stupidity; and I'm not sure about the universe.";
    char einstein2[] = "If you can't explain it to a six year old, you don't understand it yourself.";

    char *text = mmap(NULL, 1000, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    
    /* peterson's alg */
    int arr[2] = {0, 0}; // uses boolean, false & false, replace with 0,0
    int *flag = mmap(NULL, 2*sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);    
    int *turn = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    flag = &arr;
    *turn = 0;

    if(fork() == 0){
        flag[0] = 1;
        *turn = 1;
        while(flag[1] == 1 && *turn == 1){

        }
        for(int i = 0; ; i++){
            
            if(i%2 == 0){
                strcpy(text, einstein1);
            }
            else{
                strcpy(text, einstein2);
            }
        }
        flag[0] = 0;
    }
    flag[1] = 1;
    *turn = 0;
    while(flag[0] == 1 && *turn == 0){

    }
    while(1){
        char outtext[100];
        strcpy(outtext, text);
        printf("%s \n", outtext);
    }
    flag[1] = 0;

    munmap(*text, 1000);
    munmap(*turn, sizeof(int));
    munmap(*flag, 2*sizeof(int));
}