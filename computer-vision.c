#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "computer-vision.h"
#include "log.h"

#ifndef LOG_LEVEL_CONF_CV
#define LOG_LEVEL LOG_LEVEL_INFO
#else /* LOG_LEVEL_CONF_CV */
#define LOG_LEVEL LOG_LEVEL_CONF_CV
#endif /* LOG_LEVEL_CONF_CV */

#define HISTOGRAM_LENGTH 256
#define FIND_THE_MINIMUM_BETWEEN_T1_T2 1

/*--------------------------------------------------------------------------------------*/
static int find_threshold(const BYTE* const intensity, int width, int height){
  int i, histogram[HISTOGRAM_LENGTH];
  DWORD sum = 0;

  // clear memory
  memset(histogram, 0, HISTOGRAM_LENGTH * sizeof(int));

  for(i = 0; i < width * height; i++){
    histogram[(int)intensity[i]]++;
  }

  double total = 0;
  for(i = 0; i < HISTOGRAM_LENGTH; i++){
    sum += (i * histogram[i]);
    if(histogram[i] != 0){
      total++;
    }
  }

  int is_done = 0;
  int T1 = 0, T2 = HISTOGRAM_LENGTH - 1;
  while(!is_done){
    DWORD sumT1 = 0, sumT2 = 0;
    float sumT1i = 0, sumT2i = 0;

    for(i = 0; i < HISTOGRAM_LENGTH; i++){
      if(fabs(T1 - i) < fabs(T2 - i)){
        sumT1 += i * histogram[i];
        sumT1i += histogram[i];
      } else{
        sumT2 += i * histogram[i];
        sumT2i += histogram[i];
      }
    }

    // division zero
    if(sumT1i == 0){
      sumT1i = 1;
    }
    if(sumT2i == 0){
      sumT2i = 1;
    }

    int T1u = sumT1 / sumT1i;
    int T2u = sumT2 / sumT2i;
    float epsilon = 2;
    if((fabs(T1 - T1u) < epsilon && fabs(T2 - T2u) < epsilon) || \
         fabs(T1 - T2u) < epsilon && fabs(T2 - T1u) < epsilon){
      LOG_DBG("T1 = %d, T2 = %d\n", T1, T2);
      is_done = 1;
    } else{
      T1 = T1u;
      T2 = T2u;
    }
  }
#if FIND_THE_MINIMUM_BETWEEN_T1_T2
  int min = T1;
  for(i = (T1 < T2 ? T1 : T2); i < (T1 < T2 ? T2 : T1); i++){
    if(histogram[min] > histogram[i]){
      min = i;
    }
  }
  
  LOG_DBG("Threshold = %d\n", min);
  return min;
#else /* FIND_THE_MINIMUM_BETWEEN_T1_T2 */
  LOG_DBG("Threshold = %d\n", (T1 + T2) / 2);
  return (T1 + T2) / 2;
#endif /* FIND_THE_MINIMUM_BETWEEN_T1_T2 */
}

/*------------------------------------------------------------------------------*/
void convert_binary(const char *filename){
  LOG_DBG("Trying to convert \"%s\" to binary!\n", filename);
  int width, height, size;
  BYTE *buffer;

  buffer = bmp_load(filename, &width, &height, &size);
  if(buffer != NULL){
    BYTE *intensity = convert_bmp_to_intensity(buffer, width, height);
    if(intensity != NULL){
      BYTE* binary_image = (BYTE *)malloc((width * height) * sizeof(BYTE));

      if(binary_image != NULL){
        int i, threshold = find_threshold(intensity, width, height);
        for(i = 0; i < width * height; i++){
          binary_image[i] = intensity[i] > threshold ? 255 : 0;
        }

        BYTE *binary_image_bmp = convert_intensity_to_bmp(binary_image, width, height, &size);
        if(binary_image_bmp != NULL){
          if(bmp_save(BINARY_SCALE_IMAGE_PATH, width, height, binary_image_bmp)){
            LOG_INFO("%s succesfully saved!\n", BINARY_SCALE_IMAGE_PATH);
          } else{
            LOG_ERR("%s save failed!\n", BINARY_SCALE_IMAGE_PATH);
          }
          free(binary_image_bmp);
        } else{
          LOG_ERR("Intensity to bmp failed!\n");
        }
        free(binary_image);
      } else{
        LOG_ERR("Binary-image buffer allocation failed!\n");
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