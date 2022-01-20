
    /* move ptr by offset */
    int *offset;
    fread(offset, bmpFileHeader.bfOffBits, 1, im1);
    printf("offset: %d \n", *offset);