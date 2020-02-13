#include <stdio.h>
#include <stdlib.h> // exit
#include <unistd.h> // getopt

/*------------------------------------------------------------------------------*/
#include "computer-vision.h"
#include "log.h"

#define LOG_LEVEL LOG_LEVEL_INFO

/*------------------------------------------------------------------------------*/
int print_with_func_line = 0; // accessed by log.h

/*------------------------------------------------------------------------------*/
void usage(const char *);

/*------------------------------------------------------------------------------*/
int main(int argc, char *argv[]){
  int c;

  if(argc < 2){
    usage(argv[0]);
    exit(1);
  }

  while((c = getopt(argc, argv, "t:hD")) != -1){
    switch(c){
    case 't':
      LOG_PRINT_("Trying to read \"%s\" file!\n", optarg);
      char *filename = optarg;
      int width, height, size;
      BYTE *buffer;

      buffer = bmp_load(filename, &width, &height, &size);
      if(buffer != NULL){
        LOG_PRINT_("File read successfully!\n");
        free(buffer);
      } else{
        LOG_PRINT_("File read failed!\n");
      }
      return 0;
    case 'D':
      print_with_func_line = 1;
      break;
    case 'h':
    default:
      usage(argv[0]);
      exit(1);
      break;
    }
  }

  return 0;
}

/*------------------------------------------------------------------------------*/
void usage(const char *name){
  LOG_PRINT_("Usage: %s [-hD] [-t <*.bmp>]\n", name);
  LOG_PRINT_("\t\b\bOptions with no arguments\n");
  LOG_PRINT_("\t-h\tprint usage\n");
  LOG_PRINT_("\t-D\tprint with called function name and called line\n");
  LOG_PRINT_("\t\b\bOptions with arguments\n");
  LOG_PRINT_("\t-t\ttest the given bmp file readability\n");
  LOG_PRINT_("Example:\n");
  LOG_PRINT_("\t%s -t image.bmp\n", name);
}
