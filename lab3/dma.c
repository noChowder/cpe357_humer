#include <stdio.h>

unsigned char myheap[1048576];
typedef unsigned char BYTE;
#define PAGESIZE 1024

typedef struct chunkhead{
    unsigned int size;
    unsigned int info;
    unsigned char *next, *prev;
}chunkhead;

unsigned char *mymalloc(unsigned int size){
    chunkhead *head;
    head = (chunkhead *)myheap;

    unsigned int reqSize = 0;
    if(size / PAGESIZE == 0){
        reqSize = PAGESIZE;
    }
    else{
        reqSize = ((int)(size / PAGESIZE) + 1) * PAGESIZE;
    }

    if(head->info == 0 && head->size == reqSize){
        head->info = 1;
        return (BYTE *)head + sizeof(chunkhead);
    }
    else if(head->info == 0 && head->size > reqSize){
        head->info = 1;
        head->prev = 0;
        BYTE *cp = (BYTE *)head + reqSize + sizeof(chunkhead);
        chunkhead *nextHead = (chunkhead *)cp;
        nextHead->info = 0;
        nextHead->prev = (BYTE *)head;
        nextHead->size = (head->size) - reqSize - sizeof(chunkhead);
        head->size = reqSize;
        head->next = (BYTE *)nextHead;
        return (BYTE *)head + sizeof(chunkhead);
    }

    if(head->info == 0 && head->size )

    return NULL;
}

void myfree(unsigned char *address){
}

void analyse(){
    int chunkNum = 1;
    chunkhead *chunk = (chunkhead *)myheap;
    while(chunk){
        printf("Chunk #%d: \n", chunkNum);
        printf("Size = %d bytes \n", chunk->size);
        if(chunk->info == 1){
            printf("Occupied \n");
        }
        else{
            printf("Free \n");
        }
        printf("Next = %p \n", chunk->next);
        printf("Prev = %p \n", chunk->prev);
        chunkNum++;
        chunk = (chunkhead *)chunk->next;
        printf("\n");
    }
}

void main(){
    unsigned char *a, *b, *c;
    chunkhead *head = (chunkhead *)myheap;
    head->size = 1048576 - sizeof(chunkhead);
    head->info = 0;
    head->next = head->prev = 0;
    a = mymalloc(1000);
    //printf("%p \n", head);
    b = mymalloc(1000);
    c = mymalloc(2000);
    myfree(b);
    myfree(a);
    analyse();
}