/**
 * \file
 *	BMP functions
 *
 * \author
 *	Kadir Yanık <kdrynkk@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmp.h"
#include "log.h"

#ifndef LOG_LEVEL_CONF_BMP
#define LOG_LEVEL LOG_LEVEL_ERR
#else /* LOG_LEVEL_CONF_BMP */
#define LOG_LEVEL LOG_LEVEL_CONF_BMP
#endif /* LOG_LEVEL_CONF_BMP */

/*------------------------------------------------------------------------------*/
#define BITMAP_FILE_TYPE 0x4D42 // 'MB'

/*------------------------------------------------------------------------------*/
image_t* bmp_load(const char *filename)
{
    FILE *file = NULL;
    uint32_t buffer_size = 0;
    bitmap_file_header_t bmp_file_header;
    bitmap_info_header_t bmp_info_header;
    image_t *image =  NULL;

    LOG_DBG("filename:'%s'\n", filename);

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

    util_fite(((image = (image_t *)calloc(1, sizeof(image_t))) == NULL),
	    LOG_ERR("Image allocation failed!\n"));

    util_fite((bmp_file_header.size <= bmp_file_header.off_bits),
	    LOG_ERR("Image data offset is not valid!\n"));

    /* get bmp data size and allocate memory */
    buffer_size = bmp_file_header.size - bmp_file_header.off_bits;
    util_fite(((image->buf = (uint8_t *)malloc(buffer_size * sizeof(char))) == NULL),
	    LOG_ERR("Buffer allocation failed!\n"));

    /* move file pointer to beginning of the bmp data */
    fseek(file, bmp_file_header.off_bits, SEEK_SET);

    /* finally read bmp data */
    util_fite(((fread((char *)image->buf, buffer_size, sizeof(char), file)) < 1),
	    LOG_ERR("Read BmpData failed!\n"));

    /* set parameter values */
    image->width = bmp_info_header.width;
    image->height = abs(bmp_info_header.height);
    image->size = buffer_size;
    image->cb = bmp_info_header.bit_count / 8;

    LOG_DBG("'%s' successfully loaded!\n", filename);
    goto success;

fail:
    sfree_image(image);

success:
    if (file) fclose(file);
    return image;
}

