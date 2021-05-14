#include <stdio.h>
#include <stdlib.h> // srand
#include <time.h>   // time
#include <unistd.h> // getopt
#include <string.h> // strstr
#include <errno.h>
#include <limits.h>

#include "computer-vision.h"
#include "log.h"
#include "util.h"

#ifndef LOG_LEVEL_CONF_TEST
#define LOG_LEVEL LOG_LEVEL_ERR
#else /* LOG_LEVEL_CONF_TEST */
#define LOG_LEVEL LOG_LEVEL_CONF_TEST
#endif /* LOG_LEVEL_CONF_TEST */

/*------------------------------------------------------------------------------*/
#define OPT_TEST_BMP	(0x01 << 0)
#define OPT_BINARY	(0x01 << 1)
#define OPT_GRAYSCALE	(0x01 << 2)
#define OPT_DRAW_TESTS	(0x01 << 3)
#define OPT_CROP_TESTS	(0x01 << 4)

/*------------------------------------------------------------------------------*/
int print_with_func_line = 0;	/* accessed by log.h */
int verbose_output_enabled = 0;	/* accessed by log.h */
int plot_with_python = 0;	/* accessed by util.c */

/*------------------------------------------------------------------------------*/
void usage(const char *);
int safe_strtol(const char * const str, long *result);

/*------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    int ret = 0;
    char c = 0, *input_image = NULL, *output_image = NULL;
    uint8_t option_mask = 0;
    int8_t parser_index = 0;
    rectangle_t crop_rect = { .x = 0, .y = 0, .width = 0, .height = 0 };

    while ((c = getopt(argc, argv, "i:o:tbgdc:vDPh")) != -1) {
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
	    case 'd':
		option_mask |= OPT_DRAW_TESTS;
		break;
	    case 'c':
		option_mask |= OPT_CROP_TESTS;
		optind--;
		parser_index = -1;
		while ((++parser_index < 4) && optind < argc) {
		    long l = 0;

		    util_fit((safe_strtol(argv[optind], &l) != 0));
		    util_fite((l < 1),
			    fprintf(stderr, "-c arguments can not be less than 1\n"));
		    if (parser_index == 0) crop_rect.x = l;
		    else if (parser_index == 1) crop_rect.y = l;
		    else if (parser_index == 2) crop_rect.width = l;
		    else crop_rect.height = l;
		    optind++;
		}
		util_fite((crop_rect.height == 0),
			fprintf(stderr, "-c must take 4 arguments at least\n"));
		break;
	    case 'v':
		/* opens all log levels */
		verbose_output_enabled = 1;
		break;
	    case 'D':
		/* adds line number current logs */
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

    if (option_mask & OPT_TEST_BMP) {
	util_fit((test_bmp_file(input_image) != 0));
    }
    if (option_mask & OPT_BINARY) {
	srand(time(NULL));
	util_fit((convert_binary(input_image, output_image) != 0));
    }
    if (option_mask & OPT_GRAYSCALE) {
	util_fit((convert_grayscale(input_image, output_image) != 0));
    }
    if (option_mask & OPT_DRAW_TESTS) {
	plus_t plus = { .x = 60, .y = 60, .len = 40 };
	rectangle_t rect = { .x = 60, .y = 60, .width = 60, .height = 40 };
	circle_t circle = { .x = 60, .y = 60, .r = 20 };
	ellipse_t ellipse = { .x = 60, .y = 60, .a = 30, .b = 60 };
	util_fit((draw_tests(input_image, output_image, plus, rect, circle, ellipse) != 0));
    }
    if (option_mask & OPT_CROP_TESTS) {
	util_fit((crop_image(input_image, output_image, crop_rect) != 0));
    }
    goto success;

fail_with_usage:
    usage(argv[0]);

fail:
    ret = -1;

success:
    return ret;
}

/*------------------------------------------------------------------------------*/
void usage(const char *name)
{
    fprintf(stderr, "\nUsage: %s [-i <*.bmp>] [-o <*.bmp>] [-c <x> <y> <width> <height>] [-tbgdvDPh]\n"
		    "\t\b\bOptions with no arguments\n"
		    "\t-t\ttest the input bmp file readability\n"
		    "\t-b\tconvert input rgb image to binary image\n"
		    "\t-g\tconvert input rgb image to gray scale image\n"
		    "\t-d\ttest draw capability with input image\n"
		    "\t-v\tenable verbose output\n"
		    "\t-D\tprint with called function name and called line\n"
		    "\t-P\tplot graphics with python\n"
		    "\t-h\tprint usage\n"
		    "\t\b\bOptions with arguments\n"
		    "\t-i\tinput image\n"
		    "\t-o\toutput image (uses default if not given)\n"
		    "\t-c\tcropping arguments\n"
		    "Example:\n"
		    "\t%s -gi image.bmp\n"
		    "\t%s -gi image.bmp -o output.bmp\n"
		    "\t%s -Pbi image.bmp\n"
		    "\t%s -D -t -i image.bmp\n"
		    "\t%s -vi image.bmp -c 220 210 180 250\n"
		    "\t%s -vPDbgdi image.bmp\n",
		    name, name, name, name, name, name, name);
}

/*------------------------------------------------------------------------------*/
int safe_strtol(const char * const str, long *result)
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
