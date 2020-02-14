#include <stdlib.h>
#include <string.h>

#include "bmp.h"
#include "log.h"

#ifndef LOG_LEVEL_CONF_BMP
#define LOG_LEVEL LOG_LEVEL_INFO
#else /* LOG_LEVEL_CONF_BMP */
#define LOG_LEVEL LOG_LEVEL_CONF_BMP
#endif /* LOG_LEVEL_CONF_BMP */

/*------------------------------------------------------------------------------*/
#define BITMAP_FILE_TYPE 0x4D42 // 'MB'

/*------------------------------------------------------------------------------*/
BYTE* bmp_load(const char *filename, int *width, int *height, int *size){
  FILE *file;
  BITMAPFILEHEADER bmpHead;
  BITMAPINFOHEADER bmpInfo;
  BYTE *buffer;

  // check file name
  if(filename == NULL){
    LOG_ERR("filename is NULL!\n");
    return NULL;
  }

  // open file
  file = fopen(filename, "rb");
  if(file == NULL){
    LOG_ERR("File open failed!\n");
    return NULL;
  }

  // read bmp header
  if(!fread((char *)(&bmpHead), sizeof(BITMAPFILEHEADER), sizeof(char), file)){
    LOG_ERR("Reading BmpHeader!\n");
    fclose(file);
    return NULL;
  }

  // read bmp info
  if(!fread((char *)(&bmpInfo), sizeof(BITMAPINFOHEADER), sizeof(char), file)){
    LOG_ERR("Reading BmpInfo!\n");
    fclose(file);
    return NULL;
  }

  // check file type
  if(bmpHead.bfType != BITMAP_FILE_TYPE){
    LOG_ERR("This file is not a bmp file! [0x%04X]\n", bmpHead.bfType);
    fclose(file);
    return NULL;
  }

  // get image measurements
  *width = bmpInfo.biWidth;
  *height = abs(bmpInfo.biHeight);

  // check bmp file is uncompressed
  if(bmpInfo.biCompression != 0){
    LOG_ERR("This file compressed! [%u]\n", bmpInfo.biCompression);
    fclose(file);
    return NULL;
  }

  // check is 24 bit bmp
  if(bmpInfo.biBitCount != 24) {
    LOG_ERR("%u bit bmp not supported!\n", bmpInfo.biBitCount);
    fclose(file);
    return NULL;
  }

  // create buffer for data
  *size = bmpHead.bfSize - bmpHead.bfOffBits;
  buffer = (char *)malloc((*size) * sizeof(char));
  
  if(buffer == NULL){
    LOG_ERR("Buffer allocation failed!\n");
    return NULL;
  }

  // move file pointer to beginning of the bmp data
  fseek(file, bmpHead.bfOffBits, SEEK_SET);

  // read bmp data
  if(!fread((char *)(buffer), *size, sizeof(char), file)){
    LOG_ERR("Reading Data!\n");
    free(buffer);
    fclose(file);
    return NULL;
  }

  // succeed: close file and return buffer
  LOG_DBG("Reading Done!\n");
  fclose(file);
  return buffer;
}

