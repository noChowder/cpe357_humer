#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char byte;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;

struct tagBITMAPFILEHEADER{
    WORD bfType;  //specifies the file type 
    DWORD bfSize;  //specifies the size in bytes of the bitmap file 
    WORD bfReserved1;  //reserved; must be 0 
    WORD bfReserved2;  //reserved; must be 0 
    DWORD bfOffBits;  //species the offset in bytes from the bitmapfileheader to the bitmap bits 
}typedef BITMAPFILEHEADER;

struct tagBITMAPINFOHEADER{ 
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
}typedef BITMAPINFOHEADER; 

int main(int argc, char *argv[]){
    FILE *im1;
    BITMAPFILEHEADER bmpFileHeader;

    im1 = fopen("lion.bmp", "rb");
    fread(&bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, im1);
    if(bmpFileHeader.bfType != 0x4D42){
        fclose(im1);
        return -1;
    }
    printf("%X \n", bmpFileHeader.bfType);
    printf("%X \n", bmpFileHeader.bfSize);

    fclose(im1);
    return 0;
}