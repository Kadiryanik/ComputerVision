#ifndef COMPUTER_VISION_H_
#define COMPUTER_VISION_H_

/*------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*------------------------------------------------------------------------------*/
typedef unsigned char BYTE;
typedef unsigned int DWORD;
typedef unsigned short WORD;

/* TODO: intel dışında işlemciyle çalışılcaksa parser creater yazılmalı. */
typedef struct tagBITMAPFILEHEADER{
    WORD    bfType;             // 2 /* Magic identifier */
    DWORD   bfSize;             // 4 /* File size in bytes */
    WORD    bfReserved1;        // 2
    WORD    bfReserved2;        // 2
    DWORD   bfOffBits;          // 4 /* Offset to image data, bytes */
} __attribute__((packed)) BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER{
    DWORD    biSize;            // 4 /* Header size in bytes */
    int      biWidth;           // 4 /* Width of image */
    int      biHeight;          // 4 /* Height of image */
    WORD     biPlanes;          // 2 /* Number of colour planes */
    WORD     biBitCount;        // 2 /* Bits per pixel */
    DWORD    biCompression;     // 4 /* Compression type */
    DWORD    biSizeImage;       // 4 /* Image size in bytes */
    int      biXPelsPerMeter;   // 4 /* Pixels per meter */
    int      biYPelsPerMeter;   // 4 /* Pixels per meter */
    DWORD    biClrUsed;         // 4 /* Number of colours */
    DWORD    biClrImportant;    // 4 /* Important colours */
} __attribute__((packed)) BITMAPINFOHEADER;

typedef struct{
    BYTE    b;
    BYTE    g;
    BYTE    r;
} RGB_data;

/*------------------------------------------------------------------------------*/
BYTE* bmp_load(const char *filename, int *width, int *height, int *size);
int bmp_save(const char *filename, int width, int height, BYTE *data);

/*------------------------------------------------------------------------------*/

#endif /* COMPUTER_VISION_H_ */
