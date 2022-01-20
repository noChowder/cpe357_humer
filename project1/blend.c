#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;
typedef unsigned char BYTE;

typedef struct tagBITMAPFILEHEADER{
    WORD bfType;  //specifies the file type 
    DWORD bfSize;  //specifies the size in bytes of the bitmap file 
    WORD bfReserved1;  //reserved; must be 0 
    WORD bfReserved2;  //reserved; must be 0 
    DWORD bfOffBits;  //species the offset in bytes from the bitmapfileheader to the bitmap bits 
}BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER{ 
    DWORD biSize;  //specifies the number of bytes required by the struct 
    LONG biWidth;  //specifies width in pixels 
    LONG biHeight;  //species height in pixels 
    WORD biPlanes; //specifies the number of color planes, must be 1 
    WORD biBitCount; //specifies the number of bit per pixel 
    DWORD biCompression;//spcifies the type of compression 
    DWORD biSizeImage;  //size of image in bytes 
    LONG biXPelsPerMeter;  //number of pixels per meter in x axis 
    LONG biYPelsPerMeter;  //number of pixels per meter in y axis 
    DWORD biClrUsed;  //number of colors used by the bitmap 
    DWORD biClrImportant;  //number of colors that are important 
}BITMAPINFOHEADER;

int readHeaders(BITMAPFILEHEADER *fh, BITMAPINFOHEADER *ih, char *fileName){
    FILE *fp;
    /*fh = (BITMAPFILEHEADER *)malloc(sizeof(BITMAPFILEHEADER));
    if(fh == NULL){
        printf("Cannot allocate memory for bmpFileHeader \n");
        return -1;
    }
    ih = (BITMAPINFOHEADER *)malloc(sizeof(BITMAPINFOHEADER));
    if(ih == NULL){
        printf("Cannot allocate memory for bmpInfoHeader \n");
        return -1;
    }*/
    fp = fopen(fileName, "rb");
    if(fp == NULL){
        printf("Cannot read file %s. \n", fileName);
        return -1;
    }

    /* read file header */
    fread(&fh->bfType, sizeof(WORD), 1, fp);
    fread(&fh->bfSize, sizeof(DWORD), 1, fp);
    fread(&fh->bfReserved1, sizeof(WORD), 1, fp);
    fread(&fh->bfReserved2, sizeof(WORD), 1, fp);
    fread(&fh->bfOffBits, sizeof(DWORD), 1, fp);

    if(fh->bfType != 0x4D42){ // check if bmp file type
        printf("Not a bmp file. \n");
        fclose(fp);
        return -1;
    }

    /* read info header */
    fread(&ih->biSize, sizeof(DWORD), 1, fp);
    fread(&ih->biWidth, sizeof(LONG), 1, fp);
    fread(&ih->biHeight, sizeof(LONG), 1, fp);
    fread(&ih->biPlanes, sizeof(WORD), 1, fp);
    fread(&ih->biBitCount, sizeof(WORD), 1, fp);
    fread(&ih->biCompression, sizeof(DWORD), 1, fp);
    fread(&ih->biSizeImage, sizeof(DWORD), 1, fp);
    fread(&ih->biXPelsPerMeter, sizeof(LONG), 1, fp);
    fread(&ih->biYPelsPerMeter, sizeof(LONG), 1, fp);
    fread(&ih->biClrUsed, sizeof(DWORD), 1, fp);
    fread(&ih->biClrImportant, sizeof(DWORD), 1, fp);

    printf("%x \n", fh->bfType);

    fclose(fp);
    return 0;
}

int main(int argc, char *argv[]){
    char *im1 = "lion.bmp";
    //char *test = "test.bmp";
    BITMAPFILEHEADER bmpFileHeader;
    BITMAPINFOHEADER bmpInfoHeader;

    int read = readHeaders(&bmpFileHeader, &bmpInfoHeader, im1);
    if(read){
        return -1;
    }

    FILE *test;
    test = fopen("test.bmp", "wb");
    if(test == NULL){
        printf("Cannot write to file. \n");
        return -1;
    }

    /* read pixel data */
    BYTE *pixelArray;
    pixelArray = malloc(sizeof(BYTE)*bmpInfoHeader.biSizeImage);
    if(pixelArray == NULL){
        printf("Cannot allocate memory for pixelArray. \n");
        return -1;
    }

    fwrite(&bmpFileHeader, 14, 1, test);
    fwrite(&bmpInfoHeader, bmpInfoHeader.biSize, 1, test);
    
    fclose(test);
    free(pixelArray);
    return 0;
}