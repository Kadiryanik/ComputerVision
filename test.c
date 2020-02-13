#include <stdio.h>
#include <stdlib.h> // exit
#include <unistd.h> // getopt

/*------------------------------------------------------------------------------*/
#include "computer-vision.h"
#include "log.h"

#define LOG_LEVEL LOG_LEVEL_INFO

/*------------------------------------------------------------------------------*/
void usage(const char *);

/*------------------------------------------------------------------------------*/
int main(int argc, char *argv[]){
  int c;
  
  if(argc < 2){
      usage(argv[0]);
      exit(1);
  }

  while((c = getopt(argc, argv, "h")) != -1){
    switch(c){
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
  LOG_PRINT("Usage: %s [-h]\n", name);
}
