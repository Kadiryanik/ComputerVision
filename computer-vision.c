#include <stdlib.h>

#include "computer-vision.h"
#include "log.h"

#ifndef LOG_LEVEL_CONF_CV
#define LOG_LEVEL LOG_LEVEL_INFO
#else /* LOG_LEVEL_CONF_CV */
#define LOG_LEVEL LOG_LEVEL_CONF_CV
#endif /* LOG_LEVEL_CONF_CV */

/*------------------------------------------------------------------------------*/
void convert_grayscale(const char *filename){
  LOG_DBG("Trying to convert \"%s\" to gray scale!\n", filename);
  int width, height, size;
  BYTE *buffer;

  buffer = bmp_load(filename, &width, &height, &size);
  if(buffer != NULL){
    BYTE *intensity = convert_bmp_to_intensity(buffer, width, height);
    if(intensity != NULL){
      BYTE *gray_scale_bmp_data = convert_intensity_to_bmp(intensity, width, height, &size);
      if(gray_scale_bmp_data != NULL){
        if(bmp_save(GRAY_SCALE_IMAGE_PATH, width, height, gray_scale_bmp_data)){
          LOG_INFO("%s succesfully saved!\n", GRAY_SCALE_IMAGE_PATH);
        } else{
          LOG_ERR("%s save failed!\n", GRAY_SCALE_IMAGE_PATH);
        }
        free(gray_scale_bmp_data);
      } else{
        LOG_ERR("Intensity to bmp failed!\n");
      }
      free(intensity);
    } else{
      LOG_ERR("Bmp to intensity failed!\n");
    }

    free(buffer);
  } else{
    LOG_ERR("File load failed!\n");
  }
}

/*------------------------------------------------------------------------------*/
void test_bmp_file(const char *filename){
  LOG_PRINT_("Trying to load \"%s\"\n", filename);
  int width, height, size;
  BYTE *buffer;

  buffer = bmp_load(filename, &width, &height, &size);
  if(buffer != NULL){
    LOG_PRINT_("File load succeed!\n");
    free(buffer);
  } else{
    LOG_PRINT_("File load failed!\n");
  }
}