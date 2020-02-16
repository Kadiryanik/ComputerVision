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

/*--------------------------------------------------------------------------------------*/
#define HISTOGRAM_LENGTH 256
#define FIND_THE_MINIMUM_BETWEEN_T1_T2 1

#define PYTHON_PLOT_CALL "python helper/plot.py "
#define SYSTEM_CALL_BACKGROUND "&"

#define HISTOGRAM_FILE_NAME "hist.txt"

extern int plot_with_python; // defined in test.c

/*--------------------------------------------------------------------------------------*/
static int plot_histogram(const int* const histogram){
  FILE *file;
  
  // open file
  file = fopen(HISTOGRAM_FILE_NAME, "w");
  if(file == NULL){
    LOG_ERR("File open failed!\n");
    return 0;
  }
  
  // write data
  if(!fwrite(histogram, HISTOGRAM_LENGTH, sizeof(int), file)){
    LOG_ERR("File writing data failed!\n");
    fclose(file);
    return 0;
  }

  // succeed: close the file
  LOG_DBG("Writing Done!\n");
  fclose(file);

  // prepare command string
  char *plot_call = (char *)malloc(strlen(PYTHON_PLOT_CALL) + strlen(HISTOGRAM_FILE_NAME) \
    + strlen(SYSTEM_CALL_BACKGROUND) + 1); // +1 for the null-terminator
  if(plot_call != NULL){
    strcpy(plot_call, PYTHON_PLOT_CALL);
    strcat(plot_call, HISTOGRAM_FILE_NAME);
    strcat(plot_call, SYSTEM_CALL_BACKGROUND);
    LOG_DBG("Calling \"%s\"\n", plot_call);
    // call the prepared command
    system(plot_call);
    return 1;
  } else{
    LOG_ERR("Plot command buffer allocation failed!\n");
    return 0;
  }
}

/*--------------------------------------------------------------------------------------*/
static int find_threshold(const BYTE* const intensity, int width, int height){
  int i, histogram[HISTOGRAM_LENGTH];
  DWORD sum = 0;

  // clear memory
  memset(histogram, 0, HISTOGRAM_LENGTH * sizeof(int));

  for(i = 0; i < width * height; i++){
    histogram[(int)intensity[i]]++;
  }

  if(plot_with_python != 0){
    if(!plot_histogram(histogram)){
      LOG_ERR("Threshold plotting failed!\n");
    }
  }

  double total = 0;
  for(i = 0; i < HISTOGRAM_LENGTH; i++){
    sum += (i * histogram[i]);
    if(histogram[i] != 0){
      total++;
    }
  }

  int is_done = 0;
  int t1 = 0, t2 = HISTOGRAM_LENGTH - 1;
  while(!is_done){
    DWORD sum_t1 = 0, sum_t2 = 0;
    float sum_t1_i = 0, sum_t2_i = 0;

    for(i = 0; i < HISTOGRAM_LENGTH; i++){
      if(fabs(t1 - i) < fabs(t2 - i)){
        sum_t1 += i * histogram[i];
        sum_t1_i += histogram[i];
      } else{
        sum_t2 += i * histogram[i];
        sum_t2_i += histogram[i];
      }
    }

    // division zero
    if(sum_t1_i == 0){
      sum_t1_i = 1;
    }
    if(sum_t2_i == 0){
      sum_t2_i = 1;
    }

    int t1_u = sum_t1 / sum_t1_i;
    int t2_u = sum_t2 / sum_t2_i;
    float epsilon = 2;
    if((fabs(t1 - t1_u) < epsilon && fabs(t2 - t2_u) < epsilon) || \
         fabs(t1 - t2_u) < epsilon && fabs(t2 - t1_u) < epsilon){
      LOG_DBG("t1 = %d, t2 = %d\n", t1, t2);
      is_done = 1;
    } else{
      t1 = t1_u;
      t2 = t2_u;
    }
  }
#if FIND_THE_MINIMUM_BETWEEN_T1_T2
  int min = t1;
  for(i = (t1 < t2 ? t1 : t2); i < (t1 < t2 ? t2 : t1); i++){
    if(histogram[min] > histogram[i]){
      min = i;
    }
  }
  
  LOG_DBG("Threshold = %d\n", min);
  return min;
#else /* FIND_THE_MINIMUM_BETWEEN_T1_T2 */
  LOG_DBG("Threshold = %d\n", (t1 + t2) / 2);
  return (t1 + t2) / 2;
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