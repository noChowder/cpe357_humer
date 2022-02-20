#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

int main(){
    char einstein1[] = "Two things are infinite: the universe and human stupidity; and I'm not sure about the universe.";
    char einstein2[] = "If you can't explain it to a six year old, you don't understand it yourself.";

    char *text = (char *)mmap(NULL, 1000, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    
    /* peterson's alg */
    int *flag = (int *)mmap(NULL, 2*sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);    
    int *turn = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    *flag = *(flag+1) = 0; // 0 idle, 1 waiting, 2 active
    *turn = 0;
    

    if(fork() == 0){
        for(int i = 0; ; i++){
            while(1){
                *flag = 1;
                int index = *turn;
                while(index != 0){
                    if(*(flag+index) != 0){
                        index = *turn;
                    }
                    else{
                        index = (index+1) % 2;
                    }
                }
                *flag = 2;
                index = 0;
                while((index < 2) && ((index == 0) || (*(flag+index) != 2))){
                    index = index+1;
                }
                if((index >= 2) && ((*turn == 0) || (*(flag+*turn) == 0))){
                    break;
                }
            }

            if(i%2 == 0){
                *turn = 0;
                strcpy(text, einstein1);
                int index = (*turn+1) % 2;
                while(*(flag+index) == 0){
                    index = (index+1) % 2;
                }
                *turn = index;
                *flag = 0;
            }
            else{
                *turn = 0;
                strcpy(text, einstein2);
                int index = (*(turn)+1) % 2;
                while(*(flag+index) == 0){
                    index = (index+1) % 2;
                }
                *turn = index;
                *flag = 0;
            }
        }
    }
    else{
        while(1){
                *(flag+1) = 1;
                int index = *turn;
                while(index != 1){
                    if(*(flag+index) != 0){
                        index = *turn;
                    }
                    else{
                        index = (index+1) % 2;
                    }
                }
                *(flag+1) = 2;
                index = 0;
                while((index < 2) && ((index == 1) || (*(flag+index) != 2))){
                    index = index+1;
                }
                if((index >= 2) && ((*turn == 0) || (*(flag+*turn) == 0))){
                    break;
                }
            }
        while(1){
            *turn = 1;
            char outtext[100];
            strcpy(outtext, text);
            printf("%s \n", outtext);
            int index = (*(turn)+1) % 2;
            while(*(flag+index) == 0){
                index = (index+1) % 2;
            }
            *turn = index;
            *(flag+1) = 0;
        }
    }

    munmap(text, 1000);
    munmap(turn, sizeof(int));
    munmap(flag, 2*sizeof(int));
}