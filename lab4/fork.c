#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

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
        printf("Cannot read file, %s. \n", fileName);
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
        printf("Cannot read file, %s. \n", fileName);
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

    fclose(fp);
    return imageData;
}

int arg_checker(int arglen, char *file1, double brightness, int parallel, char *outfile){
    if(arglen < 5){
        printf("Too little arguments. \n");
        return -1;
    }
    if(arglen > 5){
        printf("Too many arguments. \n");
        return -1;
    }
    int len;
    len = strlen(file1);
    if(strcmp((file1 + len - 4), ".bmp")){
        printf("Incorrect file type, file1: %s \n", file1);
        return -1;
    }
    len = strlen(outfile);
    if(strcmp((outfile + len - 4), ".bmp")){
        printf("Incorrect file location, out: %s \n", outfile);
        return -1;
    }
    if(!(brightness > 0) || !(brightness < 1)){
        printf("Incorrect brightness value. \n");
        fprintf(stderr, "%f \n", brightness);
        return -1;
    }
    if(parallel != 0 && parallel != 1){
        printf("Incorrect parallel value. \n");
        return -1;
    }

    return 0;
}

int main(int argc, char *argv[]){
    int arglen = argc;
    char *file1;
    double brightness = atof(argv[2]);
    int parallel = atoi(argv[3]);
    char *out;
    file1 = argv[1];
    out = argv[4];

    arglen = 5;
    file1 = "flowers.bmp";
    brightness = 0.8;
    parallel = 0;
    out = "test.bmp";

    int check_args = arg_checker(arglen, file1, brightness, parallel, out);
    if(check_args){
        /*
        printf("Refer to man_blendimages.txt \n");
        FILE *man;
        man = fopen("man_blendimages.txt", "r");
        if(man == NULL){
            printf("Cannot read file, %s \n", "man_blendimages.txt");
            return -1;
        }
        char c = fgetc(man);
        while(c != EOF){
            printf("%c", c);
            c = fgetc(man);
        }
        fclose(man);
        */
        fprintf(stderr, "Incorrect user input. \n");
        //return -1;
    }

    BITMAPFILEHEADER bmpFileHeader1;
    BITMAPINFOHEADER bmpInfoHeader1;
    BITMAPFILEHEADER bmpFileHeaderData;
    BITMAPINFOHEADER bmpInfoHeaderData;

    /* read pixel headers image 1 */
    int check_read = read_headers(&bmpFileHeader1, &bmpInfoHeader1, file1);
    if(check_read){
        return -1;
    }
    bmpFileHeaderData = bmpFileHeader1;
    bmpInfoHeaderData = bmpInfoHeader1;

    BYTE *pixelArray1 = get_image(&bmpFileHeader1, &bmpInfoHeader1, file1); // get file data

    /* write to output file */
    FILE *outfile;
    outfile = fopen(out, "wb");
    if(outfile == NULL){
        printf("Cannot write to file, %s. \n", out);
        return -1;
    }
    
    /* write headers to output */
    fwrite(&(bmpFileHeaderData.bfType), sizeof(WORD), 1, outfile);
    fwrite(&(bmpFileHeaderData.bfSize), sizeof(DWORD), 1, outfile);
    fwrite(&(bmpFileHeaderData.bfReserved1), sizeof(WORD), 1, outfile);
    fwrite(&(bmpFileHeaderData.bfReserved2), sizeof(WORD), 1, outfile);
    fwrite(&(bmpFileHeaderData.bfOffBits), sizeof(DWORD), 1, outfile);
    fwrite(&bmpInfoHeaderData, bmpInfoHeaderData.biSize, 1, outfile);

    BYTE *brighterData;
    size_t padding = (24 * bmpInfoHeaderData.biWidth + 31) / 32 * 4;
    size_t imageSize = padding * abs(bmpInfoHeaderData.biHeight);
    brighterData = (BYTE *)malloc(imageSize);
    if(brighterData == NULL){
        printf("Cannot allocate memory for blended data. \n");
        return -1;
    }
    brighterData = pixelArray1;

    fwrite(brighterData, imageSize, 1, outfile);
    
    fclose(outfile);
    free(pixelArray1);
    free(brighterData);
    return 0;
}