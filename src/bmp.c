#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmp.h"
#include "log.h"
#include "util.h"

#ifndef LOG_LEVEL_CONF_BMP
#define LOG_LEVEL LOG_LEVEL_ERR
#else /* LOG_LEVEL_CONF_BMP */
#define LOG_LEVEL LOG_LEVEL_CONF_BMP
#endif /* LOG_LEVEL_CONF_BMP */

/*------------------------------------------------------------------------------*/
#define BITMAP_FILE_TYPE 0x4D42 // 'MB'

/*------------------------------------------------------------------------------*/
uint8_t* bmp_load(const char *filename, int *width, int *height, int *size)
{
    FILE *file = NULL;
    uint8_t *buffer = NULL;
    int buffer_size = 0;
    bitmap_file_header_t bmp_file_header;
    bitmap_info_header_t bmp_info_header;

    util_fite((filename == NULL), LOG_ERR("filename is NULL!\n"));
    util_fite(((file = fopen(filename, "rb")) == NULL), LOG_ERR("File open failed!\n"));

    util_fite(((fread((char *)(&bmp_file_header), sizeof(bitmap_file_header_t), sizeof(char), file)) < 1),
	    LOG_ERR("Read BmpHeader failed!\n"));
    util_fite(((fread((char *)(&bmp_info_header), sizeof(bitmap_info_header_t), sizeof(char), file)) < 1),
	    LOG_ERR("Read BmpInfo failed!\n"));

    /* check bmp file type */
    util_fite((bmp_file_header.type != BITMAP_FILE_TYPE),
	    LOG_ERR("This file is not a bmp file! [0x%04X]\n", bmp_file_header.type));

    /* check bmp file is uncompressed */
    util_fite((bmp_info_header.compression != 0),
	    LOG_ERR("This file compressed! [%u]\n", bmp_info_header.compression));

    /* check bmp file is 24 bit */
    util_fite((bmp_info_header.bit_count != 24),
	    LOG_ERR("%u bit bmp not supported!\n", bmp_info_header.bit_count));

    /* get bmp data size and allocate memory */
    buffer_size = bmp_file_header.size - bmp_file_header.off_bits;
    util_fite(((buffer = (uint8_t *)malloc(buffer_size * sizeof(char))) == NULL),
	    LOG_ERR("Buffer allocation failed!\n"));

    /* move file pointer to beginning of the bmp data */
    fseek(file, bmp_file_header.off_bits, SEEK_SET);

    /* finally read bmp data */
    util_fite(((fread((char *)buffer, buffer_size, sizeof(char), file)) < 1),
	    LOG_ERR("Read BmpData failed!\n"));

    /* set parameter values */
    *width = bmp_info_header.width;
    *height = abs(bmp_info_header.height);
    *size = buffer_size;

    LOG_DBG("Reading Done!\n");
    goto success;

fail:
    sfree(buffer);

success:
    if (file) fclose(file);
    return buffer;
}

/*------------------------------------------------------------------------------*/
int bmp_save(const char *filename, int width, int height, uint8_t *data)
{
    FILE *file = NULL;
    bitmap_file_header_t bmp_file_header;
    bitmap_info_header_t bmp_info_header;
    int ret = 0, size = 0;

    util_fite((filename == NULL), LOG_ERR("filename is NULL!\n"));
    util_fite(((file = fopen(filename, "w")) == NULL), LOG_ERR("File open failed!\n"));

    /* initialize */
    memset(&bmp_file_header, 0, sizeof(bitmap_file_header_t));
    memset(&bmp_info_header, 0, sizeof(bitmap_info_header_t));
    size = width * height * 3;

    /* fill headers */
    bmp_file_header.type = BITMAP_FILE_TYPE;
    bmp_file_header.size= size + sizeof(bitmap_file_header_t) + sizeof(bitmap_info_header_t);
    bmp_file_header.off_bits = bmp_file_header.size - size;	/* 54 byte */

    /* fill info */
    bmp_info_header.size = sizeof(bitmap_info_header_t);
    bmp_info_header.width = width;
    bmp_info_header.height = height;
    bmp_info_header.planes = 1;
    bmp_info_header.bit_count = 24;			/* 24 bit per pixel */
    bmp_info_header.compression = 0;			/* BI_RGB */
    bmp_info_header.size_image = 0;
    bmp_info_header.x_pels_per_meter = 0x0ec4;		/* paint and PSP use this values */
    bmp_info_header.y_pels_per_meter = 0x0ec4;
    bmp_info_header.clr_used = 0;
    bmp_info_header.clr_important = 0;

    util_fite(((fwrite((char *)(&bmp_file_header), sizeof(bitmap_file_header_t), sizeof(char), file)) < 1),
	    LOG_ERR("File writing header failed!\n"));
    util_fite(((fwrite((char *)(&bmp_info_header), sizeof(bitmap_info_header_t), sizeof(char), file)) < 1),
	    LOG_ERR("File writing header-info failed!\n"));
    util_fite(((fwrite(data, size, sizeof(char), file)) < 1),
	    LOG_ERR("File writing data failed!\n"));

    LOG_DBG("Successfully saved into '%s'!\n", filename);
    goto success;

fail:
    ret = -1;

success:
    if (file) fclose(file);
    return ret;
}

