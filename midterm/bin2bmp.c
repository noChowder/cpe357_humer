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

int readCompressedHeader(char *filename, compressedformat *compressedFileHeader){
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
    fread(&compressedFileHeader->colors, sizeof(col), 11, fp);

    fclose(fp);
    return 0;
}

int readCompressedData(char *filename, chunk *data){
    FILE *fp;
    fp = fopen(filename, "rb");
    if(fp == NULL){
        printf("Cannot read %s \n", filename);
        return -1;
    }
    fseek(fp, sizeof(compressedformat), SEEK_SET);

    while( fread(&data->color_index, sizeof(BYTE), 1, fp) != 0 ){
        fread(&data->count, sizeof(short), 1, fp);
    }

    fclose(fp);
    return 0;
}

int decompress(char *filename, compressedformat *cfh, chunk *cdata, BITMAPFILEHEADER *fh, BITMAPINFOHEADER *fih){
    FILE *filein;
    filein = fopen(filename, "rb");
    if(filein == NULL){
        printf("Cannot read %s \n", filename);
        return -1;
    }
    fseek(filein, sizeof(compressedformat), SEEK_SET);

    /* write headers to output file */
    FILE *fileout;
    fileout = fopen("outfile.bmp", "wb");
    fwrite(&fh->bfType, sizeof(WORD), 1, fileout);
    fwrite(&fh->bfSize, sizeof(DWORD), 1, fileout);
    fwrite(&fh->bfReserved1, sizeof(WORD), 1, fileout);
    fwrite(&fh->bfReserved2, sizeof(WORD), 1, fileout);
    fwrite(&fh->bfOffBits, sizeof(DWORD), 1, fileout);
    fwrite(&fih, sizeof(BITMAPINFOHEADER), 1, fileout);

    /* write data to output file */
    while( fread(&cdata->color_index, sizeof(BYTE), 1, filein) != 0 ){
        fread(&cdata->count, sizeof(short), 1, filein);

        
    }

    fclose(filein);
    fclose(fileout);
    return 0;
}

int main(){
    compressedformat cfh;
    chunk cdata;
    BITMAPFILEHEADER fh;
    BITMAPINFOHEADER fih;

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

    /* file header info */          // given
    fh.bfType = (WORD) 19778;
    fh.bfSize = (DWORD) 4320054;
    fh.bfReserved1 = (DWORD) 0;
    fh.bfReserved2 = (WORD) 0;
    fh.bfOffBits = (DWORD) 54;
    
    /* file infoheader info */      // given
    fih.biSize = (DWORD) 40;
    fih.biWidth = (LONG) 1200;
    fih.biHeight = (LONG) 1200;
    fih.biPlanes = (WORD) 1;
    fih.biBitCount = (WORD) 24;
    fih.biCompression = (DWORD) 0;
    fih.biSizeImage = (DWORD) 4320000;
    fih.biXPelsPerMeter = (LONG) 3780;
    fih.biYPelsPerMeter = (LONG) 3780;
    fih.biClrUsed = (DWORD) 0;
    fih.biClrImportant = (DWORD) 0;

    int check_decompress = decompress("compressed.bin", &cfh, &cdata, &fh, &fih);
    if(check_decompress){
        perror("decompress() failed");
        return -1;
    }
    
    /*
    for(int i = 0; i < cfh.palettecolors; i++){
        printf("index: %d \n", i);
        printf("red: %d \n", cfh.colors[i].r);
        printf("green: %d \n", cfh.colors[i].g);
        printf("blue: %d \n", cfh.colors[i].b);
        putchar('\n');
    }*/

    return 0;
}