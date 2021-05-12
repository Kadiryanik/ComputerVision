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
int print_with_func_line = 0;	/* accessed by log.h */
int plot_with_python = 0;	/* accessed by computer-vision.c */

/*------------------------------------------------------------------------------*/
void usage(const char *);

/*------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    int ret = 0;
    char c = 0, *to_binary = NULL, *to_grayscale = NULL, *test_bmp = NULL;

    while ((c = getopt(argc, argv, "b:g:t:hDP")) != -1) {
	switch(c) {
	    case 'b':
		to_binary = optarg;
		break;
	    case 'g':
		to_grayscale = optarg;
		break;
	    case 't':
		test_bmp = optarg;
		break;
	    case 'D':
		print_with_func_line = 1;
		break;
	    case 'P':
		plot_with_python = 1;
		break;
	    case 'h':
	    default:
		goto fail;
	}
    }

    if (to_binary != NULL) {
	srand(time(NULL));
	(void)convert_binary(to_binary);
	goto success;
    }
    if (to_grayscale != NULL) {
	(void)convert_grayscale(to_grayscale);
	goto success;
    }
    if (test_bmp != NULL) {
	(void)test_bmp_file(test_bmp);
	goto success;
    }

fail:
    usage(argv[0]);
    ret = -1;

success:
    return ret;
}

/*------------------------------------------------------------------------------*/
void usage(const char *name)
{
    fprintf(stderr, "Usage: %s [-hD] [-g <*.bmp>] [-t <*.bmp>]\n"
		    "\t\b\bOptions with no arguments\n"
		    "\t-h\tprint usage\n"
		    "\t-D\tprint with called function name and called line\n"
		    "\t-P\tplot graphics with python\n"
		    "\t\b\bOptions with arguments\n"
		    "\t-b\tconvert given rgb image to binary image\n"
		    "\t-g\tconvert given rgb image to gray scale image\n"
		    "\t-t\ttest the given bmp file readability\n"
		    "Example:\n"
		    "\t%s -g image.bmp\n"
		    "\t%s -D -t image.bmp\n"
		    "\t%s -Dt image.bmp\n"
		    "\t%s -Pb image.bmp\n",
		    name, name, name, name, name);
}
