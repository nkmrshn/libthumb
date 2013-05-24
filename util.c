#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "util.h"

const unsigned char SOI[] = {0xff, 0xd8};
const unsigned char APP1_MARKER[] = {0xff, 0xe1};
const unsigned char EXIF_IDENT_CODE[] = {0x45, 0x78, 0x69, 0x66, 0x00, 0x00};
const unsigned char EXIF_FIXED_CODE[] = {0x00, 0x2a};
const unsigned char TIFF_LITTLE_ENDIAN[] = {0x49, 0x49};
const unsigned char TIFF_BIG_ENDIAN[] = {0x4d, 0x4d};

unsigned long todecimal(unsigned char *str, size_t s)
{
  size_t i;
  unsigned long x = 0;

  for(i = 0; i < s; i++) {
    x += str[i] * (unsigned long)pow(256, s - i - 1);
  }

  return x;
}

int strrev(unsigned char *str, size_t s)
{
  size_t i;
  unsigned char *tmp;
  
  if((tmp = malloc(sizeof(unsigned char *) * s)) == NULL)
    return 1;
  
  for(i = 0; i < s; i++) {
    tmp[i] = str[i];
  }
  
  for(i = 1; i <= s; i++) {
    str[i - 1] = tmp[s - i];
  }

  free(tmp);
  tmp = NULL;

  return 0;
}

void vseek(FILE *fp, long offset, int whence, bool *is_valid)
{
  if(is_valid && fseek(fp, offset, whence) != 0)
    is_valid = false;
}

void vread(void *ptr, size_t s, FILE *fp, bool *is_valid)
{
  if(is_valid && fread(ptr, sizeof(unsigned char), s, fp) != s)
    is_valid = false;
}

void vrev(void *ptr, size_t s, bool *is_valid, bool *is_little_endian)
{
  if(is_valid && *is_little_endian && strrev(ptr, s) != 0)
    is_valid = false;
}

void vcmp(const unsigned char *compare, void *ptr, size_t s, bool *is_valid)
{
  if(is_valid && memcmp(compare, ptr, s) != 0)
    is_valid = false;
}

void read_exif_header(FILE *fp, unsigned char *buffer, bool *is_valid, bool *is_little_endian)
{
  //  SOI
  vread(buffer, 2, fp, is_valid);
  vcmp(SOI, buffer, 2, is_valid);

  //  APP1 Marker
  vread(buffer, 2, fp, is_valid);
  vcmp(APP1_MARKER, buffer, 2, is_valid);

  //  EXIF Identifier Code
  vseek(fp, 2, SEEK_CUR, is_valid);   //  Skip (APP1 Length)
  vread(buffer, 6, fp, is_valid);
  vcmp(EXIF_IDENT_CODE, buffer, 6, is_valid);

  //  Byte Order
  vread(buffer, 2, fp, is_valid);

  if(*is_valid && memcmp(TIFF_LITTLE_ENDIAN, buffer, 2) == 0)
    *is_little_endian = true;
  else if(*is_valid && memcmp(TIFF_BIG_ENDIAN, buffer, 2) == 0)
    *is_little_endian = false;
  else
    *is_valid = false;

  //  EXIF Fixed Code
  vread(buffer, 2, fp, is_valid);
  vrev(buffer, 2, is_valid, is_little_endian);
  vcmp(EXIF_FIXED_CODE, buffer, 2, is_valid);

  //  0th IFD Offset
  vread(buffer, 4, fp, is_valid);
  vrev(buffer, 4, is_valid, is_little_endian);

  //  0th IFD
  vseek(fp, todecimal(buffer, 4) - 8, SEEK_CUR, is_valid);

  //  0th IFD Entry Count
  vread(buffer, 2, fp, is_valid);
  vrev(buffer, 2, is_valid, is_little_endian);
}