/*------------------------------------------------------------------------------*/
uint8_t* convert_bmp_to_intensity(uint8_t *buffer, int width, int height)
{
    int row = 0, column = 0, padding = 0, scan_line_bytes = 0,
	bufpos = 0, newpos = 0, psw = 0;
    uint8_t *newbuf = NULL;

    /* make sure the parameters are valid */
    util_fite(((buffer == NULL) || (width == 0) || (height == 0)),
	    LOG_ERR("Parameters are not valid!\n"));

    /* find the number of padding bytes */
    scan_line_bytes = width * 3;
    while ((scan_line_bytes + padding) % (sizeof(uint32_t)) != 0) padding++;

    /* get the padded scanline width */
    psw = scan_line_bytes + padding;

    util_fite(((newbuf = (uint8_t *)malloc((width * height) * sizeof(uint8_t))) == NULL),
	    LOG_ERR("Memory allocation failed\n"));

    /* 24-bit to 8-bit ((R+G+B) / 3) */
    for (row = 0; row < height; row++) {
	for (column = 0; column < width; column++) {
	    newpos = row * width + column;
	    bufpos = (height - row - 1) * psw + column * 3;

	    /* could be like this too (0.11 * b[+2] + 0.59 * b[+1] + 0.3 * b[0]) */
	    newbuf[newpos] = (uint8_t)((buffer[bufpos + 2] + buffer[bufpos + 1] + buffer[bufpos]) / 3);
	}
    }

    goto success;

fail:
    sfree(newbuf);

success:
    return newbuf;
}

/*------------------------------------------------------------------------------*/
uint8_t* convert_intensity_to_bmp(uint8_t *buffer, int width, int height, int *newsize)
{
    int padding = 0, scan_line_bytes = 0, psw = 0, row = 0, column = 0;
    long bufpos = 0, newpos = 0;
    uint8_t *newbuf = NULL;

    /* make sure the parameters are valid */
    util_fite(((buffer == NULL) || (width == 0) || (height == 0)),
	    LOG_ERR("Parameters are not valid!\n"));

    /* we have to pad for the next uint32_t boundary */
    scan_line_bytes = width * 3;
    while ((scan_line_bytes + padding) % (sizeof(uint32_t)) != 0) padding++;

    /* get the padded scanline width */
    psw = scan_line_bytes + padding;

    /* set new size */
    (*newsize) = height * psw;

    util_fite(((newbuf = (uint8_t *)calloc(1, (*newsize) * sizeof(uint8_t))) == NULL),
	    LOG_ERR("Memory allocation failed!\n"));

    // 8-bit to 24-bit, set RGB with same value
    for (row = 0; row < height; row++) {
	for (column = 0; column < width; column++) {
	    bufpos = row * width + column;		    /* position in original buffer */
	    newpos = (height - row - 1) * psw + column * 3; /* position in padded buffer */

	    newbuf[newpos] = buffer[bufpos];		    /* blue */
	    newbuf[newpos + 1] = buffer[bufpos];	    /* green */
	    newbuf[newpos + 2] = buffer[bufpos];	    /* red */
	}
    }

    goto success;

fail:
    sfree(newbuf);

success:
    return newbuf;
}

