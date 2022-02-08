#include <stdio.h>
#include <stdlib.h>

typedef unsigned char BYTE;

typedef struct compressedformat{
    int width, height;  // width and height of the image, one byte each color, bgr
    int rowbyte_quarter[4]; // parallel algorithmns. location in bytes which exactly splits the result image after decompression into 4 equal parts
    int palettecolors;  // how many colors does the picture have?
    col colors[palettecolors];  // all participating colors of the image
}compressedformat;

typedef struct col{
    int r, g, b;    // red, green, blue, in that order. values not to exceed 255
}col;

typedef struct chunk{
    BYTE color_index;   // which of the color of the palette
    short count;    // how many pixel of the same color from color_index are continuously appearing
}chunk;