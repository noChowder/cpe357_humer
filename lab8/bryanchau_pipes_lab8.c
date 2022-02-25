#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/times.h>
#include <time.h>
#include <fcntl.h>

typedef unsigned char byte;

typedef struct mypipe{
    byte *pipebuffer;
    int buffersize;
    int start_occupied;
    int end_occupied;
}mypipe;

void init_pipe(mypipe *pipe, int size);

int mywrite(mypipe *pipe, byte *buffer, int size);

int myread(mypipe *pipe, byte *buffer, int size);

void main(){
    char text[100];
    mypipe pipeA;
    init(&pipeA, 32);
    mywrite(&pipeA, "hello world", 12);
    mywrite(&pipeA, "it's a nice day", 16);

    myread(&pipeA, text, 12);
    printf("%s \n", text);
    myread(&pipeA, text, 16);
    printf("%s \n", text);

    mywrite(&pipeA, "and now we test the carryover", 30);
    myread(&pipeA, text, 30);
    printf("%s \n", text);
}