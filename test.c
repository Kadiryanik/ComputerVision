/**
 * \file
 *        Testing tool
 *
 * \author
 *        Kadir Yanık <kdrynkk@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <limits.h>

#include "computer-vision.h"
#include "log.h"
#include "util.h"
#include "draw.h"

#ifndef LOG_LEVEL_CONF_TEST
#define LOG_LEVEL LOG_LEVEL_ERR
#else /* LOG_LEVEL_CONF_TEST */
#define LOG_LEVEL LOG_LEVEL_CONF_TEST
#endif /* LOG_LEVEL_CONF_TEST */

/*------------------------------------------------------------------------------*/
#define OPT_TEST_BMP		(0x01 << 0)
#define OPT_BINARY		(0x01 << 1)
#define OPT_GRAYSCALE		(0x01 << 2)
#define OPT_DRAW		(0x01 << 3)
#define OPT_CROP_IMAGE		(0x01 << 4)
#define OPT_APPLY_MASK		(0x01 << 5)
#define OPT_APPLY_MORP		(0x01 << 6)
#define OPT_IDENTIFY_REGION	(0x01 << 7)

/*------------------------------------------------------------------------------*/
int print_with_func_line = 0;	/* accessed by log.h */
int verbose_output_enabled = 0;	/* accessed by log.h */
int plot_with_python = 0;	/* accessed by util.c */

/*------------------------------------------------------------------------------*/
static void _usage(const char *);
static int _safe_strtol(const char * const str, long *result);

/*------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    int ret = 0;
    char c = 0, *input_image = NULL, *output_image = NULL, *mask_filename = NULL,
	 *morp = NULL, *draw_filename = NULL;
    uint8_t option_mask = 0;
    int8_t parser_index = 0;
    rectangle_t crop_rect = { .x = 0, .y = 0, .width = 0, .height = 0 };

    while ((c = getopt(argc, argv, "i:o:tbgRd:c:m:M:vVPh")) != -1) {
	switch(c) {
	    case 'i':
		input_image = optarg;
		break;
	    case 'o':
		output_image = optarg;
		break;
	    case 't':
		option_mask |= OPT_TEST_BMP;
		break;
	    case 'b':
		option_mask |= OPT_BINARY;
		break;
	    case 'g':
		option_mask |= OPT_GRAYSCALE;
		break;
	    case 'R':
		option_mask |= OPT_IDENTIFY_REGION;
		break;
	    case 'd':
		option_mask |= OPT_DRAW;
		draw_filename = optarg;
		break;
	    case 'c':
		option_mask |= OPT_CROP_IMAGE;
		optind--;
		parser_index = -1;
		while ((++parser_index < 4) && optind < argc) {
		    long l = 0;

		    util_fit((_safe_strtol(argv[optind], &l) != 0));
		    util_fite((l < 1),
			    fprintf(stderr, "-c arguments can not be less than 1\n"));
		    if (parser_index == 0) crop_rect.x = l;
		    else if (parser_index == 1) crop_rect.y = l;
		    else if (parser_index == 2) crop_rect.width = l;
		    else crop_rect.height = l;
		    optind++;
		}
		util_fite((crop_rect.height == 0),
			fprintf(stderr, "-c option requires 4 arguments\n"));
		break;
	    case 'm':
		option_mask |= OPT_APPLY_MASK;
		mask_filename = optarg;
		break;
	    case 'M':
		option_mask |= OPT_APPLY_MORP;
		morp = optarg;
		break;
	    case 'v':
		/* opens all log levels */
		verbose_output_enabled = 1;
		break;
	    case 'V':
		/* adds function name and line number into current logs */
		print_with_func_line = 1;
		break;
	    case 'P':
		/* plot histogram array with python */
		plot_with_python = 1;
		break;
	    case 'h':
	    default:
		goto fail_with_usage;
	}
    }

    if ((input_image == NULL) || (option_mask == 0)) {
	goto fail_with_usage;
    }

#if !LOG_FEATURE_ENABLED
    if (verbose_output_enabled) {
	fprintf(stderr, "Compile by setting the LOG_FEATURE_ENABLED flag for verbose output!\n");
    }
