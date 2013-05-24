#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "libthumb.h"

#define HEADER_BYTES 12

const unsigned char ORIENTATION[] = {0x01, 0x12};
const unsigned char JPEG_INTERCHANGE_FORMAT[] = {0x02, 0x01};
const unsigned char JPEG_INTERCHANGE_FORMAT_LENGTH[] = {0x02, 0x02};
  
THUMB_DATA *thumb_read(char *path)
{
  THUMB_DATA *thumb;
  FILE *fp;
  unsigned char *buffer;
  bool is_little_endian, is_valid;
  unsigned long i, offset = 0, count = 0;

  if((fp = fopen(path, "rb")) == NULL)
    return NULL;

  if((buffer = malloc(sizeof(unsigned char) * 6)) == NULL) {
    fclose(fp);
    return NULL;
  }

  if((thumb = malloc(sizeof(THUMB_DATA))) == NULL) {
    free(buffer);
    fclose(fp);
    return NULL;
  }

  //  Initialize
  is_valid = true;    //  assume it is valid
  is_little_endian = NULL;
  thumb->data = NULL;
  thumb->orientation = 0;
  thumb->length = 0;

  //  EXIF Header
  read_exif_header(fp, buffer, &is_valid, &is_little_endian);

  //  0th IFD Entry Count
  if(is_valid) {
    count = todecimal(buffer, 2);
  }

  //  0th IFD Entry
  for(i = 0; i < count; i++) {
    //  Read Tag
    vread(buffer, 2, fp, &is_valid);
    vrev(buffer, 2, &is_valid, &is_little_endian);

    //  Orientation
    if(is_valid && memcmp(ORIENTATION, buffer, 2) == 0){
      vseek(fp, 6, SEEK_CUR, &is_valid);  //  Skip (Type, Count)
      vread(buffer, 4, fp, &is_valid);
      vrev(buffer, 2, &is_valid, &is_little_endian);
      
      if(is_valid) {
        thumb->orientation = (short)todecimal(buffer, 2);
        break;
      }
    } else 
      vseek(fp, 10, SEEK_CUR, &is_valid);   //  Skip (Type, Count, Offset)
  }

  //  Next IFD Pointer
  if((i = count - (i + 1)) > 0)
    vseek(fp, 12 * i, SEEK_CUR, &is_valid);

  vread(buffer, 4, fp, &is_valid);
  vrev(buffer, 4, &is_valid, &is_little_endian);

  //  1st IFD
  vseek(fp, todecimal(buffer, 4) + HEADER_BYTES, SEEK_SET, &is_valid);

  //  1st IFD Entry Count
  vread(buffer, 2, fp, &is_valid);
  vrev(buffer, 2, &is_valid, &is_little_endian);

  if(is_valid)
    count = todecimal(buffer, 2);

  //  1st IFD Entry
  for(i = 0; is_valid && i < count; i++) {
    //  Read Tag
    vread(buffer, 2, fp, &is_valid);
    vrev(buffer, 2, &is_valid, &is_little_endian);

    //  JPEG Interchange Format Offset
    if(is_valid && memcmp(JPEG_INTERCHANGE_FORMAT, buffer, 2) == 0) {
      vseek(fp, 6, SEEK_CUR, &is_valid);  //  Skip (Type, Count)
      vread(buffer, 4, fp, &is_valid);
      vrev(buffer, 4, &is_valid, &is_little_endian);

      if(is_valid)
        offset = todecimal(buffer, 4);
    } else if(is_valid && memcmp(JPEG_INTERCHANGE_FORMAT_LENGTH, buffer, 2) == 0) {
      //  JPEG Interchange Format Length
      vseek(fp, 6, SEEK_CUR, &is_valid);  //  Skip (Type, Count)
      vread(buffer, 4, fp, &is_valid);
      vrev(buffer, 4, &is_valid, &is_little_endian);

      if(is_valid)
        thumb->length  = todecimal(buffer, 4);
    } else
      vseek(fp, 10, SEEK_CUR, &is_valid);   //  Skip (Type, Count, Offset)

    if(offset > 0 && thumb->length > 0)
      break;
  }

  //  JPEG Interchange Format
  if(is_valid && offset > 0 && thumb->length > 0) {
    vseek(fp, offset + HEADER_BYTES, SEEK_SET, &is_valid);
    
    if(is_valid && (thumb->data = malloc(thumb->length * sizeof(unsigned char))) != NULL)
      vread(thumb->data, thumb->length, fp, &is_valid);
  }

  if(fp != NULL)
    fclose(fp);

  free(buffer);
  buffer = NULL;

  return thumb;
}

void thumb_free(THUMB_DATA *thumb)
{
  if(thumb == NULL)
    return;

  if(thumb->data != NULL) {
    free(thumb->data);
    thumb->data = NULL;
  }

  free(thumb);
  thumb = NULL;
}
