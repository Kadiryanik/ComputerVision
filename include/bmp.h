#ifndef BMP_H_
#define BMP_H_

#include <stdio.h>
#include <stdint.h>

/*------------------------------------------------------------------------------*/
/* TODO: remove attribute packed */
typedef struct bitmap_file_header {
    uint16_t type;		/* 2: Magic identifier */
    uint32_t size;		/* 4: File size in bytes */
    uint16_t reserved_1;	/* 2 */
    uint16_t reserved_2;	/* 2 */
    uint32_t off_bits;		/* 4: Offset to image data, bytes */
} __attribute__((packed)) bitmap_file_header_t;

typedef struct bitmap_info_header {
    uint32_t size;		/* 4: Header size in bytes */
    int32_t width;		/* 4: Width of image */
    int32_t height;		/* 4: Height of image */
    uint16_t planes;		/* 2: Number of colour planes */
    uint16_t bit_count;		/* 2: Bits per pixel */
    uint32_t compression;	/* 4: Compression type */
    uint32_t size_image;	/* 4: Image size in bytes */
    int32_t x_pels_per_meter;	/* 4: Pixels per meter */
    int32_t y_pels_per_meter;	/* 4: Pixels per meter */
    uint32_t clr_used;		/* 4: Number of colours */
    uint32_t clr_important;	/* 4: Important colours */
} __attribute__((packed)) bitmap_info_header_t;

typedef struct {
    uint8_t b;
    uint8_t g;
    uint8_t r;
} rgb_data;

/*------------------------------------------------------------------------------*/
uint8_t* bmp_load(const char *filename, int *width, int *height, int *size);
int bmp_save(const char *filename, int width, int height, uint8_t *data);
uint8_t* convert_bmp_to_intensity(uint8_t *buffer, int width, int height);
uint8_t* convert_intensity_to_bmp(uint8_t *buffer, int width, int height, int *newsize);

#endif /* BMP_H_ */
