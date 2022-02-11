#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmpheaders.h"

typedef struct col{
    int r, g, b;    // red, green, blue, in that order. values not to exceed 255
}col;

typedef struct compressedformat{
    int width, height;  // width and height of the image, one byte each color, bgr
    int rowbyte_quarter[4]; // parallel algorithmns. location in bytes which exactly splits the result image after decompression into 4 equal parts
    int palettecolors;  // how many colors does the picture have?
    col colors[11];  // all participating colors of the image (11 read from palettecolors)
}compressedformat;

typedef struct chunk{
    BYTE color_index;   // which of the color of the palette
    short count;    // how many pixel of the same color from color_index are continuously appearing
}chunk;

int readCompressedHeader(char *fileName, compressedformat *compressedFileHeader){
    FILE *fp;
    fp = fopen(fileName, "rb");
    if(fp == NULL){
        printf("Cannot read %s \n", fileName);
        return -1;
    }

    fread(&compressedFileHeader->width, sizeof(int), 1, fp);
    fread(&compressedFileHeader->height, sizeof(int), 1, fp);
    fread(&compressedFileHeader->rowbyte_quarter, sizeof(int), 4, fp);
    fread(&compressedFileHeader->palettecolors, sizeof(int), 1, fp);
    fread(&compressedFileHeader->colors, sizeof(col), 11, fp);

    fclose(fp);
    return 0;
}

int readCompressedData(char *fileName, chunk *data){
    FILE *fp;
    fp = fopen(fileName, "rb");
    if(fp == NULL){
        printf("Cannot read %s \n", fileName);
        return -1;
    }
    fseek(fp, sizeof(compressedformat), SEEK_SET);

    while( fread(&data->color_index, sizeof(BYTE), 1, fp) != 0 ){
        fread(&data->count, sizeof(short), 1, fp);
    }

    fclose(fp);
    return 0;
}

int main(){
    compressedformat cfh;
    chunk cdata;

    int check_compressedHeader = readCompressedHeader("compressed.bin", &cfh);
    if(check_compressedHeader){
        perror("readCompressedHeader() failed");
        return -1;
    }
    int check_compressedData = readCompressedData("compressed.bin", &cdata);
    if(check_compressedData){
        perror("readCompressedData() failed");
        return -1;
    }
    
    for(int i = 0; i < cfh.palettecolors; i++){
        printf("index: %d \n", i);
        printf("red: %d \n", cfh.colors[i].r);
        printf("green: %d \n", cfh.colors[i].g);
        printf("blue: %d \n", cfh.colors[i].b);
        putchar('\n');
    }

    return 0;
}