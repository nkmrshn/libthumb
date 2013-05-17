#ifndef LIBTHUMB_H
#define LIBTHUMB_H

typedef struct THUMB_DATA_tag {
  unsigned char *data;  //  JPEGInterchangeFormat 
  long length;          //  JPEGInterchangeFormatLength
  short orientation;    //  Orientation
} THUMB_DATA;

THUMB_DATA *thumb_read(char *);
void thumb_free(THUMB_DATA *);

#endif
