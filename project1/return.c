#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;

typedef struct tagBITMAPFILEHEADER{
    char bfType;    
    unsigned int bfSize; 
    short bfReserved1; 
    short bfReserved2; 
    unsigned int bfOffBits;
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

typedef struct{
    char bfType[2];
    unsigned int bfSize;
    short bfReserved1;
    short bfReserved2;
    unsigned int bfOffBits;
} mystruct;

#pragma pack(push, 1)
typedef struct {
    char         filetype[2];   /* magic - always 'B' 'M' */
    unsigned int filesize;
    short        reserved1;
    short        reserved2;
    unsigned int dataoffset;    /* offset in bytes to actual bitmap data */
} file_header;

int main(){
    printf("%ld \n", sizeof(BITMAPFILEHEADER));
    printf("%ld \n", sizeof(mystruct));
    printf("%ld \n", sizeof(file_header));
}