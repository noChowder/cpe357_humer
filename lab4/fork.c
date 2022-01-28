#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

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

BYTE *get_image(BITMAPFILEHEADER *fh, BITMAPINFOHEADER *ih, BYTE *imageData, char *fileName){
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

    fread(imageData, imageSize, 1, fp);
    fclose(fp);
    return imageData;
}

BYTE *brighten(BITMAPINFOHEADER *ih, BYTE *imageData, float brightness){
    size_t padding = (24 * ih->biWidth + 31) / 32 * 4;
    for(int y = 0; y < ih->biHeight; y++){
        for(int x = 0; x < ih->biWidth; x++){
            size_t pos = padding * y + 3 * x;
            int blue = (int)imageData[pos + 0] + (brightness * 255); // blue
            if(blue > 255){
                blue = 255;
            }
            imageData[pos + 0] = (BYTE)blue;
            int green = (int)imageData[pos + 1] + (brightness * 255); // green
            if(green > 255){
                green = 255;
            }
            imageData[pos + 1] = (BYTE)green;
            int red = (int)imageData[pos + 2] + (brightness * 255); // red
            if(red > 255){
                red = 255;
            }
            imageData[pos + 2] = (BYTE)red;
        }
    }
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
    float brightness = atof(argv[2]);
    int parallel = atoi(argv[3]);
    char *out;
    file1 = argv[1];
    out = argv[4];

    /*
    arglen = 5;
    file1 = "flowers.bmp";
    brightness = .2;
    parallel = 0;
    out = "test.bmp";
    */

    int check_args = arg_checker(arglen, file1, brightness, parallel, out);
    if(check_args){
        fprintf(stderr, "Incorrect user input. \n");
        return -1;
    }

    BITMAPFILEHEADER bmpFileHeaderData;
    BITMAPINFOHEADER bmpInfoHeaderData;

    /* read pixel headers image 1 */
    int check_read = read_headers(&bmpFileHeaderData, &bmpInfoHeaderData, file1);
    if(check_read){
        return -1;
    }
    
    size_t padding = (24 * bmpInfoHeaderData.biWidth + 31) / 32 * 4;
    size_t imageSize = padding * abs(bmpInfoHeaderData.biHeight);

    /* parallel is 0 */
    if(!parallel){
        BYTE *imageData = (BYTE *)malloc(imageSize);
        if(imageData == NULL){
            printf("Cannot allocate memory for image data. \n");
            return -1;
        }
        imageData = get_image(&bmpFileHeaderData, &bmpInfoHeaderData, imageData, file1); // image data
        clock_t start = clock();
        imageData = brighten(&bmpInfoHeaderData, imageData, brightness);
        clock_t end = clock();
        double timediff = (double)(end - start) / CLOCKS_PER_SEC;
        printf("\tProcess time w/o fork is: \t%f\n", timediff);

        /* open output file */
        FILE *outfile;
        outfile = fopen(out, "wb");
        if(outfile == NULL){
            printf("Cannot write to file, %s. \n", out);
            return -1;
        }
        /* write to output */
        fwrite(&(bmpFileHeaderData.bfType), sizeof(WORD), 1, outfile);
        fwrite(&(bmpFileHeaderData.bfSize), sizeof(DWORD), 1, outfile);
        fwrite(&(bmpFileHeaderData.bfReserved1), sizeof(WORD), 1, outfile);
        fwrite(&(bmpFileHeaderData.bfReserved2), sizeof(WORD), 1, outfile);
        fwrite(&(bmpFileHeaderData.bfOffBits), sizeof(DWORD), 1, outfile);
        fwrite(&bmpInfoHeaderData, bmpInfoHeaderData.biSize, 1, outfile);
        fwrite(imageData, imageSize, 1, outfile);
        fclose(outfile);
        free(imageData);
        //printf("parallel was 0 \n");
        return 0;
    }

    /* allocate using mmap */
    BYTE *brighterData = (BYTE *)mmap(NULL, imageSize, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if(brighterData == NULL){
        printf("Cannot allocate memory for brighter data. \n");
        return -1;
    }
    brighterData = get_image(&bmpFileHeaderData, &bmpInfoHeaderData, brighterData, file1);

    clock_t start = clock();
    if(fork() == 0){ // child
        //printf("child here \n");
        for(int y = 0; y < bmpInfoHeaderData.biHeight/2; y++){
            for(int x = 0; x < bmpInfoHeaderData.biWidth; x++){
                size_t pos = padding * y + 3 * x;
                int blue = (int)brighterData[pos + 0] + (brightness * 255); // blue
                if(blue > 255){
                    blue = 255;
                }
                brighterData[pos + 0] = (BYTE)blue;
                int green = (int)brighterData[pos + 1] + (brightness * 255); // green
                if(green > 255){
                    green = 255;
                }
                brighterData[pos + 1] = (BYTE)green;
                int red = (int)brighterData[pos + 2] + (brightness * 255); // red
                if(red > 255){
                    red = 255;
                }
                brighterData[pos + 2] = (BYTE)red;
            }
        }
    }
    else{ // parent
        //printf("parent here \n");
        //wait(0);
        /*LONG height = bmpInfoHeaderData.biHeight;
        printf("%d \n", height);
        if(bmpInfoHeaderData.biHeight%2 != 0){ // catches odd rows
            height = bmpInfoHeaderData.biHeight + 1;
        }*/
        wait(0);
        for(int y = bmpInfoHeaderData.biHeight/2; y < bmpInfoHeaderData.biHeight; y++){
            for(int x = 0; x < bmpInfoHeaderData.biWidth; x++){
                size_t pos = padding * y + 3 * x;
                int blue = (int)brighterData[pos + 0] + (brightness * 255); // blue
                if(blue > 255){
                    blue = 255;
                }
                brighterData[pos + 0] = (BYTE)blue;
                int green = (int)brighterData[pos + 1] + (brightness * 255); // green
                if(green > 255){
                    green = 255;
                }
                brighterData[pos + 1] = (BYTE)green;
                int red = (int)brighterData[pos + 2] + (brightness * 255); // red
                if(red > 255){
                    red = 255;
                }
                brighterData[pos + 2] = (BYTE)red;
            }
        }
        //wait(0);
    }
    clock_t end = clock();
    double timediff = (double)(end - start) / CLOCKS_PER_SEC;
    printf("\tProcess time with fork is: \t%f\n", timediff);

    /* open output file */
    FILE *outfile;
    outfile = fopen(out, "wb");
    if(outfile == NULL){
        printf("Cannot write to file, %s. \n", out);
        return -1;
    }
    /* write to output */
    fwrite(&(bmpFileHeaderData.bfType), sizeof(WORD), 1, outfile);
    fwrite(&(bmpFileHeaderData.bfSize), sizeof(DWORD), 1, outfile);
    fwrite(&(bmpFileHeaderData.bfReserved1), sizeof(WORD), 1, outfile);
    fwrite(&(bmpFileHeaderData.bfReserved2), sizeof(WORD), 1, outfile);
    fwrite(&(bmpFileHeaderData.bfOffBits), sizeof(DWORD), 1, outfile);
    fwrite(&bmpInfoHeaderData, bmpInfoHeaderData.biSize, 1, outfile);
    fwrite(brighterData, imageSize, 1, outfile);
    fclose(outfile);
    munmap(brighterData, imageSize);
    return 0;
}