#include <stdio.h>
#include <unistd.h>

unsigned char myheap[1048576];
typedef unsigned char BYTE;
#define PAGESIZE 4096

typedef struct chunkhead{
    unsigned int size;
    unsigned int info;
    unsigned char *next, *prev;
}chunkhead;

void *startofheap = NULL;

chunkhead *get_last_chunk(){
    if(!startofheap){
        return NULL;
    }
    chunkhead *ch = (chunkhead *)startofheap;
    for( ; ch->next; ch = (chunkhead *)ch->next);
    return ch;
}

void analyze(){
    printf("\n--------------------------------------------------------------\n");
    if(!startofheap){
        printf("no heap");
        return;
    }
    chunkhead *ch = (chunkhead *)startofheap;
    for(int no = 0; ch; ch = (chunkhead *)ch->next, no++){
        printf("%d | current addr: %x |", no, ch);
        printf("size: %d | ", ch->size);
        printf("info: %d | ", ch->info);
        printf("next: %x", ch->prev);
        printf("      \n");
    }
    printf("prgram break on address %x\n", sbrk(0));
}

unsigned char *mymalloc(unsigned int size){
    return NULL;
}

void myfree(unsigned char *address){

}

void main(){
    BYTE *a[100];
    analyze(); // 50% points
    for(int i = 0; i < 100; i++){
        a[i] = mymalloc(1000);
    }
    for(int i = 0; i < 90; i++){
        myfree(a[i]);
    }
    analyze(); // 50% of points if this is correct
    myfree(a[95]);
    a[95] = mymalloc(1000);
    analyze(); // 25% points, this new chunk shoulf fill the smaller free one
    // (best fit)
    for(int i = 90; i < 100; i++){
        myfree(a[i]);
    }
    analyze(); // 25% should be an empty heap now witht the star address
    // from the program start
}