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
    size_t padding = (24 * width + 31) / 32 * 4;
    size_t imageSize = padding * height;
    BYTE *imageData;
    imageData = (BYTE *)malloc(imageSize);
    if(imageData == NULL){
        printf("Cannot allocate memory for image data. \n");
        return NULL;
    }
    fread(imageData, imageSize, 1, fp);
    /*
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            size_t pos = 3 * width * y + 3 * x;
            imageData[pos + 0] = fgetc(fp);
            imageData[pos + 1] = fgetc(fp);
            imageData[pos + 2] = fgetc(fp);
        }
    }*/

    fclose(fp);
    return imageData;
}

BYTE get_color(BYTE *imageData, BITMAPINFOHEADER *ih, int x, int y, char *s){
    BYTE color;
    size_t padding = (24 * ih->biWidth + 31) / 32 * 4;
    size_t pos = padding * y + 3 * x;

    if(!strcmp(s, "b")){
        color = imageData[pos + 0];
        return color;
    }
    else if(!strcmp(s, "g")){
        color = imageData[pos + 1];
        return color;
    }
    else if(!strcmp(s, "r")){
        color = imageData[pos + 2];
        return color;
    }

    printf("no match. \n");
    return -1;
}

BYTE get_color_bilinear(BYTE *imageData, BITMAPINFOHEADER *ih, float x, float y, char *s){
    BYTE color;
    int x1, x2, y1, y2;
    float dx, dy;
    x1 = x / 1;
    if(x1 < x){
        x2 = x1 + 1;
    }
    else{
        x2 = x1;
    }
    dx = x - x1;
    y1 = y / 1;
    if(y1 < y){
        y2 = y1 + 1;
    }
    else{
        y2 = y1;
    }
    dy = y - y1;

    if(!strcmp(s, "r")){
        BYTE red_left_upper = get_color(imageData, ih, x1, y2, "r");
        BYTE red_right_upper = get_color(imageData, ih, x2, y2, "r");
        BYTE red_left_lower = get_color(imageData, ih, x1, y1, "r");
        BYTE red_right_lower = get_color(imageData, ih, x2, y1, "r");
        BYTE red_left = red_left_upper * (1 - dy) + red_left_lower * dy;
        BYTE red_right = red_right_upper * (1 - dy) + red_right_lower * dy;
        color = red_left * (1 - dx) + red_right * dx;
        return color;
    }
    else if(!strcmp(s, "g")){
        BYTE green_left_upper = get_color(imageData, ih, x1, y2, "g");
        BYTE green_right_upper = get_color(imageData, ih, x2, y2, "g");
        BYTE green_left_lower = get_color(imageData, ih, x1, y1, "g");
        BYTE green_right_lower = get_color(imageData, ih, x2, y1, "g");
        BYTE green_left = green_left_upper * (1 - dy) + green_left_lower * dy;
        BYTE green_right = green_right_upper * (1 - dy) + green_right_lower * dy;
        color = green_left * (1 - dx) + green_right * dx;
        return color;
    }
    else if(!strcmp(s, "b")){
        BYTE blue_left_upper = get_color(imageData, ih, x1, y2, "b");
        BYTE blue_right_upper = get_color(imageData, ih, x2, y2, "b");
        BYTE blue_left_lower = get_color(imageData, ih, x1, y1, "b");
        BYTE blue_right_lower = get_color(imageData, ih, x2, y1, "b");
        BYTE blue_left = blue_left_upper * (1 - dy) + blue_left_lower * dy;
        BYTE blue_right = blue_right_upper * (1 - dy) + blue_right_lower * dy;
        color = blue_left * (1 - dx) + blue_right * dx;
        return color;
    }

    printf("no match. \n");
    return -1;
}

