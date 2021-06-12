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
#define OPT_FEATURE_EXT		(0x01 << 8)

/*------------------------------------------------------------------------------*/
int print_with_func_line = 0;		    /* accessed by log.h */
int verbose_output_enabled = 0;		    /* accessed by log.h */
int plot_with_python = 0;		    /* accessed by util.c */
double fe_match_epsilon = FE_MATCH_EPSILON; /* accessed by feature-extraction.c */
uint8_t nbr_hfl = NBR_CONF_HFL;		    /* accessed by morphology.c */

/*------------------------------------------------------------------------------*/
static void _usage(const char *);
static int _safe_strtol(const char * const str, long *result);

/*------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    int ret = 0;
    char c = 0, *input_file = NULL, *test_image_file = NULL, *output_file = NULL,
	 *mask_filename = NULL, *morp = NULL, *draw_filename = NULL, *fe_type = NULL;
    uint16_t option_mask = 0;
    int8_t parser_index = 0;
    rectangle_t crop_rect = { .x = 0, .y = 0, .width = 0, .height = 0 };

    while ((c = getopt(argc, argv, "i:o:tbgRd:c:m:M:f:T:e:N:vVPh")) != -1) {
	switch(c) {
	    case 'i':
		input_file = optarg;
		break;
	    case 'o':
		output_file = optarg;
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
	    case 'f':
		option_mask |= OPT_FEATURE_EXT;
		fe_type = optarg;
		break;
	    case 'T':
		test_image_file = optarg;
		break;
	    case 'e':
		fe_match_epsilon = strtod(optarg, NULL);
		util_fite((fe_match_epsilon <= 0),
			fprintf(stderr, "-e option MUST greater than zero\n"));
		break;
	    case 'N':
		option_mask |= OPT_FEATURE_EXT;
		long l = 0;
		util_fit((_safe_strtol(optarg, &l) != 0));
		util_fite((l < 1 || l > 0x7f),
			fprintf(stderr, "-N arguments failed, please select in [1,127]\n"));
		nbr_hfl = l;
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

    if ((input_file == NULL) || (option_mask == 0)) {
	goto fail_with_usage;
    }

#if !LOG_FEATURE_ENABLED
    if (verbose_output_enabled) {
	fprintf(stderr, "Compile by setting the LOG_FEATURE_ENABLED flag for verbose output!\n");
    }
#endif
    LOG_DBG("Options parsed successfully\n");

    srand(time(NULL));

    if (option_mask & OPT_TEST_BMP) {
	util_fit((cv_test_bmp_file(input_file) != 0));
    }
    if (option_mask & OPT_BINARY) {
	util_fit((cv_convert_binary(input_file, output_file) != 0));
    }
    if (option_mask & OPT_GRAYSCALE) {
	util_fit((cv_convert_grayscale(input_file, output_file) != 0));
    }
    if (option_mask & OPT_DRAW) {
	util_fit((cv_draw(input_file, output_file, draw_filename) != 0));
    }
    if (option_mask & OPT_CROP_IMAGE) {
	util_fit((cv_crop_image(input_file, output_file, crop_rect) != 0));
    }
    if (option_mask & OPT_APPLY_MASK) {
	util_fit((cv_apply_mask(input_file, output_file, mask_filename) != 0));
    }
    if (option_mask & OPT_APPLY_MORP) {
	util_fit((cv_apply_morphology(input_file, output_file, morp) != 0));
    }
    if (option_mask & OPT_IDENTIFY_REGION) {
	util_fit((cv_identify_regions(input_file, output_file) != 0));
    }
    if (option_mask & OPT_FEATURE_EXT) {
	util_fit((cv_feature_extraction(fe_type, input_file,
			test_image_file, output_file) != 0));
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
    fprintf(stderr, "\nUsage: %s [-i <file>] [-o <file>] [-d <file>] [-c <x> <y> <width> <height>] "
		    "[-m <file>] [-M [dilation|erosion|open|close]] [-N <n>] [-f [avg|learn]] "
		    "[-T <file>] [-tbgRvVPh]\n"
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
		    "\t-i\tinput file\n"
		    "\t-o\toutput file (uses default files if not given)\n"
		    "\t-d\tdraw shapes in the given file which contain shapes\n"
		    "\t\tformat=< <shape-name <shape-details-in-order>>* EOF >\n"
		    "\t\t  for more details please check examples in the draws folder\n"
		    "\t-c\tcropping arguments\n"
		    "\t-m\tapply the mask in the given file which contain the mask\n"
		    "\t\tformat=<width height <array-members-in-order>>\n"
		    "\t\t  for more details please check examples in the masks folder\n"
		    "\t-M\tapply morphology\n"
		    "\t-N\tset neighbor (half) frame length while selecting regions\n"
		    "\t\tincreasing this will increase performance\n"
		    "\t\t  if the regions are too close to each other in image, you need to decrease\n"
		    "\t-f\tfeature extraction\n"
		    "\t\t  avg   : gets image as input file and calculate features for regions and writes calculated\n"
		    "\t\t          average to the output file\n"
		    "\t\t  learn : gets 'class & image-path' formatted input file and writes the calculated averages\n"
		    "\t\t          for classes to the output file. Check db folder for more example\n"
		    "\t\t  test  : gets 'class & features' formatted input file and test-image file with -T option.\n"
		    "\t\t          Tries to classification image content with given classes db. Marks objects with\n"
		    "\t\t          nearest class and save as image to the output file. You can use output file of 'learn'\n"
		    "\t\t          option as input of this option.\n"
		    "\t-T\ttest input image file, meanful with only '-f test' option\n"
		    "\t-e\tmatching epsilon value, meanful with only '-f test' option\n"
		    "Example:\n"
		    "\t%s -t -i image.bmp\n"
		    "\t%s -gi image.bmp\n"
		    "\t%s -vVgi image.bmp -o output.bmp\n"
		    "\t%s -Pbi image.bmp\n"
		    "\t%s -i image.bmp -d face.txt\n"
		    "\t%s -i image.bmp -c 220 210 180 250\n"
		    "\t%s -i image.bmp -m mask.txt\n"
		    "\t%s -i shape.bmp -M open\n"
		    "\t%s -N 4 -Ri shape.bmp\n"
		    "\t%s -f avg -i shape.bmp -o result.txt\n"
		    "\t%s -f learn -i class-image-db.txt\n"
		    "\t%s -f test -i features-db.txt -T mixed.bmp\n"
		    "\t%s -vVPbgi image.bmp\n",
		    name, name, name, name, name, name, name, name, name, name,
		    name, name, name, name);
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

