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

int main(int argc, char *argv[]){
    FILE *im1, *test;
    BITMAPFILEHEADER bmpFileHeader;
    BITMAPINFOHEADER bmpInfoHeader;

    im1 = fopen("lion.bmp", "rb");
    if(im1 == NULL){
        return -1;
    }
    test = fopen("test.bmp", "wb");
    if(test == NULL){
        return -1;
    }

    /* read file header */
    fread(&(bmpFileHeader.bfType), sizeof(WORD), 1, im1);
    fread(&(bmpFileHeader.bfSize), sizeof(DWORD), 1, im1);
    fread(&(bmpFileHeader.bfReserved1), sizeof(WORD), 1, im1);
    fread(&(bmpFileHeader.bfReserved2), sizeof(WORD), 1, im1);
    fread(&(bmpFileHeader.bfOffBits), sizeof(DWORD), 1, im1);

    if(bmpFileHeader.bfType != 0x4D42){ // check if bmp file type
        printf("Not a bmp file. \n");
        fclose(im1);
        return -1;
    }

    /* read info header */
    fread(&(bmpInfoHeader.biSize), sizeof(DWORD), 1, im1);
    fread(&(bmpInfoHeader.biWidth), sizeof(LONG), 1, im1);
    fread(&(bmpInfoHeader.biHeight), sizeof(LONG), 1, im1);
    fread(&(bmpInfoHeader.biPlanes), sizeof(WORD), 1, im1);
    fread(&(bmpInfoHeader.biBitCount), sizeof(WORD), 1, im1);
    fread(&(bmpInfoHeader.biCompression), sizeof(DWORD), 1, im1);
    fread(&(bmpInfoHeader.biSizeImage), sizeof(DWORD), 1, im1);
    fread(&(bmpInfoHeader.biXPelsPerMeter), sizeof(LONG), 1, im1);
    fread(&(bmpInfoHeader.biYPelsPerMeter), sizeof(LONG), 1, im1);
    fread(&(bmpInfoHeader.biClrUsed), sizeof(DWORD), 1, im1);
    fread(&(bmpInfoHeader.biClrImportant), sizeof(DWORD), 1, im1);

    /* move ptr by offset */
    int *offset;
    fread(offset, bmpFileHeader.bfOffBits, 1, im1);
    printf("offset: %d \n", *offset);

    /* read pixel data */
    BYTE arr[bmpInfoHeader.biSizeImage];
    

    /* allocate image mem */

    fwrite(&bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, test);
    fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, test);
    
    fclose(im1);
    fclose(test);
    return 0;
}