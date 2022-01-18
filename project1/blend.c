#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char byte;

int main(int argc, char *argv[]){
    FILE *im1, *im2, *test;
    byte buff[256];

    im1 = fopen("lion.bmp", "r");
    test = fopen("test.txt", "w");
    fgets(buff, 2, im1);
    printf("%s\n", buff);

    fclose(im1);
    fclose(test);
    return 0;
}