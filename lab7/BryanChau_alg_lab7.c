#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

int main(){
    char einstein1[] = "Two things are infinite: the universe and human stupidity; and I'm not sure about the universe.";
    char einstein2[] = "If you can't explain it to a six year old, you don't understand it yourself.";

    char *text = mmap(NULL, 1000, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    
    if(fork() == 0){
        for(int i = 0; ; i++){
            
            if(i%2 == 0){
                strcpy(text, einstein1);
            }
            else{
                strcpy(text, einstein2);
            }
        }
        return 0;
    }
    while(1){
        char outtext[100];
        strcpy(outtext, text);
        printf("%s \n", outtext);
    }

    return 0;
}