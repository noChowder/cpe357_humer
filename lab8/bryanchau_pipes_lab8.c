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

void init_pipe(mypipe *pipe, int size){
    pipe->buffersize = size;
    pipe->pipebuffer = (byte *)malloc(pipe->buffersize);
    pipe->start_occupied = 0;
    pipe->end_occupied = 0;
}

int mywrite(mypipe *pipe, byte *buffer, int size){
    if((pipe->buffersize > size) && ((pipe->end_occupied - pipe->start_occupied) == 0)){
        for(int i = 0; i < size; i++){
            *(pipe->pipebuffer + i) = *(buffer + i);
        }
        pipe->start_occupied = 0;
        pipe->end_occupied = size;
        return size;
    }
    else{
        if((pipe->buffersize > size) && ((pipe->end_occupied + size) <= pipe->buffersize)){
            for(int i = pipe->end_occupied; i < (pipe->end_occupied + size); i++){
                *(pipe->pipebuffer + i) = *(buffer + (i-pipe->end_occupied));
            }
            pipe->end_occupied = (pipe->end_occupied + size);
            return size;
        }
        else{
            perror("not enough space");
            return -1;
        }

        return size;
    }
}

int myread(mypipe *pipe, byte *buffer, int size){
    if((pipe->buffersize > size) && ((pipe->end_occupied - pipe->start_occupied) > 0)){
        for(int i = pipe->start_occupied; i < (pipe->start_occupied + size); i++){
            *(buffer + (i-pipe->start_occupied)) = *(pipe->pipebuffer + i);
        }
        pipe->start_occupied = size;
        return size;
    }
    else{
        if((pipe->buffersize > size)){

        }
        else{
            perror("not enough space");
            return -1;
        }

        return 0;
    }
}

void main(){
    char text[100];
    mypipe pipeA;

    init_pipe(&pipeA, 32);
    mywrite(&pipeA, "hello world", 12);
    mywrite(&pipeA, "it's a nice day", 16);

    myread(&pipeA, text, 12);
    printf("%s \n", text);
    myread(&pipeA, text, 16);
    printf("%s \n", text);

    //mywrite(&pipeA, "and now we test the carryover", 30);
    //myread(&pipeA, text, 30);
    //printf("%s \n", text);
}