#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmpheaders.h"

typedef unsigned char BYTE;

typedef struct col{
    int r, g, b;    // red, green, blue, in that order. values not to exceed 255
}col;

typedef struct compressedformat{
    int width, height;  // width and height of the image, one byte each color, bgr
    int rowbyte_quarter[4]; // parallel algorithmns. location in bytes which exactly splits the result image after decompression into 4 equal parts
    int palettecolors;  // how many colors does the picture have?
    col colors[palettecolors];  // all participating colors of the image
}compressedformat;

typedef struct chunk{
    BYTE color_index;   // which of the color of the palette
    short count;    // how many pixel of the same color from color_index are continuously appearing
}chunk;

int readcompressed(char *filename, compressedformat *compressedFileHeader){
    FILE *fp;
    fp = fopen(filename, "rb");
    if(fp == NULL){
        printf("Cannot read %s \n", filename);
        return -1;
    }

    fread(&compressedFileHeader->width, sizeof(int), 1, fp);
    fread(&compressedFileHeader->height, sizeof(int), 1, fp);
    fread(&compressedFileHeader->rowbyte_quarter, sizeof(int), 4, fp);
    fread(&compressedFileHeader->palettecolors, sizeof(int), 1, fp);

    fclose(fp);
    return 0;
}