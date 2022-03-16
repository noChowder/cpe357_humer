#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define size 10000

void main(){
    FILE *fp;
    fp = fopen("test2.txt", "w");
    char c;
    int i = 0;

    while(i < size){
    for(int i = 0; i < 10; i++){
        c = i+'0';
        fputc(c, fp);
    }
    i++;
    }

    fclose(fp);
}