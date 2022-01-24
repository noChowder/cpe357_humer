#include <stdio.h>
#include <stdlib.h>

unsigned char myheap[1048576];
#define PAGESIZE 1024

typedef struct chunkhead{
    unsigned int size;
    unsigned int info;
    unsigned char *next, *prev;
}chunkhead;

unsigned char *mymalloc(unsigned int size){
    int n = 1;

    return ;
}

void myfree(unsigned char *address){
}

void analyse(){

}

void main(){
    unsigned char *a, *b, *c;
    a = mymalloc(1000);
    b = mymalloc(1000);
    c = mymalloc(1000);
    myfree(b);
    myfree(a);
    analyse();
}