#include <stdio.h>
#include <stdlib.h> // srand
#include <time.h>   // time
#include <unistd.h> // getopt

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

/*------------------------------------------------------------------------------*/
int print_with_func_line = 0;	/* accessed by log.h */
int verbose_output_enabled = 0;	/* accessed by log.h */
int plot_with_python = 0;	/* accessed by util.c */

/*------------------------------------------------------------------------------*/
void usage(const char *);

/*------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    int ret = 0;
    char c = 0, *input_image = NULL;
    uint8_t option_mask = 0;

    while ((c = getopt(argc, argv, "i:tbgdvDPh")) != -1) {
	switch(c) {
	    case 'i':
		input_image = optarg;
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
		goto fail;
	}
    }

    if ((input_image == NULL) || (option_mask == 0)) {
	goto fail;
    }

#if !LOG_FEATURE_ENABLED
    if (verbose_output_enabled) {
	fprintf(stderr, "Compile by setting the LOG_FEATURE_ENABLED flag for verbose output!\n");
    }
#endif

    if (option_mask & OPT_TEST_BMP) {
	(void)test_bmp_file(input_image);
    }
    if (option_mask & OPT_BINARY) {
	srand(time(NULL));
	(void)convert_binary(input_image);
    }
    if (option_mask & OPT_GRAYSCALE) {
	(void)convert_grayscale(input_image);
    }
    if (option_mask & OPT_DRAW_TESTS) {
	plus_t plus = { .x = 60, .y = 60, .len = 40 };
	rectangle_t rect = { .x = 60, .y = 60, .width = 60, .height = 40 };
	circle_t circle = { .x = 60, .y = 60, .r = 20 };
	ellipse_t ellipse = { .x = 60, .y = 60, .a = 30, .b = 60 };
	(void)draw_tests(input_image, plus, rect, circle, ellipse);
    }
    goto success;

fail:
    usage(argv[0]);
    ret = -1;

success:
    return ret;
}

/*------------------------------------------------------------------------------*/
void usage(const char *name)
{
    fprintf(stderr, "Usage: %s [-i <*.bmp>] [-tbgdvDPh]\n"
		    "\t\b\bOptions with no arguments\n"
		    "\t-h\tprint usage\n"
		    "\t-b\tconvert input rgb image to binary image\n"
		    "\t-g\tconvert input rgb image to gray scale image\n"
		    "\t-t\ttest the input bmp file readability\n"
		    "\t-v\tenable verbose output\n"
		    "\t-D\tprint with called function name and called line\n"
		    "\t-P\tplot graphics with python\n"
		    "\t\b\bOptions with arguments\n"
		    "\t-t\ttest the given bmp file readability\n"
		    "Example:\n"
		    "\t%s -gi image.bmp\n"
		    "\t%s -Pbi image.bmp\n"
		    "\t%s -D -t -i image.bmp\n"
		    "\t%s -vPDbgdi image.bmp\n",
		    name, name, name, name, name);
}