#endif
    LOG_DBG("Options parsed successfully\n");

    if (option_mask & OPT_TEST_BMP) {
	util_fit((cv_test_bmp_file(input_image) != 0));
    }
    if (option_mask & OPT_BINARY) {
	srand(time(NULL));
	util_fit((cv_convert_binary(input_image, output_image) != 0));
    }
    if (option_mask & OPT_GRAYSCALE) {
	util_fit((cv_convert_grayscale(input_image, output_image) != 0));
    }
    if (option_mask & OPT_DRAW) {
	util_fit((cv_draw(input_image, output_image, draw_filename) != 0));
    }
    if (option_mask & OPT_CROP_IMAGE) {
	util_fit((cv_crop_image(input_image, output_image, crop_rect) != 0));
    }
    if (option_mask & OPT_APPLY_MASK) {
	util_fit((cv_apply_mask(input_image, output_image, mask_filename) != 0));
    }
    if (option_mask & OPT_APPLY_MORP) {
	util_fit((cv_apply_morphology(input_image, output_image, morp) != 0));
    }
    if (option_mask & OPT_IDENTIFY_REGION) {
	util_fit((cv_identify_regions(input_image, output_image) != 0));
    }

    goto success;

fail_with_usage:
    _usage(argv[0]);

fail:
    ret = -1;

success:
    return ret;
}

/*------------------------------------------------------------------------------*/
static void _usage(const char *name)
{
    fprintf(stderr, "\nUsage: %s [-i <*.bmp>] [-o <*.bmp>] [-d <file>] [-c <x> <y> <width> <height>] "
		    "[-m <file>] [-M [dilation|erosion|open|close]] [-tbgRvVPh]\n"
		    "\t\b\bOptions with no arguments\n"
		    "\t-t\ttest the input bmp file readability\n"
		    "\t-b\tconvert input image to binary image\n"
		    "\t-g\tconvert input image to gray scale image\n"
		    "\t-R\tconvert input image to gray scale image where regions identified with color\n"
		    "\t-v\tenable verbose output\n"
		    "\t-V\tadd function name and line into current log level\n"
		    "\t-P\tplot graphics with python\n"
		    "\t-h\tprint usage\n"
		    "\t\b\bOptions with arguments\n"
		    "\t-i\tinput image\n"
		    "\t-o\toutput image (uses default files if not given)\n"
		    "\t-d\tdraw shapes in the given file which contain shapes\n"
		    "\t\tformat=< <shape-name <shape-details-in-order>>* EOF >\n"
		    "\t\t  for more details please check examples in the draws folder\n"
		    "\t-c\tcropping arguments\n"
		    "\t-m\tapply the mask in the given file which contain the mask\n"
		    "\t\tformat=<width height <array-members-in-order>>\n"
		    "\t\t  for more details please check examples in the masks folder\n"
		    "\t-M\tapply morphology\n"
		    "Example:\n"
		    "\t%s -t -i image.bmp\n"
		    "\t%s -gi image.bmp\n"
		    "\t%s -vVgi image.bmp -o output.bmp\n"
		    "\t%s -Pbi image.bmp\n"
		    "\t%s -i image.bmp -d face.txt\n"
		    "\t%s -i image.bmp -c 220 210 180 250\n"
		    "\t%s -i image.bmp -m mask.txt\n"
		    "\t%s -i shape.bmp -M open\n"
		    "\t%s -vVPbgRi image.bmp\n",
		    name, name, name, name, name, name, name, name, name, name);
}

/*------------------------------------------------------------------------------*/
static int _safe_strtol(const char * const str, long *result)
{
    int ret = 0;
    char *endptr = NULL;

    errno = 0;
    *result = strtol(str, &endptr, 10);
    util_fite((str == endptr),
	    fprintf(stderr, "'%s' invalid value!\n", str));
    util_fite((errno == ERANGE && (*result == LONG_MAX || *result == LONG_MIN)),
	    fprintf(stderr, "'%s' out of range!\n", str));

    goto success;

fail:
    ret = -1;

success:
    return ret;
}

