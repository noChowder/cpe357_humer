#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
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

BYTE *decompress(char *filename, compressedformat *cfh, chunk *cdata, BITMAPINFOHEADER *fih, BYTE *imagedata, int pos, int end, int rowbyte){
    FILE *filein;
    filein = fopen(filename, "rb");
    if(filein == NULL){
        printf("Cannot read %s \n", filename);
        return NULL;
    }
    if(rowbyte > 0){
        fseek(filein, sizeof(compressedformat) + cfh->rowbyte_quarter[rowbyte-1], SEEK_SET);
    }
    else{
        fseek(filein, sizeof(compressedformat), SEEK_SET);
    }

    /* write data to output file */
    //size_t padding = (24 * fih->biWidth + 31) / 32 * 4;
    //size_t imageSize = padding * abs(fih->biHeight);
    //pos = 0; 
    while(pos < end){
        fread(&cdata->color_index, sizeof(BYTE), 1, filein);
        fread(&cdata->count, sizeof(short), 1, filein);

        //printf("%d \n", cdata->color_index);
        while(cdata->count){
            imagedata[pos + 0] = cfh->colors[cdata->color_index].b; // blue
            imagedata[pos + 1] = cfh->colors[cdata->color_index].g; // green
            imagedata[pos + 2] = cfh->colors[cdata->color_index].r; // red
            pos+=3;
            cdata->count--;
        }
    }

    fclose(filein);
    return imagedata;
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

    /* file header info */          // given
    fh.bfType = (WORD)19778;
    fh.bfSize = (DWORD)4320054;
    fh.bfReserved1 = (DWORD)0;
    fh.bfReserved2 = (WORD)0;
    fh.bfOffBits = (DWORD)54;
    
    /* file infoheader info */      // given
    fih.biSize = (DWORD)40;
    fih.biWidth = (LONG)1200;
    fih.biHeight = (LONG)1200;
    fih.biPlanes = (WORD)1;
    fih.biBitCount = (WORD)24;
    fih.biCompression = (DWORD)0;
    fih.biSizeImage = (DWORD)4320000;
    fih.biXPelsPerMeter = (LONG)3780;
    fih.biYPelsPerMeter = (LONG)3780;
    fih.biClrUsed = (DWORD)0;
    fih.biClrImportant = (DWORD)0;

    size_t padding = (24 * fih.biWidth + 31) / 32 * 4;
    size_t imageSize = padding * abs(fih.biHeight);
    BYTE *imagedata = (BYTE *)mmap(NULL, imageSize, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if(imagedata == NULL){
        perror("mmap() failed");
        return -1;
    }
    //int *pos = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    //int *end = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);

    //printf("%d \n", cfh.rowbyte_quarter[0]);
    //printf("%d \n", cfh.rowbyte_quarter[1]);
    //printf("%d \n", cfh.rowbyte_quarter[2]);
    //printf("%d \n", cfh.rowbyte_quarter[3]);

    int f1 = fork();
    int f2 = fork();
    int pos;
    int end;

    if(f1 > 0 && f2 > 0){
        wait(0);
        //printf("process 1\n");
        pos = imageSize/4*3;
        end = imageSize;
        imagedata = decompress("compressed.bin", &cfh, &cdata, &fih, imagedata, pos, end, 3);
        if(!imagedata){
            perror("decompress() failed");
            return -1;
        }
    }
    else if(f1 == 0 && f2 > 0){
        //wait(0);
        //printf("process 2\n");
        pos = imageSize/2;
        end = imageSize/4*3;
        imagedata = decompress("compressed.bin", &cfh, &cdata, &fih, imagedata, pos, end, 2);
        if(!imagedata){
            perror("decompress() failed");
            return -1;
        }
    }
    else if(f1 > 0 && f2 == 0){
        //wait(0);
        //printf("process 3\n");
        pos = imageSize/4;
        end = imageSize/2;
        imagedata = decompress("compressed.bin", &cfh, &cdata, &fih, imagedata, pos, end, 1);
        if(!imagedata){
            perror("decompress() failed");
            return -1;
        }
    }
    else if(f1 == 0 && f2 == 0){
        //wait(0);
        //printf("process 4\n");
        pos = 0;
        end = imageSize/4;
        imagedata = decompress("compressed.bin", &cfh, &cdata, &fih, imagedata, pos, end, 0);
        if(!imagedata){
            perror("decompress() failed");
            return -1;
        }
    }
    
    /* check colors and index */
    /*for(int i = 0; i < cfh.palettecolors; i++){
        printf("index: %d \n", i);
        printf("red: %d \n", cfh.colors[i].r);
        printf("green: %d \n", cfh.colors[i].g);
        printf("blue: %d \n", cfh.colors[i].b);
        putchar('\n');
    }*/

    /*FILE *filein;
    filein = fopen("compressed.bin", "rb");
    int count = 0;
    while(fread(&cdata.color_index, sizeof(BYTE), 1, filein) != 0){
        count+=3;
    }
    printf("# of bytes: %d \n", count);
    fclose(filein);*/

    FILE *fileout;
    fileout = fopen("outfile.bmp", "wb");
    if(fileout == NULL){
        perror("fopen() failed");
        return -1;
    }

    fwrite(&fh.bfType, sizeof(WORD), 1, fileout);
    fwrite(&fh.bfSize, sizeof(DWORD), 1, fileout);
    fwrite(&fh.bfReserved1, sizeof(WORD), 1, fileout);
    fwrite(&fh.bfReserved2, sizeof(WORD), 1, fileout);
    fwrite(&fh.bfOffBits, sizeof(DWORD), 1, fileout);
    fwrite(&fih, fih.biSize, 1, fileout);
    fwrite(imagedata, imageSize, 1, fileout);

    fclose(fileout);
    munmap(imagedata, imageSize);
    return 0;
}