/*------------------------------------------------------------------------------*/
int bmp_save(const char *filename, image_t image)
{
    FILE *file = NULL;
    bitmap_file_header_t bmp_file_header;
    bitmap_info_header_t bmp_info_header;
    int ret = 0, size = 0;

    LOG_DBG("filename:'%s' image:%p\n", filename, &image);

    util_fite((filename == NULL), LOG_ERR("filename is NULL!\n"));
    util_fite(((file = fopen(filename, "w")) == NULL), LOG_ERR("File open failed!\n"));

    /* initialize */
    memset(&bmp_file_header, 0, sizeof(bitmap_file_header_t));
    memset(&bmp_info_header, 0, sizeof(bitmap_info_header_t));
    size = image.width * image.height * image.cb;

    /* fill headers */
    bmp_file_header.type = BITMAP_FILE_TYPE;
    bmp_file_header.size= size + sizeof(bitmap_file_header_t) + sizeof(bitmap_info_header_t);
    bmp_file_header.off_bits = bmp_file_header.size - size;	/* 54 byte */

    /* fill info */
    bmp_info_header.size = sizeof(bitmap_info_header_t);
    bmp_info_header.width = image.width;
    bmp_info_header.height = image.height;
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
    util_fite(((fwrite(image.buf, size, sizeof(char), file)) < 1),
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
image_t* bmp_convert_to_intensity(image_t image)
{
    uint32_t row = 0, column = 0, padded_width = 0, buf_pos = 0, new_pos = 0;
    image_t *new_image = NULL;

    LOG_DBG("image:%p\n", &image);

    /* make sure the parameters are valid */
    util_fite(((image.buf == NULL) || (image.width == 0) || (image.height == 0)),
	    LOG_ERR("Parameters are not valid!\n"));

    /* find the number of padding bytes */
    padded_width = image.width * image.cb;
    while ((padded_width % (sizeof(uint32_t))) != 0) padded_width++;

    util_fite(((new_image = (image_t *)calloc(1, sizeof(image_t))) == NULL),
	    LOG_ERR("Image allocation failed\n"));
    new_image->size = image.width * image.height;

    util_fite(((new_image->buf = (uint8_t *)malloc((new_image->size) *
	    sizeof(uint8_t))) == NULL), LOG_ERR("Image data allocation failed\n"));

    /* 24-bit to 8-bit ((R+G+B) / 3) */
    for (row = 0; row < image.height; row++) {
	for (column = 0; column < image.width; column++) {
	    new_pos = row * image.width + column;
	    buf_pos = (image.height - row - 1) * padded_width + column * 3;

	    /* could be like this too (0.11 * b[+2] + 0.59 * b[+1] + 0.3 * b[0]) */
	    new_image->buf[new_pos] = (uint8_t)((image.buf[buf_pos + 2] +
			image.buf[buf_pos + 1] + image.buf[buf_pos]) / 3);
	}
    }

    new_image->width = image.width;
    new_image->height = image.height;
    new_image->cb = 1;
    goto success;

fail:
    sfree_image(new_image);

success:
    return new_image;
}

/*------------------------------------------------------------------------------*/
image_t* bmp_convert_from_intensity(image_t image)
{
    uint32_t row = 0, column = 0, padded_width = 0, buf_pos = 0, new_pos = 0;
    image_t* new_image = NULL;

    LOG_DBG("image:%p\n", &image);

    /* make sure the parameters are valid */
    util_fite(((image.buf == NULL) || (image.width == 0) || (image.height == 0)),
	    LOG_ERR("Parameters are not valid!\n"));

    /* we have to pad for the next uint32_t boundary */
    padded_width = image.width * 3;
    while ((padded_width % (sizeof(uint32_t))) != 0) padded_width++;

    util_fite(((new_image = (image_t *)calloc(1, sizeof(image_t))) == NULL),
	    LOG_ERR("Image allocation failed\n"));
    new_image->size = image.height * padded_width;

    util_fite(((new_image->buf = (uint8_t *)malloc((new_image->size) *
	    sizeof(uint8_t))) == NULL), LOG_ERR("Image data allocation failed\n"));
    new_image->cb = 3;

    // 8-bit to 24-bit, set RGB with same value
    for (row = 0; row < image.height; row++) {
	for (column = 0; column < image.width; column++) {
	    /* position in original buffer */
	    buf_pos = row * image.width + column;
	    /* position in padded buffer */
	    new_pos = (image.height - row - 1) * padded_width + column * new_image->cb;

	    new_image->buf[new_pos] = image.buf[buf_pos];	/* blue */
	    new_image->buf[new_pos + 1] = image.buf[buf_pos];	/* green */
	    new_image->buf[new_pos + 2] = image.buf[buf_pos];	/* red */
	}
    }

    new_image->width = image.width;
    new_image->height = image.height;
    goto success;

fail:
    sfree_image(new_image);

success:
    return new_image;
}

/*------------------------------------------------------------------------------*/
/*
 * bmp stores data upside down, return the rgb pixels buffer in order.
 */
image_t* bmp_convert_to_rgb(image_t image)
{
    uint32_t row = 0, column = 0, padded_width = 0, buf_pos = 0, new_pos = 0;
    image_t *new_image = NULL;

    LOG_DBG("image:%p\n", &image);

    /* make sure the parameters are valid */
    util_fite(((image.buf == NULL) || (image.width == 0) || (image.height == 0)),
	    LOG_ERR("Parameters are not valid!\n"));

    /* find the number of padding bytes */
    padded_width = image.width * image.cb;
    while ((padded_width % (sizeof(uint32_t))) != 0) padded_width++;

    util_fite(((new_image = (image_t *)calloc(1, sizeof(image_t))) == NULL),
	    LOG_ERR("Image allocation failed\n"));
    new_image->size = image.height * image.width * image.cb;

    util_fite(((new_image->buf = (uint8_t *)malloc((new_image->size) *
		sizeof(uint8_t))) == NULL), LOG_ERR("Image data allocation failed\n"));

    for (row = 0; row < image.height; row++) {
	for (column = 0; column < image.width; column++) {
	    buf_pos = (image.height - row - 1) * padded_width + column * image.cb;
	    new_pos = row * image.width * image.cb + column * image.cb;

	    /* rgb <- bgr */
	    new_image->buf[new_pos] = image.buf[buf_pos + 2];
	    new_image->buf[new_pos + 1] = image.buf[buf_pos + 1];
	    new_image->buf[new_pos + 2] = image.buf[buf_pos];
	}
    }

    new_image->width = image.width;
    new_image->height = image.height;
    new_image->cb = image.cb;
    goto success;

fail:
    sfree_image(new_image);

success:
    return new_image;
}

/*------------------------------------------------------------------------------*/
/*
 * return the upsidedown buffer.
 */
image_t* bmp_convert_from_rgb(image_t image)
{
    uint32_t row = 0, column = 0, padded_width = 0, buf_pos = 0, new_pos = 0;
    image_t* new_image = NULL;

    LOG_DBG("image:%p\n", &image);

    /* make sure the parameters are valid */
    util_fite(((image.buf == NULL) || (image.width == 0) || (image.height == 0)),
	    LOG_ERR("Parameters are not valid!\n"));

    util_fite(((new_image = (image_t *)calloc(1, sizeof(image_t))) == NULL),
	    LOG_ERR("Image allocation failed\n"));

    /* find the number of padding bytes */
    padded_width = image.width * image.cb;
    while ((padded_width % (sizeof(uint32_t))) != 0) padded_width++;
    new_image->size = image.height * padded_width;

    util_fite(((new_image->buf = (uint8_t *)malloc((new_image->size) *
		sizeof(uint8_t))) == NULL), LOG_ERR("Image data allocation failed\n"));

    for (row = 0; row < image.height; row++) {
	for (column = 0; column < image.width; column++) {
	    buf_pos = row * image.width * image.cb + column * image.cb;
	    new_pos = (image.height - row - 1) * padded_width + column * image.cb;

	    /* bgr <- rgb */
	    new_image->buf[new_pos] = image.buf[buf_pos + 2];
	    new_image->buf[new_pos + 1] = image.buf[buf_pos + 1];
	    new_image->buf[new_pos + 2] = image.buf[buf_pos];
	}
    }

    new_image->width = image.width;
    new_image->height = image.height;
    new_image->cb = image.cb;
    goto success;

fail:
    sfree_image(new_image);

success:
    return new_image;
}
/*------------------------------------------------------------------------------*/
/*     ^
 *  x,height  <- y,width ->
 *     v
 */
image_t* bmp_crop_image(image_t image, rectangle_t rect)
{
    image_t *cropped_image = NULL;
    int32_t i = 0, j = 0, k = 0;
    uint32_t buf_pos = 0, new_pos = 0;

    LOG_DBG("image:%p rect:%p\n", &image, &rect);

    if (rect.x + rect.height >= image.height || rect.y + rect.width >= image.width) {
	LOG_ERR("Parameters are not valid for this image! [w:%u, h:%u]\n", image.width, image.height);
	goto fail;
    }
    util_fite(((cropped_image = (image_t *)calloc(1, sizeof(image_t))) == NULL),
	    LOG_ERR("Image allocation failed\n"));

    cropped_image->size = rect.width * rect.height * image.cb;
    util_fite(((cropped_image->buf = (uint8_t *)malloc((cropped_image->size) *
		sizeof(uint8_t))) == NULL), LOG_ERR("Image data allocation failed\n"));

    for (i = 0; i < rect.height; i++) {
	for (j = 0; j < rect.width; j++) {
	    buf_pos = (i + rect.x) * image.width * image.cb + (j + rect.y) * image.cb;
	    new_pos = i * rect.width * image.cb + j * image.cb;

	    for (k = 0; k < image.cb; k++) {
		cropped_image->buf[new_pos + k] = image.buf[buf_pos + k];
	    }
	}
    }

    cropped_image->cb = image.cb;
    cropped_image->width = rect.width;
    cropped_image->height = rect.height;
    goto success;

fail:
    sfree_image(cropped_image);

success:
    return cropped_image;
}

