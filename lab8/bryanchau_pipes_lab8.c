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