/*------------------------------------------------------------------------------*/
int bmp_save(const char *filename, int width, int height, BYTE *data){
  FILE *file;
  BITMAPFILEHEADER bmpHead;
  BITMAPINFOHEADER bmpInfo;
  int size = width * height * 3;

  // andinitialize them to zero
  memset(&bmpHead, 0, sizeof(BITMAPFILEHEADER));
  memset(&bmpInfo, 0, sizeof(BITMAPINFOHEADER));

  // fill headers
  bmpHead.bfType = BITMAP_FILE_TYPE;
  bmpHead.bfSize= size + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
  bmpHead.bfReserved1 = bmpHead.bfReserved2 = 0;
  bmpHead.bfOffBits = bmpHead.bfSize - size; // 54 byte

  // fill info
  bmpInfo.biSize = sizeof(BITMAPINFOHEADER);
  bmpInfo.biWidth = width;
  bmpInfo.biHeight = height;
  bmpInfo.biPlanes = 1;
  bmpInfo.biBitCount = 24;            // 24 bit per pixel
  bmpInfo.biCompression = 0;          // BI_RGB
  bmpInfo.biSizeImage = 0;
  bmpInfo.biXPelsPerMeter = 0x0ec4;   // paint and PSP use this values
  bmpInfo.biYPelsPerMeter = 0x0ec4;
  bmpInfo.biClrUsed = 0;
  bmpInfo.biClrImportant = 0;

  // check file name
  if(filename == NULL){
    LOG_ERR("filename is NULL!\n");
    return 0;
  }

  // open file
  file = fopen(filename, "w");
  if(file == NULL){
    LOG_ERR("File open failed!\n");
    return 0;
  }

  // write header
  if(!fwrite((char *)(&bmpHead), sizeof(BITMAPFILEHEADER), sizeof(char), file)){
    LOG_ERR("File writing header failed!\n");
    fclose(file);
    return 0;
  }

  // write header-info
  if(!fwrite((char *)(&bmpInfo), sizeof(BITMAPINFOHEADER), sizeof(char), file)){
    LOG_ERR("File writing header-info failed!\n");
    fclose(file);
    return 0;
  }
  
  // write data
  if(!fwrite(data, size, sizeof(char), file)){
    LOG_ERR("File writing data failed!\n");
    fclose(file);
    return 0;
  }

  // succeed: close file and return
  LOG_DBG("Writing Done!\n");
  fclose(file);
  return 1;
}

/*------------------------------------------------------------------------------*/
BYTE* convert_bmp_to_intensity(BYTE *buffer, int width, int height){
  // make sure the parameters are valid
  if((buffer == NULL) || (width == 0) || (height == 0)){
    LOG_ERR("Parameters are not valid!\n");
    return NULL;
  }

  // find the number of padding bytes
  int padding = 0;
  int scanlinebytes = width * 3;
  while((scanlinebytes + padding) % (sizeof(DWORD)) != 0){
    padding++;
  }

  // get the padded scanline width
  int psw = scanlinebytes + padding;

  // create new buffer
  BYTE *newbuf = (BYTE *)malloc((width * height) * sizeof(BYTE));

  // 24-bit to 8-bit, RGB -> ((R+G+B) / 3)
  int bufpos = 0;
  int newpos = 0;
  for(int row = 0; row < height; row++){
    for(int column = 0; column < width; column++){
      newpos = row * width + column;
      bufpos = (height - row - 1) * psw + column * 3;
      
      // could be like this too (0.11 * b[+2] + 0.59 * b[+1] + 0.3 * b[0])
      newbuf[newpos] = (BYTE)((buffer[bufpos + 2] + buffer[bufpos + 1] + buffer[bufpos]) / 3);
    }
  }
  
  return newbuf;
}

/*------------------------------------------------------------------------------*/
BYTE* convert_intensity_to_bmp(BYTE *buffer, int width, int height, int *newsize){
  // make sure the parameters are valid
  if((buffer == NULL) || (width == 0) || (height == 0)){
    LOG_ERR("Parameters are not valid!\n");
    return NULL;
  }

  // we have to pad for the next DWORD boundary
  int padding = 0;
  int scanlinebytes = width * 3;
  while((scanlinebytes + padding) % (sizeof(DWORD)) != 0){
    padding++;
  }

  // get the padded scanline width
  int psw = scanlinebytes + padding;

  // get new size
  *newsize = height * psw;

  // allocate new buf
  BYTE *newbuf = (BYTE *)malloc((*newsize) * sizeof(BYTE));

  // clear memory
  memset(newbuf, 0, *newsize);

  // 8-bit to 24-bit, set RGB with same value
  long bufpos = 0;
  long newpos = 0;
  for(int row = 0; row < height; row++){
    for(int column = 0; column < width; column++){
      bufpos = row * width + column;                  // position in original buffer
      newpos = (height - row - 1) * psw + column * 3; // position in padded buffer

      newbuf[newpos] = buffer[bufpos];                // blue
      newbuf[newpos + 1] = buffer[bufpos];            // green
      newbuf[newpos + 2] = buffer[bufpos];            // red
    }
  }
  return newbuf;
}