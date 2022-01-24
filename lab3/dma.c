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
    chunkhead *prevHead;

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
        BYTE *cp = (BYTE *)head + reqSize + sizeof(chunkhead);
        chunkhead *nextHead = (chunkhead *)cp;
        nextHead->info = 0;
        nextHead->prev = (BYTE *)head;
        nextHead->size = (head->size) - reqSize - sizeof(chunkhead);
        head->info = 1;
        head->size = reqSize;
        head->next = (BYTE *)nextHead;
        head->prev = NULL;
        return (BYTE *)head + sizeof(chunkhead);
    }

    for(; head != NULL; head = (chunkhead *)(head->next)){
        if(head->info == 1 || head->size < reqSize){
            prevHead = head;
            continue;
        }
        if(head->info == 0 && head->size == reqSize){
            head->info = 1;
            return (BYTE *)head + sizeof(chunkhead);
        }
        else if(head->info == 0 && head->size > reqSize){
            BYTE *cp = (BYTE *)head + reqSize + sizeof(chunkhead);
            chunkhead *nextHead = (chunkhead *)cp;
            nextHead->info = 0;
            nextHead->prev = (BYTE *)head;
            nextHead->size = (head->size) - reqSize - sizeof(chunkhead);
            head->info = 1;
            head->size = reqSize;
            head->next = (BYTE *)nextHead;
            head->prev = (BYTE *)prevHead;
            return (BYTE *)head + sizeof(chunkhead);
        }
        return NULL;
    }
    return NULL;
}

void myfree(unsigned char *address){
    chunkhead *head = (chunkhead *)(address - sizeof(chunkhead));
    chunkhead *nextHead = (chunkhead *)(head->next);
    chunkhead *prevHead = (chunkhead *)(head->prev);
    head->info = 0;
    if(nextHead->info == 0 && !prevHead){ // next free & prev dne
        head->size += nextHead->size + sizeof(chunkhead);
        head->next = nextHead->next;
        if(nextHead->next){
            ((chunkhead *)head->next)->prev = (BYTE *)head;
        }
    }
    else if(nextHead->info == 1 && !prevHead){ // next occupied & prev dne
        // do nothing
    }
    else if(nextHead->info == 0 && prevHead->info == 0){ // next free & prev free
        head->size += nextHead->size + prevHead->size + 2*sizeof(chunkhead);
        if(nextHead->next){
            ((chunkhead *)nextHead->next)->prev = (BYTE *)head;
        }
        if(prevHead->prev){
            ((chunkhead *)prevHead->prev)->next = (BYTE *)head;
        }
        head->next = nextHead->next;
        head->prev = prevHead->prev;
        prevHead->size = head->size;
        prevHead->next = head->next;
        prevHead->prev = head->prev;
    }
    else if(nextHead->info == 0 && prevHead->info == 1){ // next free & prev occupied
        head->size += nextHead->size + sizeof(chunkhead);
        head->next = nextHead->next;
        if(nextHead->next){
            ((chunkhead *)head->next)->prev = (BYTE *)head;
        }
    }
    else if(nextHead && prevHead->info == 0){ // next exists + prev free
        head->size += prevHead->size + sizeof(chunkhead);
        printf("%d \n", head->size);
        head->prev = prevHead->prev;
        prevHead->size = head->size;
        prevHead->next = head->next;
        if(prevHead->prev){
            ((chunkhead *)prevHead->prev)->next = (BYTE *)head;
        }
    }
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
    unsigned char *a, *b, *c, *d;
    chunkhead *head = (chunkhead *)myheap;
    head->size = 1048576 - sizeof(chunkhead);
    head->info = 0;
    head->next = head->prev = NULL;
    a = mymalloc(1000);
    b = mymalloc(1000);
    c = mymalloc(1000);
    d = mymalloc(1000);
    myfree(c);
    myfree(b);
    analyse();
}