int main(int argc, char *argv[]){
    char *im1 = "lion.bmp";
    char *im2 = "flowers.bmp";
    double ratio = .5;
    BITMAPFILEHEADER bmpFileHeader1;
    BITMAPINFOHEADER bmpInfoHeader1;
    BITMAPFILEHEADER bmpFileHeader2;
    BITMAPINFOHEADER bmpInfoHeader2;
    BITMAPFILEHEADER bmpFileHeaderData;
    BITMAPINFOHEADER bmpInfoHeaderData;

    /* read pixel headers image 1 */
    int check = read_headers(&bmpFileHeader1, &bmpInfoHeader1, im1);
    if(check){
        return -1;
    }
    bmpFileHeaderData = bmpFileHeader1;
    bmpInfoHeaderData = bmpInfoHeader1;

    /* read pixel headers image 2 */
    int check2 = read_headers(&bmpFileHeader2, &bmpInfoHeader2, im2);
    if(check2){
        return -1;
    }
    if(bmpInfoHeader2.biWidth > bmpInfoHeader1.biWidth){
        bmpFileHeaderData = bmpFileHeader2;
        bmpInfoHeaderData = bmpInfoHeader2;
    }

    BYTE *pixelArray1 = get_image(&bmpFileHeader1, &bmpInfoHeader1, im1);
    BYTE *pixelArray2 = get_image(&bmpFileHeader2, &bmpInfoHeader2, im2);

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

    BYTE *blendedData;
    size_t padding = (24 * bmpInfoHeaderData.biWidth + 31) / 32 * 4;
    size_t imageSize = padding * abs(bmpInfoHeaderData.biHeight);
    blendedData = (BYTE *)malloc(imageSize);
    if(blendedData == NULL){
        printf("Cannot allocate memory for blended data. \n");
        return -1;
    }
    BYTE r1, g1, b1, r2, g2, b2;

    for(int y = 0; y < bmpInfoHeaderData.biHeight; y++){
        for(int x = 0; x < bmpInfoHeaderData.biWidth; x++){
            size_t pos = padding * y + 3 * x;
            float x1, y1, x2, y2;
            if(bmpInfoHeader1.biWidth < bmpInfoHeaderData.biWidth){
                x1 = (float) (bmpInfoHeader1.biWidth-1) / bmpInfoHeaderData.biWidth * x;         
            }
            else{
                x1 = x;
            }
            if(bmpInfoHeader1.biHeight < bmpInfoHeaderData.biHeight){
                y1 = (float) (bmpInfoHeader1.biHeight-1) / bmpInfoHeaderData.biHeight * y;        
            }
            else{
                y1 = y;
            }
            if(bmpInfoHeader2.biWidth < bmpInfoHeaderData.biWidth){
                x2 = (float) (bmpInfoHeader2.biWidth-1) / bmpInfoHeaderData.biWidth * x;         
            }
            else{
                x2 = x;
            }
            if(bmpInfoHeader2.biHeight < bmpInfoHeaderData.biHeight){
                y2 = (float) (bmpInfoHeader2.biHeight-1) / bmpInfoHeaderData.biHeight * y;        
            }
            else{
                y2 = y;
            }
            r1 = get_color_bilinear(pixelArray1, &bmpInfoHeader1, x1, y1, "r");
            g1 = get_color_bilinear(pixelArray1, &bmpInfoHeader1, x1, y1, "g");
            b1 = get_color_bilinear(pixelArray1, &bmpInfoHeader1, x1, y1, "b");
            r2 = get_color_bilinear(pixelArray2, &bmpInfoHeader2, x2, y2, "r");
            g2 = get_color_bilinear(pixelArray2, &bmpInfoHeader2, x2, y2, "g");
            b2 = get_color_bilinear(pixelArray2, &bmpInfoHeader2, x2, y2, "b");
            blendedData[pos + 0] = ratio * b1 + (1-ratio) * b2;
            blendedData[pos + 1] = ratio * g1 + (1-ratio) * g2;
            blendedData[pos + 2] = ratio * r1 + (1-ratio) * r2;
        }
    }

    fwrite(blendedData, imageSize, 1, test);
    
    fclose(test);
    free(pixelArray1);
    free(pixelArray2);
    free(blendedData);
    return 0;
}