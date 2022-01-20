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
    size_t rowBytes = (24 * width + 31) / 32 * 4;
    //size_t imageSize = rowBytes * height;
    BYTE *imageData;
    imageData = (BYTE *)malloc(ih->biSizeImage);

    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            size_t pos = rowBytes * y + 3 * x;
            for(size_t k = 0; k < 3; k++){
                imageData[pos + k] = fgetc(fp);
            }
        }
    }

    fclose(fp);
    return imageData;
}

BYTE get_color(BYTE *imageData, BITMAPINFOHEADER *ih, int x, int y, char *s){
    BYTE color;
    size_t rowBytes = (24 * ih->biWidth + 31) / 32 * 4;

    if(!strcmp(s, "r")){
        color = imageData[x*3 + y*rowBytes + 0];
        return color;
    }
    else if(!strcmp(s, "g")){
        color = imageData[x*3 + y*rowBytes + 1];
        return color;
    }
    else if(!strcmp(s, "b")){
        color = imageData[x*3 + y*rowBytes + 2];
        return color;
    }

    printf("no match. \n");
    return -1;
}

BYTE *get_color_bilinear(BYTE *arr1, BYTE *arr2, size_t imageSize, double ratio){
    BYTE *imageData;
    imageData = (BYTE *)malloc(imageSize);

    return imageData;
}

int main(int argc, char *argv[]){
    char *im1 = "lion.bmp";
    char *im2 = "wolf.bmp";
    BITMAPFILEHEADER bmpFileHeader1;
    BITMAPINFOHEADER bmpInfoHeader1;
    BITMAPFILEHEADER bmpFileHeader2;
    BITMAPINFOHEADER bmpInfoHeader2;
    BITMAPFILEHEADER bmpFileHeaderData;
    BITMAPINFOHEADER bmpInfoHeaderData;
    size_t imageSize = 0;

    /* read pixel headers image 1 */
    int check = read_headers(&bmpFileHeader1, &bmpInfoHeader1, im1);
    if(check){
        return -1;
    }
    if(bmpInfoHeader1.biSizeImage > imageSize){
        imageSize = bmpInfoHeader1.biSizeImage;
        bmpFileHeaderData = bmpFileHeader1;
        bmpInfoHeaderData = bmpInfoHeader1;
    }

    /* read pixel headers image 2 */
    int check2 = read_headers(&bmpFileHeader2, &bmpInfoHeader2, im2);
    if(check2){
        return -1;
    }
    if(bmpInfoHeader2.biSizeImage > imageSize){
        imageSize = bmpInfoHeader2.biSizeImage;
        bmpFileHeaderData = bmpFileHeader2;
        bmpInfoHeaderData = bmpInfoHeader2;
    }

    BYTE *pixelArray1 = get_image(&bmpFileHeader1, &bmpInfoHeader1, im1);
    BYTE *pixelArray2 = get_image(&bmpFileHeader2, &bmpInfoHeader2, im2);

    /*
    BYTE r, g, b;
    int x, y;
    x = 0;
    y = 0;
    r = get_color(pixelArray1, &bmpInfoHeader1, x, y, "r");
    g = get_color(pixelArray1, &bmpInfoHeader1, x, y, "g");
    b = get_color(pixelArray1, &bmpInfoHeader1, x, y, "b");
    printf("%d %d %d \n", r, g, b);
    */

    /* write to output file */
    FILE *test;
    test = fopen("test.bmp", "wb");
    if(test == NULL){
        printf("Cannot write to file. \n");
        return -1;
    }
    
    /* write headers to output */
    fwrite(&(bmpFileHeaderData.bfType), 2, 1, test);
    fwrite(&(bmpFileHeaderData.bfSize), 4, 1, test);
    fwrite(&(bmpFileHeaderData.bfReserved1), 2, 1, test);
    fwrite(&(bmpFileHeaderData.bfReserved2), 2, 1, test);
    fwrite(&(bmpFileHeaderData.bfOffBits), 4, 1, test);
    fwrite(&bmpInfoHeaderData, bmpInfoHeaderData.biSize, 1, test);

    double ratio = 1;
    BYTE *blendedData;
    blendedData = (BYTE *)malloc(imageSize);
    if(blendedData == NULL){
        printf("Cannot allocate memory for blended data. \n");
        return -1;
    }

    BYTE r1, g1, b1, r2, g2, b2;
    size_t rowBytes = (24 * bmpInfoHeaderData.biWidth + 31) / 32 * 4;
    for(int y = 0; y < abs(bmpInfoHeaderData.biHeight); y++){
        for(int x = 0; x < bmpInfoHeaderData.biWidth; x++){
            size_t pos = rowBytes * y + 3 * x;
            r1 = get_color(pixelArray1, &bmpInfoHeader1, x, y, "r");
            g1 = get_color(pixelArray1, &bmpInfoHeader1, x, y, "g");
            b1 = get_color(pixelArray1, &bmpInfoHeader1, x, y, "b");
            r2 = get_color(pixelArray2, &bmpInfoHeader2, x, y, "r");
            g2 = get_color(pixelArray2, &bmpInfoHeader2, x, y, "g");
            b2 = get_color(pixelArray2, &bmpInfoHeader2, x, y, "b");
            blendedData[pos + 0] = ratio * r1 + (1-ratio) * r2;
            blendedData[pos + 1] = ratio * g1 + (1-ratio) * g2;
            blendedData[pos + 2] = ratio * b1 + (1-ratio) * b2;
        }
    }
    //printf("%d %d %d \n", pixelArray1[0], pixelArray1[1], pixelArray1[2]);

    fwrite(blendedData, imageSize, 1, test);
    
    fclose(test);
    free(pixelArray1);
    free(pixelArray2);
    free(blendedData);
    return 0;
}