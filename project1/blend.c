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

int read_headers(BITMAPFILEHEADER *fh, BITMAPINFOHEADER *ih, char *fileName){
    FILE *fp;

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

    fclose(fp);
    return 0;
}

BYTE *get_image(BITMAPFILEHEADER *fh, BITMAPINFOHEADER *ih, char *fileName){
    FILE *fp;

    fp = fopen(fileName, "rb");
    if(fp == NULL){
        printf("Cannot read file %s. \n", fileName);
        return NULL;
    }
    fseek(fp, fh->bfOffBits, SEEK_SET);

    size_t height = abs(ih->biHeight);
    size_t width = ih->biWidth;
    //size_t rowBytes = (24 * width + 31) / 32 * 4;
    //size_t imageSize = rowBytes * height;
    BYTE *imageData;
    imageData = (BYTE *)malloc(ih->biSizeImage);
    //printf("%ld \n", imageSize);

    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            size_t pos = width*3 * y + 3 * x;
            for(size_t k = 0; k < 3; k++){
                imageData[pos + k] = fgetc(fp);
            }
        }
    }

    fclose(fp);
    return imageData;
}

BYTE *get_color(BYTE *arr1, BYTE *arr2, size_t imageSize, double ratio){
    BYTE *imageData;
    imageData = (BYTE *)malloc(imageSize);

    for(int i = 0; i < imageSize; i++){
        imageData[i] = (ratio * arr1[i] + (1-ratio) * arr2[i]);
    }

    return imageData;
}

int main(int argc, char *argv[]){
    char *im1 = "lion.bmp";
    char *im2 = "wolf.bmp";
    BITMAPFILEHEADER bmpFileHeader;
    BITMAPINFOHEADER bmpInfoHeader;
    size_t imageSize = 0;

    /* read pixel data image 1 */
    int check = read_headers(&bmpFileHeader, &bmpInfoHeader, im1);
    if(check){
        return -1;
    }
    BYTE *pixelArray1 = get_image(&bmpFileHeader, &bmpInfoHeader, im1);
    if(bmpInfoHeader.biSizeImage > imageSize){
        imageSize = bmpInfoHeader.biSizeImage;
    }

    /* read pixel data image 2 */
    int check2 = read_headers(&bmpFileHeader, &bmpInfoHeader, im2);
    if(check2){
        return -1;
    }
    BYTE *pixelArray2 = get_image(&bmpFileHeader, &bmpInfoHeader, im2);
    if(bmpInfoHeader.biSizeImage > imageSize){
        imageSize = bmpInfoHeader.biSizeImage;
    }

    /* write to output file */
    FILE *test;
    test = fopen("test.bmp", "wb");
    if(test == NULL){
        printf("Cannot write to file. \n");
        return -1;
    }

    //printf("%ld \n", ftell(test));
    fwrite(&(bmpFileHeader.bfType), 2, 1, test);
    fwrite(&(bmpFileHeader.bfSize), 4, 1, test);
    fwrite(&(bmpFileHeader.bfReserved1), 2, 1, test);
    fwrite(&(bmpFileHeader.bfReserved2), 2, 1, test);
    fwrite(&(bmpFileHeader.bfOffBits), 4, 1, test);
    fwrite(&bmpInfoHeader, bmpInfoHeader.biSize, 1, test);
    //printf("%ld \n", ftell(test));

    BYTE *blendedData = get_color(pixelArray1, pixelArray2, imageSize, .5);

    fwrite(blendedData, imageSize, 1, test);
    //printf("%ld \n", ftell(test));
    fclose(test);

    int check3 = read_headers(&bmpFileHeader, &bmpInfoHeader, "test.bmp");
    if(check3){
        return -1;
    }
    test = fopen("test.bmp", "rb");
    //printf("%x \n", bmpFileHeader.bfType);
    //printf("file size: %d \n", bmpFileHeader.bfSize);
    //printf("image size: %d \n", bmpInfoHeader.biSizeImage);
    //printf("offset: %d \n", bmpFileHeader.bfOffBits);
    
    fclose(test);
    free(pixelArray1);
    free(pixelArray2);
    free(blendedData);
    return 0;
}