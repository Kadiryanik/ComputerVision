#include <stdio.h>
#include <stdlib.h> // exit
#include <unistd.h> // getopt

#include "computer-vision.h"
#include "log.h"

#ifndef LOG_LEVEL_CONF_TEST
#define LOG_LEVEL LOG_LEVEL_INFO
#else /* LOG_LEVEL_CONF_TEST */
#define LOG_LEVEL LOG_LEVEL_CONF_TEST
#endif /* LOG_LEVEL_CONF_TEST */

/*------------------------------------------------------------------------------*/
int print_with_func_line = 0; // accessed by log.h
int plot_with_python = 0; // accessed by computer-vision.c

/*------------------------------------------------------------------------------*/
void usage(const char *);

/*------------------------------------------------------------------------------*/
int main(int argc, char *argv[]){
  int c;
  char *to_binary = NULL;
  char *to_grayscale = NULL;
  char *test_bmp = NULL;

  while((c = getopt(argc, argv, "b:g:t:hDP")) != -1){
    switch(c){
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
        usage(argv[0]);
        return 1;
    }
  }

  LOG_DBG("option parse done!\n");

  if(to_binary != NULL){
    convert_binary(to_binary);
    return 0;
  }
  if(to_grayscale != NULL){
    convert_grayscale(to_grayscale);
    return 0;
  }
  if(test_bmp != NULL){
    test_bmp_file(test_bmp);
    return 0;
  }

  // if we reach here mean something wrong print usage
  usage(argv[0]);
  return 1;
}

/*------------------------------------------------------------------------------*/
void usage(const char *name){
  LOG_PRINT_("Usage: %s [-hD] [-g <*.bmp>] [-t <*.bmp>]\n", name);
  LOG_PRINT_("\t\b\bOptions with no arguments\n");
  LOG_PRINT_("\t-h\tprint usage\n");
  LOG_PRINT_("\t-D\tprint with called function name and called line\n");
  LOG_PRINT_("\t-P\tplot graphics with python\n");
  LOG_PRINT_("\t\b\bOptions with arguments\n");
  LOG_PRINT_("\t-b\tconvert given rgb image to binary image\n");
  LOG_PRINT_("\t-g\tconvert given rgb image to gray scale image\n");
  LOG_PRINT_("\t-t\ttest the given bmp file readability\n");
  LOG_PRINT_("Example:\n");
  LOG_PRINT_("\t%s -g image.bmp\n", name);
  LOG_PRINT_("\t%s -D -t image.bmp\n", name);
  LOG_PRINT_("\t%s -Dt image.bmp\n", name);
  LOG_PRINT_("\t%s -Pb image.bmp\n", name);
}
